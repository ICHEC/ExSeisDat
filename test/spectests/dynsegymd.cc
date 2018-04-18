#include "dynsegymdtest.hh"

#include "ExSeisDat/PIOL/param_utils.hh"

// TODO: Add test for param_utils::cpyPrm called with different sets of rules,
// i.e dst and
//       src disagree on rules

TEST(RuleEntry, SEGYFloat)
{
    SEGYFloatRuleEntry entry(2U, PIOL_TR_xSrc, PIOL_TR_ScaleCoord);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(PIOL_TR_xSrc), entry.loc);
    ASSERT_EQ(size_t(PIOL_TR_ScaleCoord), entry.scalLoc);
}

TEST(RuleEntry, SEGYShort)
{
    SEGYShortRuleEntry entry(2U, PIOL_TR_ScaleCoord);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(PIOL_TR_ScaleCoord), entry.loc);
}

TEST(RuleEntry, SEGYLong)
{
    SEGYLongRuleEntry entry(2U, PIOL_TR_il);
    ASSERT_EQ(2U, entry.num);
    ASSERT_EQ(size_t(PIOL_TR_il), entry.loc);
}


TEST_F(RuleFixList, List)
{
    size_t i = 0;
    for (auto& m : rule->translate) {
        auto entry = dynamic_cast<SEGYFloatRuleEntry*>(m.second);

        int match = 0;
        for (auto& me : meta)
            match += (me == m.first);
        EXPECT_EQ(match, 1) << i;

        match = 0;
        for (auto loc : locs)
            match += (loc == entry->loc);
        ASSERT_EQ(match, 1) << i;
        ASSERT_EQ(size_t(PIOL_TR_ScaleCoord), entry->scalLoc) << i;
    }
    ASSERT_EQ(rule->extent(), locs[3] - size_t(PIOL_TR_ScaleCoord) + 4U);
}

TEST_F(RuleFixEmpty, AddRmLongRules)
{
    rule->addLong(PIOL_META_xl, PIOL_TR_il);
    ASSERT_EQ(rule->getEntry(PIOL_META_xl)->loc, size_t(PIOL_TR_il));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(4));
    rule->rmRule(PIOL_META_xl);
    ASSERT_EQ(NULL, rule->getEntry(PIOL_META_xl));
}

TEST_F(RuleFixEmpty, AddRmFloatRules)
{
    rule->addSEGYFloat(PIOL_META_dsdr, PIOL_TR_SrcMeas, PIOL_TR_SrcMeasExp);
    ASSERT_NE(nullptr, rule->getEntry(PIOL_META_dsdr));
    auto frule =
      dynamic_cast<SEGYFloatRuleEntry*>(rule->getEntry(PIOL_META_dsdr));
    ASSERT_NE(nullptr, frule);
    ASSERT_EQ(frule->loc, size_t(PIOL_TR_SrcMeas));
    ASSERT_EQ(frule->scalLoc, size_t(PIOL_TR_SrcMeasExp));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(6));
    rule->rmRule(PIOL_META_dsdr);
    ASSERT_EQ(nullptr, rule->getEntry(PIOL_META_dsdr));
}

TEST_F(RuleFixEmpty, Extent)
{
    rule->addSEGYFloat(PIOL_META_dsdr, PIOL_TR_SrcMeas, PIOL_TR_ScaleCoord);
    ASSERT_EQ(
      rule->extent(),
      size_t(PIOL_TR_SrcMeas) + 4U - size_t(PIOL_TR_ScaleCoord));
}

TEST_F(RuleFixList, setPrm)
{
    rule->addLong(PIOL_META_dsdr, PIOL_TR_SrcMeas);
    rule->addShort(PIOL_META_il, PIOL_TR_ScaleElev);
    Param prm(rule, 100);
    for (size_t i = 0; i < 100; i++) {
        param_utils::setPrm(
          i, PIOL_META_xSrc, exseis::utils::Floating_point(i) + 1., &prm);
        param_utils::setPrm(
          i, PIOL_META_ySrc, exseis::utils::Floating_point(i) + 2., &prm);
        param_utils::setPrm(
          i, PIOL_META_xRcv, exseis::utils::Floating_point(i) + 3., &prm);
        param_utils::setPrm(
          i, PIOL_META_yRcv, exseis::utils::Floating_point(i) + 4., &prm);
        param_utils::setPrm(
          i, PIOL_META_dsdr, exseis::utils::Integer(i + 1), &prm);
        param_utils::setPrm(i, PIOL_META_il, short(i + 2), &prm);
    }
    for (size_t i = 0; i < 100; i++) {
        if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
            ASSERT_DOUBLE_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_xSrc, &prm),
              exseis::utils::Floating_point(i + 1));
            ASSERT_DOUBLE_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_ySrc, &prm),
              exseis::utils::Floating_point(i + 2));
            ASSERT_DOUBLE_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_xRcv, &prm),
              exseis::utils::Floating_point(i + 3));
            ASSERT_DOUBLE_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_yRcv, &prm),
              exseis::utils::Floating_point(i + 4));
        }
        else {
            ASSERT_FLOAT_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_xSrc, &prm),
              exseis::utils::Floating_point(i + 1));
            ASSERT_FLOAT_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_ySrc, &prm),
              exseis::utils::Floating_point(i + 2));
            ASSERT_FLOAT_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_xRcv, &prm),
              exseis::utils::Floating_point(i + 3));
            ASSERT_FLOAT_EQ(
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_yRcv, &prm),
              exseis::utils::Floating_point(i + 4));
        }
        ASSERT_EQ(
          param_utils::getPrm<exseis::utils::Integer>(i, PIOL_META_dsdr, &prm),
          exseis::utils::Integer(i + 1));
        ASSERT_EQ(
          param_utils::getPrm<short>(i, PIOL_META_il, &prm), short(i + 2));
    }
}

TEST_F(RuleFixDefault, Constructor)
{
    ASSERT_EQ(rule->translate.size(), static_cast<size_t>(12));
    ASSERT_EQ(rule->numFloat, static_cast<size_t>(6));
    ASSERT_EQ(rule->numLong, static_cast<size_t>(4));
    ASSERT_EQ(rule->numShort, static_cast<size_t>(0));
    ASSERT_EQ(rule->numIndex, static_cast<size_t>(2));
    ASSERT_EQ(rule->extent(), static_cast<size_t>(240));
}

TEST_F(RuleFixDefault, MakeCopy)
{
    auto r = new Rule(rule->translate, false);
    ASSERT_EQ(r->translate.size(), static_cast<size_t>(12));
    ASSERT_EQ(r->numFloat, static_cast<size_t>(6));
    ASSERT_EQ(r->numLong, static_cast<size_t>(4));
    ASSERT_EQ(r->numShort, static_cast<size_t>(0));
    ASSERT_EQ(r->numIndex, static_cast<size_t>(2));
    ASSERT_EQ(r->extent(), static_cast<size_t>(192));
}

TEST_F(RuleFixDefault, MakeCopyFull)
{
    auto r = new Rule(rule->translate, true);
    ASSERT_EQ(r->translate.size(), static_cast<size_t>(12));
    ASSERT_EQ(r->numFloat, static_cast<size_t>(6));
    ASSERT_EQ(r->numLong, static_cast<size_t>(4));
    ASSERT_EQ(r->numShort, static_cast<size_t>(0));
    ASSERT_EQ(r->numIndex, static_cast<size_t>(2));
    ASSERT_EQ(r->extent(), static_cast<size_t>(240));
}
