#include "mockrule.hh"

namespace PIOL {

Rule::Rule(bool full, bool defaults, bool extras)
{
    mockRule().ctor(this, full, defaults, extras);
}

Rule::Rule(const std::vector<Meta>& m, bool full, bool defaults, bool extras)
{
    mockRule().ctor(this, m, full, defaults, extras);
}

Rule::Rule(RuleMap translate_, bool full)
{
    mockRule().ctor(this, translate_, full);
}

Rule::~Rule()
{
    mockRule().dtor(this);
}

bool Rule::addRule(Meta m)
{
    return mockRule().addRule(this, m);
}

bool Rule::addRule(const Rule& r)
{
    return mockRule().addRule(this, r);
}

void Rule::addLong(Meta m, Tr loc)
{
    mockRule().addLong(this, m, loc);
}

void Rule::addSEGYFloat(Meta m, Tr loc, Tr scalLoc)
{
    mockRule().addSEGYFloat(this, m, loc, scalLoc);
}

void Rule::addShort(Meta m, Tr loc)
{
    mockRule().addShort(this, m, loc);
}

void Rule::addIndex(Meta m)
{
    mockRule().addIndex(this, m);
}

void Rule::addCopy()
{
    mockRule().addCopy(this);
}

void Rule::rmRule(Meta m)
{
    mockRule().rmRule(this, m);
}

size_t Rule::extent()
{
    return mockRule().extent(this);
}

size_t Rule::memUsage() const
{
    return mockRule().memUsage(this);
}

size_t Rule::paramMem() const
{
    return mockRule().paramMem(this);
}

RuleEntry* Rule::getEntry(Meta entry)
{
    return mockRule().getEntry(this, entry);
}

}  // namespace PIOL
