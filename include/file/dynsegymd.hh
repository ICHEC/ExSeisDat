/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
 *   \brief The state of this file is temporarily SEG-Y specific, when new formats are
 *          investigated this file should be split into a  format-agnostic component and a
 *          SEG-Y specific component.
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLFILEDYNSEGYMD_INCLUDE_GUARD
#define PIOLFILEDYNSEGYMD_INCLUDE_GUARD
#include "global.hh"
#include <unordered_map>
#include "file/file.hh"
#include "file/segymd.hh"
namespace PIOL { namespace File {
enum class Meta : size_t
{
    xSrc,
    ySrc,
    xRcv,
    yRcv,
    xCmp,
    yCmp,
    il,
    xl,
    tn,
//Non-standard
    dsdr
};

/*! Trace Header offsets
 */
enum class Tr : size_t
{
    SeqNum      = 1U,   //!< int32_t. The trace sequence number within the Line
    SeqFNum     = 5U,   //!< int32_t. The trace sequence number within SEG-Y File
    ORF         = 9U,   //!< int32_t. The original field record number.
    TORF        = 13U,  //!< int32_t. The trace number within the ORF.
    RcvElv      = 41U,  //!< int32_t. The Receiver group elevation
    SurfElvSrc  = 45U,  //!< int32_t. The surface elevation at the source.
    SrcDpthSurf = 49U,  //!< int32_t. The source depth below surface (opposite of above?).
    DtmElvRcv   = 53U,  //!< int32_t. The datum elevation for the receiver group.
    DtmElvSrc   = 57U,  //!< int32_t. The datum elevation for the source.
    WtrDepSrc   = 61U,  //!< int32_t. The water depth for the source.
    WtrDepRcv   = 65U,  //!< int32_t. The water depth for the receive group.
    ScaleElev   = 69U,  //!< int16_t. The scale coordinate for 41-68 (elevations + depths)
    ScaleCoord  = 71U,  //!< int16_t. The scale coordinate for 73-88 + 181-188
    xSrc        = 73U,  //!< int32_t. The X coordinate for the source
    ySrc        = 77U,  //!< int32_t. The Y coordinate for the source
    xRcv        = 81U,  //!< int32_t. The X coordinate for the receive group
    yRcv        = 85U,  //!< int32_t. The Y coordinate for the receive group
    UpSrc       = 91U,  //!< int16_t. The uphole time at the source. (ms)
    UpRcv       = 97U,  //!< int16_t. The uphole time at the receive group. (ms)
    xCmp        = 181U, //!< int32_t. The X coordinate for the CMP
    yCmp        = 185U, //!< int32_t. The Y coordinate for the CMP
    il          = 189U, //!< int32_t. The Inline grid point.
    xl          = 193U, //!< int32_t. The Crossline grid point.
    SrcMeas     = 225U, //!< int32_t. Source measurement
    SrcMeasExp  = 229U, //!< int32_t. Source measurement exponent
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

    operator size_t ()
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

#if defined(__INTEL_COMPILER) || __GNUC__ < 6    //Compiler defects
struct EnumHash
{
    template <typename T>
    size_t operator()(T t) const
    {
        return static_cast<size_t>(t);
    }
};
#endif

enum class MdType : size_t
{
    Long,
    Short,
    Float
};

struct RuleEntry
{
    size_t num;
    size_t loc;
    RuleEntry(size_t num_, size_t loc_) : num(num_), loc(loc_) { }
    virtual ~RuleEntry() { };
    virtual size_t min(void) = 0;
    virtual size_t max(void) = 0;
    virtual MdType type(void) = 0;
};

struct SEGYLongRuleEntry : public RuleEntry
{
    SEGYLongRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) { }
    size_t min(void)
    {
        return loc;
    }
    size_t max(void)
    {
        return loc+4U;
    }
    MdType type(void)
    {
        return MdType::Long;
    }
};

struct SEGYShortRuleEntry : public RuleEntry
{
    SEGYShortRuleEntry(size_t num_, Tr loc_) : RuleEntry(num_, size_t(loc_)) { }
    size_t min(void)
    {
        return loc;
    }
    size_t max(void)
    {
        return loc+2U;
    }
    MdType type(void)
    {
        return MdType::Short;
    }
};

struct SEGYFloatRuleEntry : public RuleEntry
{
    size_t scalLoc;
    SEGYFloatRuleEntry(size_t num_, Tr loc_, Tr scalLoc_)
                            : RuleEntry(num_, size_t(loc_)), scalLoc(size_t(scalLoc_)) { }
    size_t min(void)
    {
        return std::min(scalLoc, loc);
    }
    size_t max(void)
    {
        return std::max(scalLoc+2U, loc+4U);
    }
    MdType type(void)
    {
        return MdType::Float;
    }
};

//TODO: When implementing alternative file formats, this Rule structure must be generalised
#if defined(__INTEL_COMPILER) || __GNUC__ < 6    //Compiler defects
typedef std::unordered_map<Meta, RuleEntry *, EnumHash> RuleMap;
#else
typedef std::unordered_map<Meta, RuleEntry *> RuleMap;
#endif

struct Rule
{
    size_t numLong;     //Number of long rules
    size_t numFloat;    //Number of float rules
    size_t numShort;    //Number of short rules
    size_t start;
    size_t end;
    struct
    {
        uint32_t badextent;
        uint32_t fullextent;
    } flag;

    RuleMap translate;
    Rule(bool full, bool defaults);
    Rule(bool full, std::vector<Meta> & m);
    Rule(RuleMap translate_, bool full = true);
    ~Rule(void);

    //Rule setting
    void addLong(Meta m, Tr loc);
    void addFloat(Meta m, Tr loc, Tr scalLoc);
    void addShort(Meta m, Tr loc);
    void rmRule(Meta m);
    size_t extent(void);
    RuleEntry * getEntry(Meta entry);
};

//Access
prmRet getPrm(Rule * r, size_t i, Meta entry, const Param * prm);
void setPrm(Rule * r, size_t i, Meta entry, geom_t val, Param * prm);
void setPrm(Rule * r, size_t i, Meta entry, llint val, Param * prm);
void setPrm(Rule * r, size_t i, Meta entry, short val, Param * prm);
}}
#endif
