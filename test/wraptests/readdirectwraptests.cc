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

    EXPECT_CALL(mockReadDirect(), readNs(EqDeref(read_direct_ptr)))
        .WillOnce(CheckReturn(600));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), readNt(EqDeref(read_direct_ptr)))
        .WillOnce(CheckReturn(610));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockReadDirect(), readInc(EqDeref(read_direct_ptr)))
        .WillOnce(CheckReturn(620.0));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    //EXPECT_CALL(
    //    mockReadDirect(),
    //    readParam(EqDeref(read_direct_ptr), 630, 640, EqDeref(param))
    //);
//void ReadDirect::readParam(csize_t offset, csize_t sz, Param * prm) const
//{
//    mockReadDirect().readParam(this, offset, sz, prm);
//}

//void ReadDirect::readTrace(
//    csize_t offset, csize_t sz, trace_t * trace, Param * prm) const
//{
//    mockReadDirect().readTrace(this, offset, sz, trace, prm);
//}
//
//
//void ReadDirect::readTrace(
//    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
//{
//    mockReadDirect().readTrace(this, sz, offset, trace, prm);
//}
//
//void ReadDirect::readTraceNonMono(
//    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
//{
//    mockReadDirect().readTraceNonMono(this, sz, offset, trace, prm);
//}
//
//void ReadDirect::readParam(csize_t sz, csize_t * offset, Param * prm) const
//{
//    mockReadDirect().readParam(this, sz, offset, prm);
//}
}
