#include "mockreaddirect.hh"
#include "readdirectwraptests.hh"
#include "wraptesttools.hh"

using namespace PIOL;
using namespace testing;

const auto readText_return = std::string{"Test ReadDirect Text"};

void test_PIOL_File_ReadDirect(
  std::shared_ptr<ExSeis*> piol, std::shared_ptr<Param*> param)
{

    auto read_direct_ptr = std::make_shared<ReadDirect*>();

    EXPECT_CALL(
      mockReadDirect(), ctor(_, GetEqDeref(piol), "Test_ReadDirect_filename"))
      .WillOnce(SaveArg<0>(read_direct_ptr));

    EXPECT_CALL(mockReadDirect(), readText(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(readText_return));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    const size_t ns = 600;
    EXPECT_CALL(mockReadDirect(), readNs(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(ns));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), readNt(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(610));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), readInc(EqDeref(read_direct_ptr)))
      .WillOnce(CheckReturn(620.0));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mockReadDirect(),
      readParam(EqDeref(read_direct_ptr), 630, 640, EqDeref(param)));

    EXPECT_CALL(
      mockReadDirect(),
      readParamNonContiguous(EqDeref(read_direct_ptr), 650, _, EqDeref(param)))
      .WillOnce(WithArg<2>(Invoke([](const size_t* trace) {
          for (size_t i = 0; i < 650; i++) {
              EXPECT_FLOAT_EQ(trace[i], 1.0 * i);
          }
      })));


    EXPECT_CALL(
      mockReadDirect(),
      readTrace(EqDeref(read_direct_ptr), 660, 670, _, EqDeref(param)))
      .WillOnce(DoAll(
        WithArg<3>(Invoke([](float* trace) {
            for (size_t i = 0; i < ns * 670; i++) {
                trace[i] = 1.0 * i;
            }
        })),
        CheckInOutParam("trace *= 2")));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());


    EXPECT_CALL(
      mockReadDirect(), readTraceNonContiguous(
                          EqDeref(read_direct_ptr), 680, _, _, EqDeref(param)))
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
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());


    EXPECT_CALL(
      mockReadDirect(), readTraceNonMonotonic(
                          EqDeref(read_direct_ptr), 690, _, _, EqDeref(param)))
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
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), dtor(EqDeref(read_direct_ptr)));
}
