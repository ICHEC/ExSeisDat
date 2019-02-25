#include "exseisdat/flow/Set.hh"

#include "exseisdat/flow/Op.hh"
#include "exseisdat/flow/RadonGatherState.hh"

#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/WriteSEGY.hh"
#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/signal_processing/AGC.hh"

#include <glob.h>
#include <numeric>
#include <regex>

using namespace exseis::utils;
using namespace exseis::piol;

namespace exseis {
namespace flow {

/// Typedef for functions that have separate input and output of
/// traces/parameters
typedef std::function<void(const TraceBlock* in, TraceBlock* out)> Mod;

/// Typedef for functions that have the same input and output of
/// traces/parameters
typedef std::function<std::vector<size_t>(TraceBlock* data)> InPlaceMod;


Set::Set(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string pattern,
    std::string outfix,
    std::shared_ptr<Rule> rule) :
    m_piol(piol),
    m_outfix(outfix),
    m_rule(rule),
    m_cache(m_piol),
    m_rank(m_piol->comm->get_rank()),
    m_num_rank(m_piol->comm->get_num_rank())
{
    add(pattern);
}

Set::Set(std::shared_ptr<ExSeisPIOL> piol, std::shared_ptr<Rule> rule) :
    m_piol(piol),
    m_rule(rule),
    m_cache(m_piol),
    m_rank(m_piol->comm->get_rank()),
    m_num_rank(m_piol->comm->get_num_rank())
{
}

Set::~Set(void)
{
    if (!m_outfix.empty()) {
        output(m_outfix);
    }
}

void Set::add(std::unique_ptr<ReadInterface> in)
{
    m_file.emplace_back(std::make_shared<FileDesc>());
    auto& f = m_file.back();
    f->ifc  = std::move(in);

    auto dec = utils::block_decomposition(
        f->ifc->read_nt(), m_piol->comm->get_num_rank(),
        m_piol->comm->get_rank());

    f->ilst.resize(dec.local_size);
    f->olst.resize(dec.local_size);
    std::iota(f->ilst.begin(), f->ilst.end(), dec.global_offset);

    auto key = std::make_pair<size_t, exseis::utils::Floating_point>(
        f->ifc->read_ns(), f->ifc->read_sample_interval());
    m_fmap[key].emplace_back(f);

    auto& off = m_offmap[key];
    std::iota(f->olst.begin(), f->olst.end(), off + dec.global_offset);
    off += f->ifc->read_nt();
}

void Set::add(std::string pattern)
{
    m_outmsg = "ExSeisPIOL: Set layer output\n";
    glob_t globs;
    int err = glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK, nullptr, &globs);
    if (err != 0) {
        /// @todo: Is this std::exit ?!?
        exit(-1);
    }

    std::regex reg(
        ".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize
                        | std::regex::extended);

    for (size_t i = 0; i < globs.gl_pathc; i++) {
        // For each input file which matches the regex
        if (std::regex_match(globs.gl_pathv[i], reg)) {
            add(std::make_unique<ReadSEGY>(m_piol, globs.gl_pathv[i]));
        }
    }
    globfree(&globs);
    m_piol->assert_ok();
}

void Set::summary(void) const
{
    for (auto& f : m_file) {
        std::string msg =
            "name: " + f->ifc->file_name() + "\n"
            + "-\tNs: " + std::to_string(f->ifc->read_ns()) + "\n"
            + "-\tNt: " + std::to_string(f->ifc->read_nt()) + "\n" + "-\tInc: "
            + std::to_string(f->ifc->read_sample_interval()) + "\n";

        m_piol->log->add_entry(exseis::utils::Log_entry{
            exseis::utils::Status::Request, msg, exseis::utils::Verbosity::none,
            EXSEISDAT_SOURCE_POSITION("exseis::flow::Set::summary")});
    }

    if (m_rank == 0) {
        for (auto& m : m_fmap) {
            m_piol->log->add_entry(exseis::utils::Log_entry{
                exseis::utils::Status::Request,
                "Local File count for (" + std::to_string(m.first.first)
                    + " nt, " + std::to_string(m.first.second)
                    + " sample_interval) = " + std::to_string(m.second.size()),
                exseis::utils::Verbosity::none,
                EXSEISDAT_SOURCE_POSITION("exseis::flow::Set::summary")});
        }
    }

    m_piol->log->process_entries();
}


// TODO: Gather to Single is fine, Single to Gather is not
std::unique_ptr<TraceBlock> Set::calc_func(
    FuncLst::iterator f_curr,
    const FuncLst::iterator f_end,
    FuncOpt type,
    std::unique_ptr<TraceBlock> b_in)
{
    if (f_curr == f_end || !(*f_curr)->opt.check(type)) {
        return b_in;
    }

    if (type == FuncOpt::Gather) {
        if ((*f_curr)->opt.check(FuncOpt::Gather)) {
            auto b_out = std::make_unique<TraceBlock>();
            dynamic_cast<Op<Mod>*>(f_curr->get())
                ->func(b_in.get(), b_out.get());
            b_in = std::move(b_out);
        }

        ++f_curr;
    }

    if (type == FuncOpt::Gather || type == FuncOpt::SingleTrace) {
        if ((*f_curr)->opt.check(FuncOpt::SingleTrace)) {
            type = FuncOpt::SingleTrace;
            dynamic_cast<Op<InPlaceMod>*>(f_curr->get())->func(b_in.get());
        }
    }

    return calc_func(++f_curr, f_end, type, std::move(b_in));
}

std::vector<std::string> Set::start_single(
    FuncLst::iterator f_curr, const FuncLst::iterator f_end)
{
    std::vector<std::string> names;
    for (auto& o : m_fmap) {
        auto& f_que = o.second;

        std::string name;
        if (f_que.empty()) {
            return std::vector<std::string>{};
        }

        size_t ns_0 = f_que[0]->ifc->read_ns();
        exseis::utils::Floating_point sample_interval =
            f_que[0]->ifc->read_sample_interval();

        if (f_que.size() == 1) {
            name = m_outfix + ".segy";
        }
        else {
            name = m_outfix + std::to_string(ns_0) + "_"
                   + std::to_string(sample_interval) + ".segy";
        }

        names.push_back(name);

        WriteSEGY out(m_piol, name);
        // TODO: Will need to delay ns_0 call depending for operations that
        // modify
        //       the number of samples per trace
        out.write_ns(ns_0);
        out.write_sample_interval(sample_interval);
        out.write_text(m_outmsg);

        const size_t memlim = 1024LU * 1024LU * 1024LU;
        size_t max          = memlim
                     / (5LU * sizeof(size_t) + segy::segy_trace_size(ns_0)
                        + 2LU * m_rule->memory_usage_per_header()
                        + 2LU * segy::segy_trace_data_size(ns_0));

        for (auto& f : f_que) {
            ReadInterface* in = f->ifc.get();
            size_t lnt        = f->ilst.size();
            size_t ns         = in->read_ns();
            // Initialise the blocks
            auto biggest = m_piol->comm->max(lnt);
            size_t extra = biggest / max
                           + static_cast<size_t>(biggest % max > 0)
                           - (lnt / max + static_cast<size_t>(lnt % max > 0));

            for (size_t i = 0; i < lnt; i += max) {
                size_t rblock = (i + max < lnt ? max : lnt - i);
                std::vector<exseis::utils::Trace_value> trc(rblock * ns);
                /// @todo Rule should be made of rules stored in function list
                Trace_metadata prm(*m_rule, rblock);

                /// @todo Use non-monotonic call here
                in->read_trace_non_contiguous(
                    rblock, f->ilst.data() + i, trc.data(), &prm);

                std::vector<size_t> sortlist =
                    get_sort_index(rblock, f->olst.data() + i);

                auto b_in = std::make_unique<TraceBlock>();
                b_in->prm.reset(new Trace_metadata(*m_rule, rblock));
                b_in->trc.resize(rblock * ns);
                b_in->ns              = ns;
                b_in->sample_interval = sample_interval;

                for (size_t j = 0LU; j < rblock; j++) {
                    b_in->prm->copy_entries(j, prm, sortlist[j]);
                    for (size_t k = 0LU; k < ns; k++) {
                        b_in->trc[j * ns + k] = trc[sortlist[j] * ns + k];
                    }
                    sortlist[j] = f->olst[i + sortlist[j]];
                }

                auto b_final = calc_func(
                    f_curr, f_end, FuncOpt::SingleTrace, std::move(b_in));
                out.write_trace_non_contiguous(
                    rblock, sortlist.data(), b_final->trc.data(),
                    b_final->prm.get());
            }

            for (size_t i = 0; i < extra; i++) {
                // @todo Synchronization problem here with
                //       read_trace_non_contiguous and
                //       write_trace_non_contiguous. Tha branching for passing
                //       or not passing a param structure doesn't work well with
                //       the chunked and synchronized approach taken here.
                std::vector<exseis::utils::Trace_value> trc(0);
                Trace_metadata prm(*m_rule, 0);
                in->read_trace_non_contiguous(0, nullptr, trc.data(), &prm);

                auto b_in = std::make_unique<TraceBlock>();
                b_in->prm.reset(new Trace_metadata(*m_rule, 1LU));
                b_in->trc.resize(1LU);
                b_in->ns              = f->ifc->read_ns();
                b_in->nt              = 0LU;
                b_in->sample_interval = f->ifc->read_sample_interval();

                auto b_final =
                    calc_func(f_curr, f_end, FuncOpt::Gather, std::move(b_in));
                out.write_trace_non_contiguous(
                    0, nullptr, b_final->trc.data(), b_final->prm.get());
            }
        }
    }
    return names;
}

std::string Set::start_gather(
    FuncLst::iterator f_curr, const FuncLst::iterator f_end)
{
    if (m_file.size() > 1LU) {
        OpOpt opt = {FuncOpt::NeedMeta, FuncOpt::NeedTrcVal,
                     FuncOpt::SingleTrace};
        FuncLst t_func;
        t_func.push_back(std::make_shared<Op<InPlaceMod>>(
            opt, nullptr, nullptr, [](TraceBlock*) -> std::vector<size_t> {
                return std::vector<size_t>{};
            }));

        std::string t_outfix = m_outfix;
        m_outfix             = "temp";
        std::vector<std::string> names =
            calc_func(t_func.begin(), t_func.end());
        m_outfix = t_outfix;
        drop();
        for (std::string n : names) {
            // TODO: Open with delete on close?
            add(n);
        }
    }
    std::string gname;
    for (auto& o : m_file) {
        // Locate gather boundaries.
        auto gather = get_il_xl_gathers(m_piol.get(), o->ifc.get());
        auto gdec =
            utils::block_decomposition(gather.size(), m_num_rank, m_rank);

        size_t number_of_gathersather = gdec.local_size;

        std::vector<size_t> gather_numbers;
        for (auto f_temp = f_curr;
             f_temp != f_end && (*f_temp)->opt.check(FuncOpt::Gather);
             f_temp++) {
            auto* p = dynamic_cast<Op<Mod>*>(f_temp->get());
            assert(p);
            for (size_t i = 0; i < number_of_gathersather; i++) {
                gather_numbers.push_back(i * m_num_rank + m_rank);
            }

            p->state->make_state(gather_numbers, gather);
        }

        // TODO: Loop and add rules
        // TODO: need better rule handling, create rule of all rules in gather
        //       functions
        const auto gather_rule =
            Rule(std::initializer_list<Meta>{Meta::il, Meta::xl});

        auto f_temp = f_curr;
        while (++f_temp != f_end && (*f_temp)->opt.check(FuncOpt::Gather)) {
        }

        gname = (f_temp != f_end ? "gtemp.segy" : m_outfix + ".segy");

        // Use inputs as default values. These can be changed later
        WriteSEGY out(m_piol, gname);

        size_t w_offset = 0LU;
        size_t i_offset = 0LU;
        size_t extra =
            m_piol->comm->max(number_of_gathersather) - number_of_gathersather;
        size_t ig = 0;
        for (size_t gather_number : gather_numbers) {
            auto gval           = gather[gather_number];
            const size_t i_g_sz = gval.num_traces;

            // Initialise the blocks
            auto b_in = std::make_unique<TraceBlock>();
            b_in->prm.reset(new Trace_metadata(gather_rule, i_g_sz));
            b_in->trc.resize(i_g_sz * o->ifc->read_ns());
            b_in->ns                = o->ifc->read_ns();
            b_in->nt                = o->ifc->read_nt();
            b_in->sample_interval   = o->ifc->read_sample_interval();
            b_in->number_of_gathers = number_of_gathersather;
            b_in->gather_number     = ig++;

            size_t ioff = m_piol->comm->offset(i_g_sz);
            o->ifc->read_trace(
                i_offset + ioff, b_in->prm->size(), b_in->trc.data(),
                b_in->prm.get());
            i_offset += m_piol->comm->sum(i_g_sz);

            auto b_out =
                calc_func(f_curr, f_end, FuncOpt::Gather, std::move(b_in));

            size_t woff = m_piol->comm->offset(b_out->prm->size());
            // For simplicity, the output is now
            out.write_ns(b_out->ns);
            out.write_sample_interval(b_out->sample_interval);

            out.write_trace(
                w_offset + woff, b_out->prm->size(),
                (b_out->prm->size() != 0 ? b_out->trc.data() : nullptr),
                b_out->prm.get());

            w_offset += m_piol->comm->sum(b_out->prm->size());
        }
        for (size_t i = 0; i < extra; i++) {
            m_piol->comm->offset(0LU);
            o->ifc->read_trace(size_t(0), size_t(0), nullptr, nullptr);
            m_piol->comm->sum(0LU);

            auto b_in = std::make_unique<TraceBlock>();
            b_in->prm.reset(new Trace_metadata(gather_rule, 0LU));
            b_in->trc.resize(0LU);
            b_in->ns              = o->ifc->read_ns();
            b_in->nt              = o->ifc->read_nt();
            b_in->sample_interval = o->ifc->read_sample_interval();

            auto b_out =
                calc_func(f_curr, f_end, FuncOpt::Gather, std::move(b_in));
            out.write_ns(b_out->ns);
            out.write_sample_interval(b_out->sample_interval);

            m_piol->comm->offset(0LU);
            out.write_trace(size_t(0), size_t(0), nullptr, nullptr);
            m_piol->comm->sum(0LU);
        }
    }

    while (++f_curr != f_end && (*f_curr)->opt.check(FuncOpt::Gather)) {
    }

    if (f_curr != f_end) {
        drop();
        add(gname);
        return "";
    }
    else {
        return gname;
    }
}

// calc for subsets only
Set::FuncLst::iterator Set::calc_func_s(
    FuncLst::iterator f_curr, const FuncLst::iterator f_end, FileDeque& f_que)
{
    std::shared_ptr<TraceBlock> block;

    if ((*f_curr)->opt.check(FuncOpt::NeedMeta)) {
        if (!(*f_curr)->opt.check(FuncOpt::NeedTrcVal)) {
            block = m_cache.cache_prm((*f_curr)->rule, f_que);
        }
        else {
            m_piol->log->add_entry(exseis::utils::Log_entry{
                exseis::utils::Status::Error,
                "Not implemented both trace + parameters yet",
                exseis::utils::Verbosity::none,
                EXSEISDAT_SOURCE_POSITION("exseis::flow::Set::calc_func_s")});
        }
    }
    else if ((*f_curr)->opt.check(FuncOpt::NeedTrcVal)) {
        block = m_cache.cache_trc(f_que);
    }

    // The operation call
    std::vector<size_t> trlist =
        dynamic_cast<Op<InPlaceMod>*>(f_curr->get())->func(block.get());

    size_t j = 0;
    for (auto& f : f_que) {
        std::copy(&trlist[j], &trlist[j + f->olst.size()], f->olst.begin());
        j += f->olst.size();
    }

    if (++f_curr != f_end) {
        if ((*f_curr)->opt.check(FuncOpt::SubSetOnly)) {
            return calc_func_s(f_curr, f_end, f_que);
        }
    }
    return f_curr;
}

Set::FuncLst::iterator Set::start_subset(
    FuncLst::iterator f_curr, const FuncLst::iterator f_end)
{
    std::vector<FuncLst::iterator> flist;

    // TODO: Parallelisable
    for (auto& o : m_fmap) {
        // Iterate across the full function list
        flist.push_back(calc_func_s(f_curr, f_end, o.second));
    }

    assert(std::equal(flist.begin() + 1LU, flist.end(), flist.begin()));

    return flist.front();
}

std::vector<std::string> Set::calc_func(
    FuncLst::iterator f_curr, const FuncLst::iterator f_end)
{
    if (f_curr != f_end) {
        if ((*f_curr)->opt.check(FuncOpt::SubSetOnly)) {
            f_curr = start_subset(f_curr, f_end);
        }
        else if ((*f_curr)->opt.check(FuncOpt::Gather)) {
            std::string gname = start_gather(f_curr, f_end);

            // If startGather returned an empty string, then fCurr == fEnd was
            // reached.
            if (!gname.empty()) {
                return std::vector<std::string>{gname};
            }

            /// @todo Later this will need to be changed when the gather also
            ///       continues with single trace cases
            // Skip fCurr where check(FuncOpt::Gather) is ok
            for (; f_curr != f_end; ++f_curr) {
                if (!(*f_curr)->opt.check(FuncOpt::Gather)) {
                    break;
                }
            }
        }
        else if ((*f_curr)->opt.check(FuncOpt::SingleTrace)) {
            std::vector<std::string> sname = start_single(f_curr, f_end);

            if (!sname.empty()) {
                return sname;
            }

            for (; f_curr != f_end; ++f_curr) {
                if (!(*f_curr)->opt.check(FuncOpt::SingleTrace)) {
                    break;
                }
            }
        }
        else {
            m_piol->log->add_entry(exseis::utils::Log_entry{
                exseis::utils::Status::Error,
                "Error, not supported yet. TODO: Implement support for a truly global operation.",
                exseis::utils::Verbosity::none,
                EXSEISDAT_SOURCE_POSITION("exseis::flow::Set::calc_func")});

            ++f_curr;
        }

        calc_func(f_curr, f_end);
    }

    return std::vector<std::string>{};
}

std::vector<std::string> Set::output(std::string oname)
{
    OpOpt opt = {FuncOpt::NeedMeta, FuncOpt::NeedTrcVal, FuncOpt::SingleTrace};
    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, nullptr, nullptr, [](TraceBlock*) -> std::vector<size_t> {
            return std::vector<size_t>{};
        }));

    m_outfix                     = oname;
    std::vector<std::string> out = calc_func(m_func.begin(), m_func.end());
    m_func.clear();

    return out;
}

void Set::get_min_max(
    MinMaxFunc<Trace_metadata> xlam,
    MinMaxFunc<Trace_metadata> ylam,
    CoordElem* minmax)
{
    // TODO: This needs to be changed to be compatible with ExSeisFlow
    minmax[0].val = std::numeric_limits<exseis::utils::Floating_point>::max();
    minmax[1].val = std::numeric_limits<exseis::utils::Floating_point>::min();
    minmax[2].val = std::numeric_limits<exseis::utils::Floating_point>::max();
    minmax[3].val = std::numeric_limits<exseis::utils::Floating_point>::min();
    for (size_t i = 0; i < 4; i++) {
        minmax[i].num = std::numeric_limits<size_t>::max();
    }

    CoordElem tminmax[4LU];

    for (auto& f : m_file) {
        std::vector<Trace_metadata> vprm;
        Trace_metadata prm(*m_rule, f->ilst.size());

        f->ifc->read_param_non_contiguous(f->ilst.size(), f->ilst.data(), &prm);

        for (size_t i = 0; i < f->ilst.size(); i++) {
            vprm.emplace_back(*m_rule, 1LU);
            vprm.back().copy_entries(0, prm, i);
        }
        // TODO: Minmax can't assume ordered data! Fix this!
        size_t offset = m_piol->comm->offset(f->ilst.size());
        piol::get_min_max(
            m_piol.get(), offset, f->ilst.size(), vprm.data(), xlam, ylam,
            tminmax);
        for (size_t i = 0LU; i < 2LU; i++) {

            {
                auto& tmm = tminmax[2LU * i];
                auto& mm  = minmax[2LU * i];
                if (tmm.val < mm.val) {
                    mm = tmm;
                }
                else if (tmm.val > mm.val) {
                    // do nothing
                }
                else {
                    // tmm.val == mm.val
                    mm.num = std::min(tmm.num, mm.num);
                }
            }

            {
                auto& tmm2 = tminmax[2LU * i + 1LU];
                auto& mm2  = minmax[2LU * i + 1LU];
                if (tmm2.val > mm2.val) {
                    mm2 = tmm2;
                }
                else if (tmm2.val > mm2.val) {
                    // do nothing
                }
                else {
                    // tmm2.val == mm2.val
                    mm2.num = std::min(tmm2.num, mm2.num);
                }
            }
        }
    }
}

void Set::sort(CompareP sort_func)
{
    auto r = std::make_shared<Rule>(std::initializer_list<Meta>{
        Meta::il, Meta::xl, Meta::x_src, Meta::y_src, Meta::x_rcv, Meta::y_rcv,
        Meta::xCmp, Meta::yCmp, Meta::Offset, Meta::WtrDepRcv, Meta::tn});

    // TODO: This is not the ideal mechanism, hack for now. See the note in the
    //       calc_func for single traces
    m_rule->add_rule(*r);
    sort(r, sort_func);
}

void Set::sort(std::shared_ptr<Rule> r, CompareP sort_func)
{
    OpOpt opt = {FuncOpt::NeedMeta, FuncOpt::ModMetaVal, FuncOpt::DepMetaVal,
                 FuncOpt::SubSetOnly};

    m_func.push_back(std::make_shared<Op<InPlaceMod>>(
        opt, r, nullptr,
        [this, sort_func](TraceBlock* in) -> std::vector<size_t> {
            // TODO: It will eventually be necessary to support this use case.
            if (m_piol->comm->min(in->prm->size()) < 3LU) {
                m_piol->log->add_entry(exseis::utils::Log_entry{
                    exseis::utils::Status::Error,
                    "Sorting small files with multiple processes not supported!",
                    exseis::utils::Verbosity::none,
                    EXSEISDAT_SOURCE_POSITION("exseis::flow::Set::sort")});

                return std::vector<size_t>{};
            }
            else {
                return piol::sort(m_piol.get(), *(in->prm), sort_func);
            }
        }));
}

void Set::to_angle(
    std::string vm_name,
    const size_t v_bin,
    const size_t output_traces_per_gather,
    exseis::utils::Floating_point output_sample_interval)
{
    OpOpt opt  = {FuncOpt::NeedMeta,   FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal,
                 FuncOpt::ModMetaVal, FuncOpt::DepTrcVal,  FuncOpt::DepTrcOrder,
                 FuncOpt::DepTrcCnt,  FuncOpt::DepMetaVal, FuncOpt::Gather};
    auto state = std::make_shared<RadonGatherState>(
        m_piol, vm_name, v_bin, output_traces_per_gather,
        output_sample_interval);
    m_func.emplace_back(std::make_shared<Op<Mod>>(
        opt, m_rule, state, [state](const TraceBlock* in, TraceBlock* out) {
            const size_t i_g_sz = in->prm->size();
            out->ns             = in->ns;
            out->sample_interval =
                state->output_sample_interval;  // 1 degree in radians
            out->trc.resize(state->output_traces_per_gather * out->ns);
            out->prm.reset(new Trace_metadata(
                in->prm->rules, state->output_traces_per_gather));
            if (in->prm->size() == 0) {
                return;
            }

            // For each angle in the angle gather
            for (size_t j = 0; j < state->output_traces_per_gather; j++) {
                // For each sample (angle + radon)
                for (size_t z = 0; z < in->ns; z++) {
                    // We are using coordinate level accuracy when its not
                    // performance critical.
                    const exseis::utils::Floating_point vm_model = static_cast<
                        exseis::utils::Floating_point>(
                        state->vtrc
                            [in->gather_number * state->v_ns
                             + std::min(
                                 size_t(
                                     exseis::utils::Floating_point(
                                         z * in->sample_interval)
                                     / state->velocity_model_sample_interval),
                                 state->v_ns)]);

                    const auto k =
                        llround(
                            vm_model
                            / cos(exseis::utils::Floating_point(
                                j * out->sample_interval)))
                        / static_cast<exseis::utils::Integer>(state->v_bin);

                    if (k > 0 && static_cast<size_t>(k) < i_g_sz) {
                        out->trc[j * out->ns + z] =
                            in->trc[static_cast<size_t>(k) * in->ns + z];
                    }
                }
            }

            for (size_t j = 0; j < state->output_traces_per_gather; j++) {
                // TODO: Set the rest of the parameters
                // TODO: Check the get numbers
                out->prm->set_integer(
                    j, Meta::il, state->il[in->gather_number]);
                out->prm->set_integer(
                    j, Meta::xl, state->xl[in->gather_number]);
            }
        }));
}


void Set::taper(
    Taper_function taper_function,
    size_t taper_size_at_begin,
    size_t taper_size_at_end)
{
    OpOpt opt = {FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal, FuncOpt::DepTrcVal,
                 FuncOpt::SingleTrace};
    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, m_rule, nullptr, [=](TraceBlock* in) -> std::vector<size_t> {
            // Loop over traces and apply the taper to each one
            for (size_t i = 0; i < in->prm->size(); i++) {

                const auto trace_length = in->ns;

                auto* trace_i = &(in->trc[i * trace_length]);

                exseis::utils::taper(
                    trace_length, trace_i, taper_function, taper_size_at_begin,
                    taper_size_at_end);
            }

            return std::vector<size_t>{};
        }));
}

void Set::mute(
    Taper_function taper_function,
    size_t mute_size_at_begin,
    size_t taper_size_at_begin,
    size_t taper_size_at_end,
    size_t mute_size_at_end)
{
    OpOpt opt = {FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal, FuncOpt::DepTrcVal,
                 FuncOpt::SingleTrace};
    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, m_rule, nullptr, [=](TraceBlock* in) -> std::vector<size_t> {
            // Loop over traces and apply the taper to each one
            for (size_t i = 0; i < in->prm->size(); i++) {

                const auto trace_length = in->ns;

                auto* trace_i = &(in->trc[i * trace_length]);

                exseis::utils::mute(
                    trace_length, trace_i, taper_function, mute_size_at_begin,
                    taper_size_at_begin, taper_size_at_end, mute_size_at_end);
            }

            return std::vector<size_t>{};
        }));
}

void Set::agc(
    Gain_function agc_func,
    size_t window,
    exseis::utils::Trace_value target_amplitude)
{
    OpOpt opt = {FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal, FuncOpt::DepTrcVal,
                 FuncOpt::SingleTrace};

    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, m_rule, nullptr,
        [agc_func, window,
         target_amplitude](TraceBlock* in) -> std::vector<size_t> {
            const auto num_traces        = in->prm->size();
            const auto samples_per_trace = in->ns;

            for (size_t i = 0; i < num_traces; i++) {
                // Pointer to the beginning of trace i.
                auto* trace_start = &(in->trc[i * in->ns]);

                exseis::utils::agc(
                    samples_per_trace, trace_start, agc_func, window,
                    target_amplitude);
            }
            return std::vector<size_t>{};
        }));
}

void Set::text(std::string outmsg)
{
    m_outmsg = outmsg;
}

/********************************** Non-Core **********************************/
void Set::sort(SortType type)
{
    Set::sort(get_comp(type));
}

void Set::get_min_max(Meta m1, Meta m2, CoordElem* minmax)
{
    /// @todo: This is very likely broken

    bool m1_add = m_rule->add_rule(m1);
    bool m2_add = m_rule->add_rule(m2);

    Set::get_min_max(
        [m1](const Trace_metadata& a) -> exseis::utils::Floating_point {
            return a.get_floating_point(0LU, m1);
        },
        [m2](const Trace_metadata& a) -> exseis::utils::Floating_point {
            return a.get_floating_point(0LU, m2);
        },
        minmax);

    if (m1_add) {
        m_rule->rm_rule(m1);
    }
    if (m2_add) {
        m_rule->rm_rule(m2);
    }
}

void Set::temporal_filter(
    FltrType type,
    FltrDmn domain,
    PadType pad,
    exseis::utils::Trace_value fs,
    std::vector<exseis::utils::Trace_value> corners,
    size_t nw,
    size_t win_cntr)
{
    assert(corners.size() == 2);
    OpOpt opt = {FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal, FuncOpt::DepTrcVal,
                 FuncOpt::SingleTrace};
    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, m_rule, nullptr,
        [type, domain, corners, pad, fs, nw,
         win_cntr](TraceBlock* in) -> std::vector<size_t> {
            piol::temporal_filter(
                in->prm->size(), in->ns, in->trc.data(), fs, type, domain, pad,
                nw, win_cntr, corners);
            return std::vector<size_t>{};
        }));
}

void Set::temporal_filter(
    FltrType type,
    FltrDmn domain,
    PadType pad,
    exseis::utils::Trace_value fs,
    size_t n,
    std::vector<exseis::utils::Trace_value> corners,
    size_t nw,
    size_t win_cntr)
{
    assert(corners.size() == 2);
    OpOpt opt = {FuncOpt::NeedTrcVal, FuncOpt::ModTrcVal, FuncOpt::DepTrcVal,
                 FuncOpt::SingleTrace};
    m_func.emplace_back(std::make_shared<Op<InPlaceMod>>(
        opt, m_rule, nullptr,
        [type, domain, corners, pad, fs, nw, win_cntr,
         n](TraceBlock* in) -> std::vector<size_t> {
            piol::temporal_filter(
                in->prm->size(), in->ns, in->trc.data(), fs, type, domain, pad,
                nw, win_cntr, corners, n);
            return std::vector<size_t>{};
        }));
}

}  // namespace flow
}  // namespace exseis
