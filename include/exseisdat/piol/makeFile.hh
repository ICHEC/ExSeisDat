////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Functions for creating instances of ReadInterface and WriteInterface
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_MAKEFILE_HH
#define EXSEISDAT_PIOL_MAKEFILE_HH

#include "exseisdat/piol/file/Input_file.hh"
#include "exseisdat/piol/file/Output_file.hh"

#include "exseisdat/piol/file/detail/ObjectInterface.hh"

#include <memory>
#include <string>
#include <type_traits>


namespace exseis {
namespace piol {

///*! Construct Input_file objects with default object and MPI-IO layers.
// * @tparam T The type of the file layer.
// * @param[in] piol The piol shared object.
// * @param[in] name The name of the file.
// * @return Return a pointer of the respective file type.
// */
// template<class T>
// std::unique_ptr<
//  typename std::enable_if<std::is_base_of<Input_file, T>::value, T>::type>
// makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
//{
//    return std::make_unique<T>(
//      piol, name, makeDefaultObj(piol, name, FileMode::Read));
//}
//
///*! Construct Output_file objects with default object and MPI-IO layers
// * @tparam T The type of the file layer
// * @param[in] piol The piol shared object
// * @param[in] name The name of the file
// * @return Return a pointer of the respective file type.
// */
// template<class T>
// std::unique_ptr<
//  typename std::enable_if<std::is_base_of<Output_file, T>::value, T>::type>
// makeFile(std::shared_ptr<ExSeisPIOL> piol, const std::string& name)
//{
//    return std::make_unique<T>(
//      piol, name, makeDefaultObj(piol, name, FileMode::Write));
//}

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_MAKEFILE_HH
