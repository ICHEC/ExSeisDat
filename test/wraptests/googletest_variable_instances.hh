////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Declare extern templates for weak symbols defined from using the
///        googletest library.
/// @detail The googletest library uses static variables in static classes
///         (effectively as inline variables) to build certain return values,
///         whose addresses are accessed. This can cause some issues with global
///         weak symbols while using the AddressSanitizer. We define strong
///         symbols for these variables here to avoid these issues.
///
///         On OSX, the issue was the wrap tests hanging on initialization
///         when AddressSanitizer was active. This is likely due to the
///         AddressSanitizer adding instrumentation to the variables, but
///         without a strong reference, needing to add it in every translation
///         unit it was visible. The old fix was to redefine the return type of
///         some functions to a const return type, making them different types,
///         likely breaking some circular dependency in the initialization
///         order.
///
///         The TypeIdHelper<Test>::dummy_ variable still suffers from global
///         weak definition, but since it's instantiated and accessed
///         implicitly in the compiled source of the googletest library,
///         we can't intercept it nicely. However, it's only accessed directly
///         from that source file, so the dependencies shouldn't be an issue.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_TEST_WRAPTESTS_GOOGLETEST_VARIABLE_INSTANCES_HH
#define EXSEISDAT_TEST_WRAPTESTS_GOOGLETEST_VARIABLE_INSTANCES_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "exseisdat/utils/typedefs.hh"

namespace exseis {
namespace piol {
class RuleEntry;
}  // namespace piol
}  // namespace exseis

extern template testing::DefaultValue<exseis::piol::RuleEntry*>::ValueProducer*
    testing::DefaultValue<exseis::piol::RuleEntry*>::producer_;

extern template testing::DefaultValue<size_t>::ValueProducer*
    testing::DefaultValue<size_t>::producer_;

extern template testing::DefaultValue<bool>::ValueProducer*
    testing::DefaultValue<bool>::producer_;

extern template testing::DefaultValue<double>::ValueProducer*
    testing::DefaultValue<double>::producer_;

extern template testing::DefaultValue<exseis::utils::Integer>::ValueProducer*
    testing::DefaultValue<exseis::utils::Integer>::producer_;

extern template testing::DefaultValue<int16_t>::ValueProducer*
    testing::DefaultValue<int16_t>::producer_;

extern template const std::string*
    testing::DefaultValue<const std::string&>::address_;

extern template testing::DefaultValue<std::string>::ValueProducer*
    testing::DefaultValue<std::string>::producer_;

extern template testing::DefaultValue<std::vector<std::string>>::ValueProducer*
    testing::DefaultValue<std::vector<std::string>>::producer_;

#endif  // EXSEISDAT_TEST_WRAPTESTS_GOOGLETEST_VARIABLE_INSTANCES_HH
