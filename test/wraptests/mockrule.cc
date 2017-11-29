#include "mockrule.hh"

namespace PIOL {

MockRule& MockRule::instance()
{
    static MockRule instance;
    return instance;
}

Rule::Rule(bool full, bool defaults, bool extras)
{
    MockRule::instance().ctor(this, full, defaults, extras);
}

Rule::Rule(const std::vector<Meta>& m, bool full, bool defaults, bool extras)
{
    MockRule::instance().ctor(this, m, full, defaults, extras);
}

Rule::Rule(RuleMap translate_, bool full)
{
    MockRule::instance().ctor(this, translate_, full);
}

Rule::~Rule()
{
    MockRule::instance().dtor(this);
}

bool Rule::addRule(Meta m)
{
    return MockRule::instance().addRule(this, m);
}

bool Rule::addRule(const Rule& r)
{
    return MockRule::instance().addRule(this, r);
}

void Rule::addLong(Meta m, Tr loc)
{
    MockRule::instance().addLong(this, m, loc);
}

void Rule::addSEGYFloat(Meta m, Tr loc, Tr scalLoc)
{
    MockRule::instance().addSEGYFloat(this, m, loc, scalLoc);
}

void Rule::addShort(Meta m, Tr loc)
{
    MockRule::instance().addShort(this, m, loc);
}

void Rule::addIndex(Meta m)
{
    MockRule::instance().addIndex(this, m);
}

void Rule::addCopy()
{
    MockRule::instance().addCopy(this);
}

void Rule::rmRule(Meta m)
{
    MockRule::instance().rmRule(this, m);
}

size_t Rule::extent()
{
    return MockRule::instance().extent(this);
}

size_t Rule::memUsage() const
{
    return MockRule::instance().memUsage(this);
}

size_t Rule::paramMem() const
{
    return MockRule::instance().paramMem(this);
}

RuleEntry * Rule::getEntry(Meta entry)
{
    return MockRule::instance().getEntry(this, entry);
}

} // namespace PIOL
