#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKPARAM_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKPARAM_HH

#include "exseisdat/piol/Trace_metadata.hh"

#include "printers.hh"

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

namespace exseis {
namespace piol {

using namespace exseis::utils::typedefs;


class Mock_Trace_metadata;
::testing::StrictMock<Mock_Trace_metadata>& mock_trace_metadata();

class Mock_Trace_metadata {
  public:
    MOCK_METHOD3(ctor, void(Trace_metadata*, Rule& rules_, size_t num_traces));
    MOCK_METHOD2(ctor, void(Trace_metadata*, size_t num_traces));
    MOCK_METHOD1(dtor, void(Trace_metadata*));

    MOCK_CONST_METHOD3(
        get_floating_point,
        Floating_point(const Trace_metadata*, size_t trace_index, Meta entry));
    MOCK_METHOD4(
        set_floating_point,
        void(
            Trace_metadata*,
            size_t trace_index,
            Meta entry,
            Floating_point value));

    MOCK_CONST_METHOD3(
        get_integer,
        Integer(const Trace_metadata*, size_t trace_index, Meta entry));
    MOCK_METHOD4(
        set_integer,
        void(Trace_metadata*, size_t trace_index, Meta entry, Integer value));

    MOCK_CONST_METHOD3(
        get_index,
        size_t(const Trace_metadata*, size_t trace_index, Meta entry));
    MOCK_METHOD4(
        set_index,
        void(Trace_metadata*, size_t trace_index, Meta entry, size_t value));

    MOCK_METHOD4(
        copy_entries,
        void(
            Trace_metadata*,
            size_t trace_index,
            const Trace_metadata& source_trace_metadata,
            size_t source_trace_index));

    MOCK_CONST_METHOD1(size, size_t(const Trace_metadata*));

    MOCK_CONST_METHOD1(memory_usage, size_t(const Trace_metadata*));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKPARAM_HH
