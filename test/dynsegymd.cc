#include "dynsegymdtest.hh"
TEST(PrmReturn, Integer)
{
    prmRet ret;
    ret.val.i = 100;
    ASSERT_EQ(100, int(ret));
    ASSERT_EQ(100, llint(ret));
    ASSERT_EQ(100U, size_t(ret));
    ret.val.i = 1337*10000;
    ASSERT_EQ(13370000, int(ret));
    ASSERT_EQ(13370000, llint(ret));
    ASSERT_EQ(13370000U, size_t(ret));
}

TEST(PrmReturn, Short)
{
    prmRet ret;
    ret.val.i = 100;
    ASSERT_EQ(100, short(ret));
}

TEST(PrmReturn, Float)
{
    prmRet ret;
    geom_t f = 1337000.;
    ret.val.f = f;
    ASSERT_DOUBLE_EQ(f, double(ret));
    ASSERT_DOUBLE_EQ(float(f), float(ret));
}

TEST(RuleEntry, SEGYFloat)
{
    SEGYFloatRuleEntry entry(2U, Tr::xSrc, Tr::ScaleCoord);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(Tr::xSrc), entry.loc);
    ASSERT_EQ(size_t(Tr::ScaleCoord), entry.scalLoc);
}

TEST(RuleEntry, SEGYShort)
{
    SEGYShortRuleEntry entry(2U, Tr::ScaleCoord);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(Tr::ScaleCoord), entry.loc);
}

TEST(RuleEntry, SEGYLong)
{
    SEGYLongRuleEntry entry(2U, Tr::il);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(Tr::il), entry.loc);
}


TEST_F(RuleFixList, List)
{
    size_t i = 0;
    for (auto & m : rule->translate)
    {
        auto entry = dynamic_cast<SEGYFloatRuleEntry *>(m.second);

        int match = 0;
        for (auto & me : meta)
            match += (me == m.first);
        ASSERT_EQ(match, 1) << i;

        match = 0;
        for (auto loc : locs)
            match += (loc == entry->loc);
        ASSERT_EQ(match, 1) << i;
        ASSERT_EQ(size_t(Tr::ScaleCoord), entry->scalLoc) << i;
    }
    ASSERT_EQ(rule->extent(), locs[3] - size_t(Tr::ScaleCoord) + 4U);
}

TEST_F(RuleFixEmpty, AddRmLongRules)
{
    rule->addLong(Meta::xl, Tr::il);
    ASSERT_EQ(rule->getEntry(Meta::xl)->loc, size_t(Tr::il));
    ASSERT_EQ(rule->extent(), 4);
    rule->rmRule(Meta::xl);
    ASSERT_EQ(NULL, rule->getEntry(Meta::xl));
}

TEST_F(RuleFixEmpty, AddRmFloatRules)
{
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);
    ASSERT_NE(nullptr, rule->getEntry(Meta::dsdr));
    auto frule = dynamic_cast<SEGYFloatRuleEntry *>(rule->getEntry(Meta::dsdr));
    ASSERT_NE(nullptr, frule);
    ASSERT_EQ(frule->loc, size_t(Tr::SrcMeas));
    ASSERT_EQ(frule->scalLoc, size_t(Tr::SrcMeasExp));
    ASSERT_EQ(rule->extent(), 6);
    rule->rmRule(Meta::dsdr);
    ASSERT_EQ(nullptr, rule->getEntry(Meta::dsdr));
}

TEST_F(RuleFixEmpty, Extent)
{
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::ScaleCoord);
    ASSERT_EQ(rule->extent(), size_t(Tr::SrcMeas) + 4U - size_t(Tr::ScaleCoord));
}

TEST_F(RuleFixList, setPrm)
{
    rule->addLong(Meta::dsdr, Tr::SrcMeas);
    rule->addShort(Meta::il, Tr::ScaleElev);
    Param prm(rule, 100);
    for (size_t i = 0; i < 100; i++)
    {
        setPrm(i, Meta::xSrc, geom_t(i) + 1., &prm);
        setPrm(i, Meta::ySrc, geom_t(i) + 2., &prm);
        setPrm(i, Meta::xRcv, geom_t(i) + 3., &prm);
        setPrm(i, Meta::yRcv, geom_t(i) + 4., &prm);
        setPrm(i, Meta::dsdr, llint(i + 1), &prm);
        setPrm(i, Meta::il, short(i + 2), &prm);
    }
    for (size_t i = 0; i < 100; i++)
    {
        ASSERT_DOUBLE_EQ(geom_t(getPrm(i, Meta::xSrc, &prm)), geom_t(i) + 1.);
        ASSERT_DOUBLE_EQ(geom_t(getPrm(i, Meta::ySrc, &prm)), geom_t(i) + 2.);
        ASSERT_DOUBLE_EQ(geom_t(getPrm(i, Meta::xRcv, &prm)), geom_t(i) + 3.);
        ASSERT_DOUBLE_EQ(geom_t(getPrm(i, Meta::yRcv, &prm)), geom_t(i) + 4.);
        ASSERT_EQ(llint(getPrm(i, Meta::dsdr, &prm)), llint(i + 1));
        ASSERT_EQ(short(getPrm(i, Meta::il, &prm)), short(i + 2));
    }
}

TEST_F(RuleFixDefault, Constructor)
{
        ASSERT_EQ(rule->translate.size(), 9);
        ASSERT_EQ(rule->numFloat, 6);
        ASSERT_EQ(rule->numLong, 3);
        ASSERT_EQ(rule->numShort, 0);
        ASSERT_EQ(rule->extent(), 240U);
}

TEST_F(RuleFixDefault, MakeCopy)
{
    auto r = new Rule(rule->translate, false);
    ASSERT_EQ(r->translate.size(), 9);
    ASSERT_EQ(r->numFloat, 6);
    ASSERT_EQ(r->numLong, 3);
    ASSERT_EQ(r->numShort, 0);
    ASSERT_EQ(r->extent(), 192);
}

TEST_F(RuleFixDefault, MakeCopyFull)
{
    auto r = new Rule(rule->translate, true);
    ASSERT_EQ(r->translate.size(), 9);
    ASSERT_EQ(r->numFloat, 6);
    ASSERT_EQ(r->numLong, 3);
    ASSERT_EQ(r->numShort, 0);
    ASSERT_EQ(r->extent(), 240);
}
