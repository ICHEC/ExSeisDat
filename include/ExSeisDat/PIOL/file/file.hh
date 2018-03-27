////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date January 2017
/// @brief The File layer interface
/// @details The File layer interface is a base class which specific File
///          implementations work off
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD

#include "ExSeisDat/PIOL/file/Model3dInterface.hh"
#include "ExSeisDat/PIOL/file/ReadInterface.hh"
#include "ExSeisDat/PIOL/file/WriteInterface.hh"

#include "ExSeisDat/PIOL/object/object.hh"

#include <memory>
#include <string>
#include <type_traits>


namespace PIOL {
namespace File {

/*! Construct ReadSEGY objects with default object and MPI-IO layers.
 * @tparam T The type of the file layer.
 * @param[in] piol The piol shared object.
 * @param[in] name The name of the file.
 * @return Return a pointer of the respective file type.
 */
template<class T>
std::unique_ptr<typename std::enable_if<
  std::is_base_of<File::ReadInterface, T>::value,
  T>::type>
makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
{
    return std::make_unique<T>(
      piol, name, Obj::makeDefaultObj(piol, name, FileMode::Read));
}

/*! Construct WriteSEGY objects with default object and MPI-IO layers
 * @tparam T The type of the file layer
 * @param[in] piol The piol shared object
 * @param[in] name The name of the file
 * @return Return a pointer of the respective file type.
 */
template<class T>
std::unique_ptr<typename std::enable_if<
  std::is_base_of<File::WriteInterface, T>::value,
  T>::type>
makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
{
    return std::make_unique<T>(
      piol, name, Obj::makeDefaultObj(piol, name, FileMode::Write));
}

}  // namespace File
}  // namespace PIOL

#endif
