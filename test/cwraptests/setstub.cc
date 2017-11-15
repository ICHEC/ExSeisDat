#include "stubtools.hh"
#include "setstub.hh"

std::function<
    void(PIOL::Piol piol_, std::string pattern, std::string outfix_,
         std::shared_ptr<PIOL::File::Rule> rule_)
> PIOL_Set_new_1_cb;
PIOL::Set::Set(Piol piol_, std::string pattern, std::string outfix_,
               std::shared_ptr<File::Rule> rule_):
    cache(piol_)
{
    if(PIOL_Set_new_1_cb) {
        PIOL_Set_new_1_cb(piol_, pattern, outfix_, rule_);
    }
}

std::function<
    void(PIOL::Piol piol_, std::shared_ptr<PIOL::File::Rule> rule_)
> PIOL_Set_new_2_cb;
PIOL::Set::Set(Piol piol_, std::shared_ptr<File::Rule> rule_):
    cache(piol_)
{
    if(PIOL_Set_new_2_cb) {
        PIOL_Set_new_2_cb(piol_, rule_);
    }
}


MAKE_STUB_0_CTOR(PIOL::Set::~Set, PIOL_Set_delete)
//std::function<void()> PIOL_Set_delete_cb;
//PIOL::Set::~Set(void)
//{
//    if(PIOL_Set_delete_cb) {
//        PIOL_Set_delete_cb();
//    }
//}

std::function<void(PIOL::CompareP)> PIOL_Set_sort_1_cb;
void PIOL::Set::sort(CompareP sortFunc)
{
    if(PIOL_Set_sort_1_cb) {
        PIOL_Set_sort_1_cb(sortFunc);
    }
}

std::function<void(std::shared_ptr<PIOL::File::Rule>, PIOL::CompareP)> PIOL_Set_sort_2_cb;
void PIOL::Set::sort(std::shared_ptr<File::Rule> r, CompareP sortFunc)
{
    if(PIOL_Set_sort_2_cb) {
        PIOL_Set_sort_2_cb(r, sortFunc);
    }
}

std::function<std::vector<std::string>(std::string)> PIOL_Set_output_cb;
std::vector<std::string> PIOL::Set::output(std::string oname)
{
    if(PIOL_Set_output_cb) {
        return PIOL_Set_output_cb(oname);
    }
    return std::vector<std::string>();
}

std::function<
    void(MinMaxFunc<PIOL::File::Param> xlam,
         MinMaxFunc<PIOL::File::Param> ylam, CoordElem * minmax)
> PIOL_Set_getMinMax_1_cb;
void PIOL::Set::getMinMax(MinMaxFunc<File::Param> xlam,
                          MinMaxFunc<File::Param> ylam, CoordElem * minmax)
{
    if(PIOL_Set_getMinMax_1_cb) {
        PIOL_Set_getMinMax_1_cb(xlam, ylam, minmax);
    }
}

std::function<void(Meta m1, Meta m2, CoordElem * minmax)> PIOL_Set_getMinMax_2_cb;
void PIOL::Set::getMinMax(Meta m1, Meta m2, CoordElem * minmax)
{
    if(PIOL_Set_getMinMax_2_cb) {
        PIOL_Set_getMinMax_2_cb(m1, m2, minmax);
    }
}


std::function<void(std::string outmsg_)> PIOL_Set_text_cb;
void PIOL::Set::text(std::string outmsg_)
{
    if(PIOL_Set_text_cb) {
        PIOL_Set_text_cb(outmsg_);
    }
}

std::function<void(void)> PIOL_Set_summary_cb;
void PIOL::Set::summary(void) const
{
    if(PIOL_Set_summary_cb) {
        PIOL_Set_summary_cb();
    }
}

std::function<void(std::unique_ptr<PIOL::File::ReadInterface> in)> PIOL_Set_add_1_cb;
void PIOL::Set::add(std::unique_ptr<File::ReadInterface> in)
{
    if(PIOL_Set_add_1_cb) {
        PIOL_Set_add_1_cb(std::move(in));
    }
}

std::function<void(std::string name)> PIOL_Set_add_2_cb;
void PIOL::Set::add(std::string name)
{
    if(PIOL_Set_add_2_cb) {
        PIOL_Set_add_2_cb(name);
    }
}

std::function<
    void(std::string vmName, csize_t vBin, csize_t oGSz, geom_t oInc)
> PIOL_Set_toAngle_cb;
void PIOL::Set::toAngle(std::string vmName, csize_t vBin, csize_t oGSz,
                        geom_t oInc)
{
    if(PIOL_Set_toAngle_cb) {
        PIOL_Set_toAngle_cb(vmName, vBin, oGSz, oInc);
    }
}

std::function<void(SortType type)> PIOL_Set_sort_cb;
void PIOL::Set::sort(SortType type)
{
    if(PIOL_Set_sort_cb) {
        PIOL_Set_sort_cb(type);
    }
}

std::function<void(TaperType type, size_t nTailLft, size_t nTailRt)> PIOL_Set_taper_1_cb;
void PIOL::Set::taper(TaperType type, size_t nTailLft, size_t nTailRt)
{
    if(PIOL_Set_taper_1_cb) {
        PIOL_Set_taper_1_cb(type, nTailLft, nTailRt);
    }
}

std::function<void(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt)> PIOL_Set_taper_2_cb;
void PIOL::Set::taper(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt)
{
    if(PIOL_Set_taper_2_cb) {
        PIOL_Set_taper_2_cb(tapFunc, nTailLft, nTailRt);
    }
}

std::function<void(AGCType type, size_t window, trace_t normR)> PIOL_Set_AGC_1_cb;
void PIOL::Set::AGC(AGCType type, size_t window, trace_t normR)
{
    if(PIOL_Set_AGC_1_cb) {
        PIOL_Set_AGC_1_cb(type, window, normR);
    }
}

std::function<void(AGCFunc agcFunc, size_t window, trace_t normR)> PIOL_Set_AGC_2_cb;
void PIOL::Set::AGC(AGCFunc agcFunc, size_t window, trace_t normR)
{
    if(PIOL_Set_AGC_2_cb) {
        PIOL_Set_AGC_2_cb(agcFunc, window, normR);
    }
}


std::function<void(PIOL::FltrType type, PIOL::FltrDmn domain, PIOL::PadType pad,
                   trace_t fs, std::vector<trace_t> corners,
                   size_t nw, size_t winCntr)
> PIOL_Set_temporalFilter_1_cb;
void PIOL::Set::temporalFilter(FltrType type, FltrDmn domain, PadType pad,
                               trace_t fs, std::vector<trace_t> corners,
                               size_t nw, size_t winCntr)
{
    if(PIOL_Set_temporalFilter_1_cb) {
        PIOL_Set_temporalFilter_1_cb(type, domain, pad, fs, corners, nw, winCntr);
    }
}

std::function<void(PIOL::FltrType type, PIOL::FltrDmn domain, PIOL::PadType pad,
                   trace_t fs, size_t N,
                   std::vector<trace_t> corners, size_t nw, size_t winCntr)
> PIOL_Set_temporalFilter_2_cb;
void PIOL::Set::temporalFilter(FltrType type, FltrDmn domain, PadType pad,
                               trace_t fs, size_t N,
                               std::vector<trace_t> corners, size_t nw,
                               size_t winCntr)
{
    if(PIOL_Set_temporalFilter_2_cb) {
        PIOL_Set_temporalFilter_2_cb(type, domain, pad, fs, N, corners, nw, winCntr);
    }
}
