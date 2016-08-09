
#include "objsegytest.hh"
ACTION_P(extraTrCheck, ho)  //Use this when writing
{
    for (size_t i = 0; i < SEGSz::getMDSz(); i++)
        ASSERT_EQ(ho[i], arg4[i]) << "Error with header byte: " << i << " |\n";
}

void ExpectWriteTrHdrPattern(size_t offset, size_t ns, MockData * mock, std::vector<uchar> * tr)
{
    size_t foff = SEGSz::getDOLoc(offset, ns);
    EXPECT_CALL(*mock, write(foff, SEGSz::getMDSz(), SEGSz::getDOSz(ns), 1U, _)).Times(Exactly(1)).WillOnce(extraTrCheck(tr->data()));
}

TEST_F(ObjSpecTest, SEGYTrWrite)
{
    SCOPED_TRACE("SEGYRead");
    const size_t ns = 200U;
    const size_t traceNum = 10U;

    auto mock = std::make_shared<MockData>(piol, notFile);
    std::vector<uchar> cTrHdr(SEGSz::getMDSz());
    for (auto i = 0U; i < cTrHdr.size(); i++)
        cTrHdr[i] = getPattern(i);

    Obj::SEGY segy(piol, notFile, segyOpt, mock);
    piol->isErr();

    std::vector<uchar> trHdr(SEGSz::getMDSz());
    for (auto i = 0U; i < trHdr.size(); i++)
        trHdr[i] = getPattern(i);

    ExpectWriteTrHdrPattern(traceNum, ns, mock.get(), &cTrHdr);
    segy.writeDOMD(traceNum, ns, 1U, trHdr.data());
}
