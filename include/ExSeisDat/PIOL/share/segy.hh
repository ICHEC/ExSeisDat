////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHARESEGY_INCLUDE_GUARD
#define PIOLSHARESEGY_INCLUDE_GUARD

#include "ExSeisDat/PIOL/anc/global.hh"

namespace PIOL {
namespace SEGSz {

/*! @brief An enumeration containing important SEG-Y sizes
 */
enum class Size : size_t {
    Text = 3200U,
    /// The size of the SEG-Y header (text extensions not used)
    HO = 3600U,
    /// The SEG-Y trace header size
    DOMd = 240U
};

/*! @brief Return the size of the text field
 *  @return  Returns the size of the text field in bytes
 */
constexpr size_t getTextSz()
{
    return static_cast<size_t>(Size::Text);
}

/*! @brief Return the size of the Header Object (assuming text extensions are
 *         not used)
 *  @return Returns the size of the HO in bytes.
 */
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

/*! @brief Return the size of the Data-Object Metadata object
 *  @return Returns the size of the DOMD in bytes
 */
constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::DOMd);
}

/*! @brief Return the size of the Data-Object Field object
 *  @param[in] ns The number of elements in the data-field.
 *  @return Returns the size of the data-field in bytes
 */
template<typename T = float>
constexpr size_t getDFSz(const size_t ns)
{
    return ns * sizeof(T);
}

/*! @brief Return the size of the Data-Object.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the DO size.
 */
template<typename T = float>
size_t getDOSz(const size_t ns)
{
    return getMDSz() + getDFSz<T>(ns);
}

/*! @brief Return the expected size of the file if there are nt data-objects and
 *         ns elements in a data-field.
 *  @param[in] nt The number of data objects.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the expected file size.
 */
template<typename T = float>
size_t getFileSz(const size_t nt, const size_t ns)
{
    return getHOSz() + nt * getDOSz<T>(ns);
}

/*! @brief Return the offset location of a specific data object.
 *  @param[in] i The location of the ith data object will be returned.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the location.
 */
template<typename T = float>
size_t getDOLoc(const size_t i, const size_t ns)
{
    return getFileSz<T>(i, ns);
}

/*! @brief Return the offset location of a specific data-field
 *  @param[in] i The location of the ith data-field will be returned.
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the location.
 */
template<typename T = float>
size_t getDODFLoc(const size_t i, const size_t ns)
{
    return getFileSz<T>(i, ns) + getMDSz();
}

/*! @brief Return the number of traces in a file given a file size
 *  @param[in] fsz the size of a file or expected size in bytes
 *  @param[in] ns The number of elements in the data-field.
 *  @tparam T The datatype of the data-field. The default value is float.
 *  @return Returns the number of traces.
 */
template<typename T = float>
size_t getNt(const size_t fsz, const size_t ns)
{
    return (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz<T>(ns);
}

}  // namespace SEGSz
}  // namespace PIOL

#endif
