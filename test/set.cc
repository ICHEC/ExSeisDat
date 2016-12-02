#include "settest.hh"

TEST_F(SetTest, Concat)
{
    init(1, 1, 1);

    set->sort(File::SortType::SrcRcv);

    for (size_t i = 0; i < set->file.size(); i++)
        for (size_t j = 0; j < set->file[i]->lst.size(); j++)
            std::cout << set->file[i]->lst[j] << std::endl;
}
