#include "object/object.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
class MockObject : public PIOL::Obj::Interface
{
    public :
    MOCK_METHOD2(getSize, size_t(size_t, size_t));
    MOCK_METHOD1(readHO, void(unsigned char *));
    MOCK_METHOD4(readDO, void(size_t, size_t, unsigned char *, size_t));
    MOCK_METHOD4(readDODF, void(size_t, size_t, float *, size_t));
    MOCK_METHOD4(readDOMD, void(size_t, size_t, unsigned char *, size_t));

    MOCK_METHOD1(writeHO, void(unsigned char *));
    MOCK_METHOD4(writeDO, void(size_t, size_t, unsigned char *, size_t));
    MOCK_METHOD4(writeDODF, void(size_t, size_t, float *, size_t));
    MOCK_METHOD4(writeDOMD, void(size_t, size_t, unsigned char *, size_t));
};
int main(int argc, char ** argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    NiceMock<MockObject> MockObj;

    EXPECT_CALL(MockObj, getSize(Gt(0), Gt(0))).Times(1).WillRepeatedly(Return(10));
    EXPECT_CALL(MockObj, readHO(NotNull()));

//    std::cout << "Mock " << MockObj.getSize(1, 1) << std::endl;
}

