#include "global.hh"
#include <typeinfo>


struct RuleEntry
{
    csize_t loc;
    RuleEntry(csize_t loc_) : loc(loc_) { }
    virtual std::type_info type(void) const = 0;
};

struct IntRuleEntry : public RuleEntry
{
    IntRuleEntry(size_t loc_) : RuleEntry(loc_) { }
    std::type_info type(void) const
    {
        return typeid(int);
    }
};

struct ShortRuleEntry : public RuleEntry
{
    ShortRuleEntry(size_t loc_) : RuleEntry(loc_) { }
    std::type_info type(void) const
    {
        return typeid(short);
    }
};

struct FloatRuleEntry : public RuleEntry
{
    size_t scalLoc;
    FloatRuleEntry(size_t scalLoc_) : RuleEntry(loc_, typeid(geom_t)), scalLoc(scalLoc_) { }
    std::type_info type(void) const
    {
        return typeid(double);
    }
};

//A mechanism to store new rules
class Rule
{
    std::vector<IntRuleEntry *> intrules;
    std::vector<ShortRuleEntry *> shortrules;
    std::vector<FloatRuleEntry *> floatrules;
    public :
    Rule()
    {
        //TODO: Add defaults
    }
    ~Rule()
    {
        for (auto * rule : intrules)
            delete rule;
        for (auto * rule : shortrules)
            delete rule;
        for (auto * rule : floatrules)
            delete rule;
    }
    RuleEntry * addInt(csize_t loc)
    {
        intrules.push_back(loc);
        return intrules.front();
    }

    RuleEntry * addShort(csize_t loc)
    {
        shortrules.push_back(loc);
        return shortrules.front();
    }

    RuleEntry * addFloat(csize_t loc, csize_t scalLoc)
    {
        floatrules.push_back(loc, scalLoc);
        return floatrules.front();
    }
};

//C compatible structure
struct Param
{
    geom_t * flts;
    llint *  ints;
    short *  shts;
    size_t * tns;
};

struct prmRet
{
    union
    {
        llint i;
        geom_t f;
        short s;
    } val;
    operator long int ()
    {
        return val.i;
    }
    operator int ()
    {
        return val.i;
    }
    operator float ()
    {
        return val.f;
    }
    operator double ()
    {
        return val.f;
    }
    operator short ()
    {
        return val.s;
    }
};

class DynParam : public Param
{
    Rule * rules;
    Param * prm;

    public :
    prmRet getPrm(size_t i, RuleEntry * id)
    {
        prmRet ret;
        switch (id->type())
        {
            case typeid(int) :
            prm.i = 0;//TODO: Retrieve INT
            break;
            case typeid(short) :
            prm.s = 0;//TODO: Retrieve Short
            break;
            case typeid(double) :
            prm.f = 0;//TODO: Retrieve float
            break;
        }
        return ret;
    }
    void setPrm(size_t i, RuleEntry * id, geom_t val)
    {
    }
    void setPrm(size_t i, RuleEntry * id, llint val)
    {
    }
    void setPrm(size_t i, RuleEntry * id, short val)
    {
    }
};


