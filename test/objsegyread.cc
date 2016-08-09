#include "objsegytest.hh"
TEST_F(ObjSpecTest, TestBypassConstructor)
{
    auto mock = std::make_shared<MockData>(piol, notFile);
    std::shared_ptr<Data::Interface> data = mock;

    Obj::SEGY segy(piol, notFile, segyOpt, data);
    EXPECT_EQ(piol, segy.piol);
    EXPECT_EQ(notFile, segy.name);
    EXPECT_EQ(data, segy.data);
}

void SEGYFileSizeTest(std::shared_ptr<ExSeisPIOL> piol, std::string name, const Obj::SEGYOpt & segyOpt, size_t sz)
{
    auto mock = std::make_shared<MockData>(piol, name);
    EXPECT_CALL(*mock, getFileSz()).Times(Exactly(1)).WillOnce(Return(sz));

    Obj::SEGY segy(piol, name, segyOpt, mock);
    piol->isErr();
    EXPECT_EQ(sz, segy.getFileSz());
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    size_t sz = 40U*prefix(2U);
    SCOPED_TRACE("SmallSEGYFileSize");
    SEGYFileSizeTest(piol, notFile, segyOpt, sz);
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    size_t sz = 8U*prefix(4U);
    SCOPED_TRACE("BigSEGYFileSize");
    SEGYFileSizeTest(piol, notFile, segyOpt, sz);
}

void ExpectHOPattern(MockData * mock, std::vector<uchar> * ho)
{
    for (size_t i = 0U; i < SEGSz::getHOSz(); i++)
        ho->at(i) = getPattern(i);
    EXPECT_CALL(*mock, read(0U, SEGSz::getHOSz(), _)).Times(Exactly(1)).WillOnce(SetArrayArgument<2>(ho->begin(), ho->end()));
}

void ExpectTrHdrPattern(size_t offset, size_t ns, MockData * mock, std::vector<uchar> * tr)
{
    size_t foff = SEGSz::getDOLoc(offset, ns);
    for (size_t i = 0U; i < SEGSz::getMDSz(); i++)
        tr->at(i) = getPattern(foff+i);
    EXPECT_CALL(*mock, read(foff, SEGSz::getMDSz(), SEGSz::getDOSz(ns), 1U, _)).Times(Exactly(1)).WillOnce(SetArrayArgument<4>(tr->begin(), tr->end()));
}

TEST_F(ObjSpecTest, SEGYHORead)
{
    SCOPED_TRACE("SEGYRead");
    const size_t extra = 1111U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cHo(SEGSz::getHOSz());

    ExpectHOPattern(mock.get(), &cHo);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);
    piol->isErr();

    std::vector<uchar> ho(SEGSz::getHOSz() + extra);
    for (auto i = 0U; i < extra; i++)
        ho[ho.size()-extra+i] = magicNum1;

    segy.readHO(ho.data());
    piol->isErr();

    for (auto i = 0U; i < SEGSz::getHOSz(); i++)
        ASSERT_EQ(getPattern(i), ho[i]) << "Pattern " << i;
    for (auto i = 0U; i < extra; i++)
        ASSERT_EQ(magicNum1, ho[ho.size()-extra+i]) << "Pattern Extra " << i;
}

TEST_F(ObjSpecTest, SEGYTrRead)
{
    const size_t ns = 200U;
    const size_t traceNum = 10U;
    const size_t extra = 1111U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cTrHdr(SEGSz::getMDSz());
    ExpectTrHdrPattern(traceNum, ns, mock.get(), &cTrHdr);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);

    piol->isErr();

    std::vector<uchar> trHdr(SEGSz::getMDSz() + extra);
    for (auto i = 0U; i < extra; i++)
        trHdr[trHdr.size()-extra+i] = magicNum1;

    segy.readDOMD(traceNum, ns, 1U, trHdr.data());

    for (auto i = 0U; i < SEGSz::getMDSz(); i++)
    {
        auto pat = getPattern(SEGSz::getDOLoc(traceNum, ns) + i);
        ASSERT_EQ(pat, trHdr[i]) << "tr Pattern " << i;
    }
    for (auto i = 0U; i < extra; i++)
        ASSERT_EQ(magicNum1, trHdr[trHdr.size()-extra+i]) << "tr Pattern Extra " << i;
}

