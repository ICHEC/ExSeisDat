#ifndef PIOLSHARESEGY_INCLUDE_GUARD
#define PIOLSHARESEGY_INCLUDE_GUARD
namespace PIOL { namespace SEGSz {
/*! \brief An enumeration containing important SEG-Y sizes
 */
enum class Size : size_t
{
    HO = 3600U,  //!< The size of the SEG-Y header (text extensions not used)
    DOMd = 240U  //!< The SEG-Y trace header size
};

/*! \brief Return the size of the Data-Object Metadata field
 */
constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::DOMd);
}

/*! \brief Return the size of the Header Object (assuming not text extensions are used)
 */
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

/*! \brief Return the size of the Data-Object.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 */
template <typename T = float>
inline size_t getDOSz(const size_t ns)
{
    return getMDSz()+ns*sizeof(T);
}

/*! \brief Return the offset location of a specific data object.
 *  \param[in] i The location of the ith data object will be returned.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 */
template <typename T = float>
inline size_t getDOLoc(const size_t i, const size_t ns)
{
    return getHOSz() + i*getDOSz<T>(ns);
}

/*! \brief Return the offset location of a specific data-field
 *  \param[in] i The location of the ith data-field will be returned.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 */
template <typename T = float>
inline size_t getDODFLoc(const size_t i, const size_t ns)
{
    return getDOLoc<T>(i, ns) + getMDSz();
}
}}
#endif

