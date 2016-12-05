#include "settest.hh"

TEST_F(SetTest, SortSrcX)
{
    init(1, 1, 1, 1, true);

    set->sort(File::SortType::SrcRcv);

    for (size_t i = 0; i < set->file.size(); i++)
        for (size_t j = 1; j < set->file[i]->lst.size(); j++)
            EXPECT_EQ(set->file[i]->lst[j] + 1, set->file[i]->lst[j-1]);
}

TEST_F(SetTest, SortRcvX)
{
    init(1, 1, 1, 1, true);

    set->sort(File::SortType::RcvOff);

    for (size_t i = 0; i < set->file.size(); i++)
        for (size_t j = 1; j < set->file[i]->lst.size(); j++)
            EXPECT_EQ(set->file[i]->lst[j] - 1, set->file[i]->lst[j-1]);
}

TEST_F(SetTest, SortSortRcv)
{
    init(1, 1, 1, 2, true);

    set->sort(File::SortType::SrcRcv);
    set->sort(File::SortType::RcvOff);

    for (size_t i = 0; i < set->file.size(); i++)
        for (size_t j = 1; j < set->file[i]->lst.size(); j++)
            EXPECT_EQ(set->file[i]->lst[j] - 1, set->file[i]->lst[j-1]);
}

