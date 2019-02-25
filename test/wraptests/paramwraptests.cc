#include "mockparam.hh"
#include "paramwraptests.hh"
#include "rulewraptests.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::piol;


std::shared_ptr<Trace_metadata*> test_piol_file_trace_metadata(
    std::shared_ptr<Rule*> test_rule)
{
    auto param_ptr = std::make_shared<Trace_metadata*>();
    // Copy constructor likely called to pass Rule to Trace_metadata, and again
    // from Trace_metadata to Mock_Trace_metadata. (true, true, true)
    // constructor called on constructing Trace_metadata.
    EXPECT_CALL(mock_rule(), Rule_ctor(_, _, _));
    EXPECT_CALL(mock_rule(), Rule_ctor(_, _, _, _));
    EXPECT_CALL(mock_trace_metadata(), ctor(_, EqDerefDeref(test_rule), 300))
        .WillOnce(SaveArg<0>(param_ptr));
    EXPECT_CALL(mock_rule(), Rule_dtor(_)).Times(AnyNumber());

    auto param_ptr_2 = std::make_shared<Trace_metadata*>();
    // Constructor for Rule called in Trace_metadata constructor.
    EXPECT_CALL(mock_rule(), Rule_ctor(_, _, _, _));
    EXPECT_CALL(mock_trace_metadata(), ctor(_, 310))
        .WillOnce(SaveArg<0>(param_ptr_2));

    EXPECT_CALL(mock_trace_metadata(), size(EqDeref(param_ptr)))
        .WillOnce(CheckReturn(320));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(mock_trace_metadata(), memory_usage(EqDeref(param_ptr)))
        .WillOnce(CheckReturn(330));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mock_trace_metadata(), get_integer(EqDeref(param_ptr), 340, Meta::Copy))
        .WillOnce(CheckReturn(350));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(
        mock_trace_metadata(), get_index(EqDeref(param_ptr), 360, Meta::Copy))
        .WillOnce(CheckReturn(370));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(
        mock_trace_metadata(),
        get_floating_point(EqDeref(param_ptr), 380, Meta::Copy))
        .WillOnce(CheckReturn(390.0));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mock_trace_metadata(),
        set_integer(EqDeref(param_ptr), 400, Meta::Copy, 410));
    EXPECT_CALL(
        mock_trace_metadata(),
        set_index(EqDeref(param_ptr), 420, Meta::Copy, 430));
    EXPECT_CALL(
        mock_trace_metadata(),
        set_floating_point(
            EqDeref(param_ptr), 440, Meta::Copy, DoubleEq(450.0)));

    EXPECT_CALL(
        mock_trace_metadata(),
        copy_entries(
            EqDeref(param_ptr), 460, AddressEqDeref(param_ptr_2), 470));

    EXPECT_CALL(mock_trace_metadata(), dtor(EqDeref(param_ptr_2)));
    EXPECT_CALL(mock_rule(), Rule_dtor(_)).Times(AnyNumber());

    return param_ptr;
}
