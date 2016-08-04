/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details The intention is to make this obselete very soon
 *//*******************************************************************************************/
#ifndef PIOLSHARESEGY_INCLUDE_GUARD
#define PIOLSHARESEGY_INCLUDE_GUARD
namespace PIOL { namespace SEGSz {
/*! \brief An enumeration containing important SEG-Y sizes
 */
enum class Size : size_t
{
    Text = 3200U,
    HO = 3600U,  //!< The size of the SEG-Y header (text extensions not used)
    DOMd = 240U  //!< The SEG-Y trace header size
};

/*! \brief Return the size of the text field
 *  \return  Returns the size of the text field in bytes
 */
constexpr size_t getTextSz()
{
    return static_cast<size_t>(Size::Text);
}

/*! \brief Return the size of the Data-Object Metadata object
 *  \return Returns the size of the DOMD in bytes
 */
constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::DOMd);
}

/*! \brief Return the size of the Data-Object Metadata object
 *  \return Returns the size of the DOMD in bytes
 */
template <typename T = float>
constexpr size_t getDFSz(size_t ns)
{
    return ns * sizeof(T);
}

/*! \brief Return the size of the Header Object (assuming text extensions are not used)
 *  \return Returns the size of the HO in bytes.
 */
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
}

/*! \brief Return the size of the Data-Object.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the DO size.
 */
template <typename T = float>
inline size_t getDOSz(const size_t ns)
{
    return getMDSz() + ns*sizeof(T);
}

/*! \brief Return the expected size of the file if there are nt data-objects and
 *         ns elements in a data-field.
 *  \param[in] nt The number of data objects.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the expected file size.
 */
template <typename T = float>
inline size_t getFileSz(const size_t nt, const size_t ns)
{
    return getHOSz() + nt*getDOSz(ns);
}

/*! \brief Return the offset location of a specific data object.
 *  \param[in] i The location of the ith data object will be returned.
 *  \param[in] ns The number of elements in the data-field.
 *  \tparam T The datatype of the data-field. The default value is float.
 *  \return Returns the location.
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
 *  \return Returns the location.
 */
template <typename T = float>
inline size_t getDODFLoc(const size_t i, const size_t ns)
{
    return getDOLoc<T>(i, ns) + getMDSz();
}
}}
#endif

