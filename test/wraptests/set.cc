#include "mockset.hh"

namespace PIOL {

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

void Set::taper(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt)
{
    mockSet().taper(this, tapFunc, nTailLft, nTailRt);
}

void Set::AGC(Gain_function agcFunc, size_t window, trace_t target_amplitude)
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
  std::string vmName, const size_t vBin, const size_t oGSz, geom_t oInc)
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

void Set::taper(TaperType type, size_t nTailLft, size_t nTailRt)
{
    mockSet().taper(this, type, nTailLft, nTailRt);
}

void Set::temporalFilter(
  FltrType type,
  FltrDmn domain,
  PadType pad,
  trace_t fs,
  std::vector<trace_t> corners,
  size_t nw,
  size_t winCntr)
{
    mockSet().temporalFilter(this, type, domain, pad, fs, corners, nw, winCntr);
}

void Set::temporalFilter(
  FltrType type,
  FltrDmn domain,
  PadType pad,
  trace_t fs,
  size_t N,
  std::vector<trace_t> corners,
  size_t nw,
  size_t winCntr)
{
    mockSet().temporalFilter(
      this, type, domain, pad, fs, N, corners, nw, winCntr);
}

}  // namespace PIOL
