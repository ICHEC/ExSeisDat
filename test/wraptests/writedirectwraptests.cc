#include "writedirectwraptests.hh"
#include "mockwritedirect.hh"
#include "wraptesttools.hh"

using namespace PIOL;
using namespace File;
using namespace testing;

const auto writeText_return = std::string{"Test WriteDirect Text"};

void test_PIOL_File_WriteDirect(
    std::shared_ptr<ExSeis*> piol, std::shared_ptr<Param*> param
) {

    auto write_direct_ptr = std::make_shared<WriteDirect*>();

    EXPECT_CALL(
        mockWriteDirect(),
        ctor(_, GetEqDeref(piol), "Test_WriteDirect_filename")
    ).WillOnce(SaveArg<0>(write_direct_ptr));

    EXPECT_CALL(
        mockWriteDirect(),
        writeText(EqDeref(write_direct_ptr), StrEq("Test WriteDirect Text"))
    );

    const size_t ns = 700;
    EXPECT_CALL(mockWriteDirect(), writeNs(EqDeref(write_direct_ptr), ns));

    EXPECT_CALL(mockWriteDirect(), writeNt(EqDeref(write_direct_ptr), 710));

    EXPECT_CALL(
        mockWriteDirect(),
        writeInc(EqDeref(write_direct_ptr), DoubleEq(720.0))
    );

    EXPECT_CALL(
        mockWriteDirect(),
        writeParam(EqDeref(write_direct_ptr), 730, 740, EqDeref(param))
    );


    EXPECT_CALL(
        mockWriteDirect(),
        writeParamNonContiguous(
            EqDeref(write_direct_ptr), 750, _, EqDeref(param)
        )
    ).WillOnce(
        WithArg<2>(Invoke([](const size_t* offsets) {
            for(size_t i=0; i<750; i++) {
                EXPECT_EQ(offsets[i], i);
            }
        }))
    );


    EXPECT_CALL(
        mockWriteDirect(),
        writeTrace(
            EqDeref(write_direct_ptr), 760, 770, _, EqDeref(param)
        )
    ).WillOnce(
        WithArg<3>(Invoke([](float* trace) {
            for(size_t i=0; i<ns*770; i++) {
                EXPECT_FLOAT_EQ(trace[i], i*1.0);
            }
        }))
    );


    EXPECT_CALL(
        mockWriteDirect(),
        writeTraceNonContiguous(
            EqDeref(write_direct_ptr), 780, _, _, EqDeref(param)
        )
    ).WillOnce(
        WithArgs<2, 3>(Invoke([](const size_t* offsets, float* trace) {
            for(size_t i=0; i<780; i++) {
                EXPECT_EQ(offsets[i], i);
            }
            for(size_t i=0; i<ns*780; i++) {
                EXPECT_FLOAT_EQ(trace[i], i*1.0);
            }
        }))
    );

    EXPECT_CALL(mockWriteDirect(), dtor(EqDeref(write_direct_ptr)));
}
