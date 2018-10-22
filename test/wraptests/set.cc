#include "mockset.hh"

using namespace exseis::utils;
using namespace exseis::piol;

namespace exseis {
namespace flow {

Set::Set(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string pattern,
  std::string outfix,
  std::shared_ptr<Rule> rule) :
    m_rule(rule),
    m_cache(piol)
{
    mock_set().ctor(this, piol, pattern, outfix, rule);
}

Set::Set(std::shared_ptr<ExSeisPIOL> piol, std::shared_ptr<Rule> rule) :
    m_rule(rule),
    m_cache(piol)
{
    mock_set().ctor(this, piol, rule);
}

Set::~Set()
{
    mock_set().dtor(this);
}

void Set::sort(CompareP sort_func)
{
    mock_set().sort(this, sort_func);
}

void Set::sort(std::shared_ptr<Rule> r, CompareP sort_func)
{
    mock_set().sort(this, r, sort_func);
}

std::vector<std::string> Set::output(std::string oname)
{
    return mock_set().output(this, oname);
}

void Set::get_min_max(
  MinMaxFunc<Trace_metadata> xlam,
  MinMaxFunc<Trace_metadata> ylam,
  CoordElem* minmax)
{
    mock_set().get_min_max(this, xlam, ylam, minmax);
}

void Set::taper(
  Taper_function taper_function, size_t n_tail_lft, size_t n_tail_rt)
{
    mock_set().taper(this, taper_function, n_tail_lft, n_tail_rt);
}

void Set::agc(
  Gain_function agc_func,
  size_t window,
  exseis::utils::Trace_value target_amplitude)
{
    mock_set().agc(this, agc_func, window, target_amplitude);
}

void Set::text(std::string outmsg)
{
    mock_set().text(this, outmsg);
}

void Set::summary() const
{
    mock_set().summary(this);
}

void Set::add(std::unique_ptr<ReadInterface> in)
{
    mock_set().add(this, std::move(in));
}

void Set::add(std::string name)
{
    mock_set().add(this, name);
}

void Set::to_angle(
  std::string vm_name,
  const size_t v_bin,
  const size_t output_traces_per_gather,
  exseis::utils::Floating_point output_sample_interval)
{
    mock_set().toAngle(
      this, vm_name, v_bin, output_traces_per_gather, output_sample_interval);
}

void Set::sort(SortType type)
{
    mock_set().sort(this, type);
}

void Set::get_min_max(Meta m1, Meta m2, CoordElem* minmax)
{
    mock_set().get_min_max(this, m1, m2, minmax);
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
    mock_set().temporal_filter(
      this, type, domain, pad, fs, corners, nw, win_cntr);
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
    mock_set().temporal_filter(
      this, type, domain, pad, fs, n, corners, nw, win_cntr);
}

}  // namespace flow
}  // namespace exseis
