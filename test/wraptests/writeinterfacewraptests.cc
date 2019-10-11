#include "mockwriteinterface.hh"
#include "wraptesttools.hh"
#include "writeinterfacewraptests.hh"

using namespace testing;
using namespace exseis::piol;

const auto write_text_return = std::string{"Test WriteInterface Text"};

void test_piol_file_write_interface(
    std::shared_ptr<ExSeis*> piol, std::shared_ptr<Trace_metadata*> param)
{

    auto write_direct_ptr = std::make_shared<Output_file*>();

    EXPECT_CALL(
        mock_write_interface(),
        ctor(_, GetEqDeref(piol), "Test_WriteInterface_filename"))
        .WillOnce(SaveArg<0>(write_direct_ptr));

    EXPECT_CALL(
        mock_write_interface(),
        write_text(
            EqDeref(write_direct_ptr), StrEq("Test WriteInterface Text")));

    const size_t ns = 700;
    EXPECT_CALL(
        mock_write_interface(), write_ns(EqDeref(write_direct_ptr), ns));

    EXPECT_CALL(
        mock_write_interface(), write_nt(EqDeref(write_direct_ptr), 710));

    EXPECT_CALL(
        mock_write_interface(),
        write_sample_interval(EqDeref(write_direct_ptr), DoubleEq(720.0)));

    EXPECT_CALL(
        mock_write_interface(),
        write_param(EqDeref(write_direct_ptr), 730, 740, EqDeref(param), 0));


    EXPECT_CALL(
        mock_write_interface(),
        write_param_non_contiguous(
            EqDeref(write_direct_ptr), 750, _, EqDeref(param), 0))
        .WillOnce(WithArg<2>(Invoke([](const size_t* offsets) {
            for (size_t i = 0; i < 750; i++) {
                EXPECT_EQ(offsets[i], i);
            }
        })));


    EXPECT_CALL(
        mock_write_interface(),
        write_trace(EqDeref(write_direct_ptr), 760, 770, _, EqDeref(param), 0))
        .WillOnce(WithArg<3>(Invoke([](float* trace) {
            for (size_t i = 0; i < ns * 770; i++) {
                EXPECT_FLOAT_EQ(trace[i], i * 1.0);
            }
        })));


    EXPECT_CALL(
        mock_write_interface(),
        write_trace_non_contiguous(
            EqDeref(write_direct_ptr), 780, _, _, EqDeref(param), 0))
        .WillOnce(
            WithArgs<2, 3>(Invoke([](const size_t* offsets, float* trace) {
                for (size_t i = 0; i < 780; i++) {
                    EXPECT_EQ(offsets[i], i);
                }
                for (size_t i = 0; i < ns * 780; i++) {
                    EXPECT_FLOAT_EQ(trace[i], i * 1.0);
                }
            })));

    EXPECT_CALL(mock_write_interface(), dtor(EqDeref(write_direct_ptr)));
}
