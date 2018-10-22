#include "mockreadinterface.hh"
#include "readinterfacewraptests.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::piol;

const auto read_text_return = std::string{"Test ReadInterface Text"};

void test_piol_file_read_interface(
  std::shared_ptr<ExSeis*> piol, std::shared_ptr<Trace_metadata*> param)
{

    auto read_direct_ptr = std::make_shared<ReadInterface*>();

    EXPECT_CALL(
      mock_read_interface(),
      ctor(_, GetEqDeref(piol), "Test_ReadInterface_filename"))
      .WillOnce(SaveArg<0>(read_direct_ptr));

    EXPECT_CALL(mock_read_interface(), read_text(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(read_text_return));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    const size_t ns = 600;
    EXPECT_CALL(mock_read_interface(), read_ns(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(ns));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_read_interface(), read_nt(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(610));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mock_read_interface(), read_sample_interval(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(620.0));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mock_read_interface(),
      read_param(EqDeref(read_direct_ptr), 630, 640, EqDeref(param), 0));

    EXPECT_CALL(
      mock_read_interface(),
      read_param_non_contiguous(
        EqDeref(read_direct_ptr), 650, _, EqDeref(param), 0))
      .WillOnce(WithArg<2>(Invoke([](const size_t* trace) {
          for (size_t i = 0; i < 650; i++) {
              EXPECT_FLOAT_EQ(trace[i], 1.0 * i);
          }
      })));


    EXPECT_CALL(
      mock_read_interface(),
      read_trace(EqDeref(read_direct_ptr), 660, 670, _, EqDeref(param), 0))
      .WillOnce(DoAll(
        WithArg<3>(Invoke([](float* trace) {
            for (size_t i = 0; i < ns * 670; i++) {
                trace[i] = 1.0 * i;
            }
        })),
        CheckInOutParam("trace *= 2")));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());


    EXPECT_CALL(
      mock_read_interface(),
      read_trace_non_contiguous(
        EqDeref(read_direct_ptr), 680, _, _, EqDeref(param), 0))
      .WillOnce(DoAll(
        WithArgs<2, 3>(Invoke([](const size_t* offsets, float* traces) {
            for (size_t i = 0; i < 680; i++) {
                EXPECT_EQ(offsets[i], i);
            }

            for (size_t i = 0; i < ns * 680; i++) {
                traces[i] = 1.0 * i;
            }
        })),
        CheckInOutParam("traces[i] = i")));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());


    EXPECT_CALL(
      mock_read_interface(),
      read_trace_non_monotonic(
        EqDeref(read_direct_ptr), 690, _, _, EqDeref(param), 0))
      .WillOnce(DoAll(
        WithArgs<2, 3>(Invoke([](const size_t* offsets, float* traces) {
            for (size_t i = 0; i < 690; i++) {
                EXPECT_EQ(offsets[i], i);
            }

            for (size_t i = 0; i < ns * 690; i++) {
                traces[i] = 1.0 * i;
            }
        })),
        CheckInOutParam("traces *= 2")));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_read_interface(), dtor(EqDeref(read_direct_ptr)));
}
