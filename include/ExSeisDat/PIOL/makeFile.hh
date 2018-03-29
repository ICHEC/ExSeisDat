////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Functions for creating instances of ReadInterface and WriteInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_MAKEFILE_HH
#define EXSEISDAT_PIOL_MAKEFILE_HH

#include "ExSeisDat/PIOL/ReadInterface.hh"
#include "ExSeisDat/PIOL/WriteInterface.hh"

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
      piol, name, Obj::makeDefaultObj(piol, name, Data::FileMode::Read));
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
      piol, name, Obj::makeDefaultObj(piol, name, Data::FileMode::Write));
}

}  // namespace File
}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_MAKEFILE_HH
