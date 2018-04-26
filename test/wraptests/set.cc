#include "mockset.hh"

using namespace exseis::utils;
using namespace exseis::PIOL;

namespace exseis {
namespace Flow {

Set::Set(
  std::shared_ptr<ExSeisPIOL> piol_,
  std::string pattern,
  std::string outfix_,
  std::shared_ptr<Rule> rule_) :
    rule(rule_),
    cache(piol_)
{
    mockSet().ctor(this, piol_, pattern, outfix_, rule_);
}

Set::Set(std::shared_ptr<ExSeisPIOL> piol_, std::shared_ptr<Rule> rule_) :
    rule(rule_),
    cache(piol_)
{
    mockSet().ctor(this, piol_, rule_);
}

Set::~Set()
{
    mockSet().dtor(this);
}

void Set::sort(CompareP sortFunc)
{
    mockSet().sort(this, sortFunc);
}

void Set::sort(std::shared_ptr<Rule> r, CompareP sortFunc)
{
    mockSet().sort(this, r, sortFunc);
}

std::vector<std::string> Set::output(std::string oname)
{
    return mockSet().output(this, oname);
}

void Set::getMinMax(
  MinMaxFunc<Param> xlam, MinMaxFunc<Param> ylam, CoordElem* minmax)
{
    mockSet().getMinMax(this, xlam, ylam, minmax);
}

void Set::taper(Taper_function taper_function, size_t nTailLft, size_t nTailRt)
{
    mockSet().taper(this, taper_function, nTailLft, nTailRt);
}

void Set::AGC(
  Gain_function agcFunc,
  size_t window,
  exseis::utils::Trace_value target_amplitude)
{
    mockSet().AGC(this, agcFunc, window, target_amplitude);
}

void Set::text(std::string outmsg_)
{
    mockSet().text(this, outmsg_);
}

void Set::summary() const
{
    mockSet().summary(this);
}

void Set::add(std::unique_ptr<ReadInterface> in)
{
    mockSet().add(this, std::move(in));
}

void Set::add(std::string name)
{
    mockSet().add(this, name);
}

void Set::toAngle(
  std::string vmName,
  const size_t vBin,
  const size_t oGSz,
  exseis::utils::Floating_point oInc)
{
    mockSet().toAngle(this, vmName, vBin, oGSz, oInc);
}

void Set::sort(SortType type)
{
    mockSet().sort(this, type);
}

void Set::getMinMax(Meta m1, Meta m2, CoordElem* minmax)
{
    mockSet().getMinMax(this, m1, m2, minmax);
}

void Set::temporalFilter(
  FltrType type,
  FltrDmn domain,
  PadType pad,
  exseis::utils::Trace_value fs,
  std::vector<exseis::utils::Trace_value> corners,
  size_t nw,
  size_t winCntr)
{
    mockSet().temporalFilter(this, type, domain, pad, fs, corners, nw, winCntr);
}

void Set::temporalFilter(
  FltrType type,
  FltrDmn domain,
  PadType pad,
  exseis::utils::Trace_value fs,
  size_t N,
  std::vector<exseis::utils::Trace_value> corners,
  size_t nw,
  size_t winCntr)
{
    mockSet().temporalFilter(
      this, type, domain, pad, fs, N, corners, nw, winCntr);
}

}  // namespace Flow
}  // namespace exseis
