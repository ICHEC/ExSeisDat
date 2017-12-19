#include "readdirectwraptests.hh"
#include "mockreaddirect.hh"
#include "wraptesttools.hh"

using namespace PIOL;
using namespace File;
using namespace testing;

const auto readText_return = std::string{"Test ReadDirect Text"};

void test_PIOL_File_ReadDirect(
    std::shared_ptr<ExSeis*> piol, std::shared_ptr<Param*> param
) {

    auto read_direct_ptr = std::make_shared<ReadDirect*>();

    EXPECT_CALL(
        mockReadDirect(),
        ctor(_, GetEqDeref(piol), "Test_ReadDirect_filename")
    ).WillOnce(SaveArg<0>(read_direct_ptr));

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
        readParam(EqDeref(read_direct_ptr), 630, 640, EqDeref(param))
    );

    EXPECT_CALL(
        mockReadDirect(),
        readTrace(
            EqDeref(read_direct_ptr), 650, 660, _, EqDeref(param)
        )
    ).WillOnce(DoAll(
        WithArg<3>(Invoke([](float* trace) {
            for(size_t i=0; i<ns*660; i++) {
                EXPECT_FLOAT_EQ(trace[i], 1.0*i);
            }

            for(size_t i=0; i<ns*660; i++) {
                trace[i] *= 2;
            }
        })),
        CheckInOutParam("trace *= 2")
    ));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), dtor(EqDeref(read_direct_ptr)));
}
