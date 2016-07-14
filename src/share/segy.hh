#ifndef PIOLSHARESEGY_INCLUDE_GUARD
#define PIOLSHARESEGY_INCLUDE_GUARD
namespace PIOL { namespace SEGSz {
enum class Size : size_t
{
    HO = 3600,
    Md = 240
};

constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::Md);
}
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

template <typename T = float>
inline size_t getDOSz(size_t ns)
{
    return getMDSz()+ns*sizeof(T);
}
template <typename T = float>
inline size_t getDOLoc(size_t i, size_t ns)
{
    return getHOSz() + i*getDOSz<T>(ns);
}

template <typename T = float>
inline size_t getDODFLoc(size_t i, size_t ns)
{
    return getDOLoc<T>(i, ns) + getMDSz();
}}}
#endif

