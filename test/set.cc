/* These unit tests might be fairly redundant since they are basically the same as the system tests.
 */
#include "settest.hh"
#include <iostream>
#include <string>
#include <cmath>
const std::vector<size_t> sortOffLine = {
0, 2, 5, 10, 16, 25, 34, 44, 57, 72, 88, 99, 109, 122, 133, 144, 154, 165, 175,
186, 196, 206, 217, 227, 237, 248, 258, 268, 278, 288, 298, 308, 318, 329, 339,
349, 359, 369, 379, 389, 399, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500,
510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660,
670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800, 810, 820,
830, 840, 850, 860, 870, 880, 890, 900, 910, 920, 930, 940, 950, 960, 970, 980,
990, 1, 3, 7, 12, 18, 27, 36, 47, 61, 74, 89, 100, 112, 124, 134, 145, 155, 166,
177, 187, 197, 208, 218, 228, 238, 249, 259, 269, 279, 289, 299, 309, 320, 330,
340, 350, 360, 370, 380, 390, 400, 411, 421, 431, 441, 451, 461, 471, 481, 491,
501, 511, 521, 531, 541, 551, 561, 571, 581, 591, 601, 611, 621, 631, 641, 651,
661, 671, 681, 691, 701, 711, 721, 731, 741, 751, 761, 771, 781, 791, 801, 811,
821, 831, 841, 851, 861, 871, 881, 891, 901, 911, 921, 931, 941, 951, 961, 971,
981, 991, 4, 6, 8, 14, 21, 29, 38, 51, 63, 78, 90, 102, 114, 125, 135, 146, 157,
168, 178, 188, 198, 209, 219, 230, 240, 250, 260, 270, 280, 290, 300, 311, 321,
331, 341, 351, 361, 371, 381, 392, 402, 412, 422, 432, 442, 452, 462, 472, 482,
492, 502, 512, 522, 532, 542, 552, 562, 572, 582, 592, 602, 612, 622, 632, 642,
652, 662, 672, 682, 692, 702, 712, 722, 732, 742, 752, 762, 772, 782, 792, 802,
812, 822, 832, 842, 852, 862, 872, 882, 892, 902, 912, 922, 932, 942, 952, 962,
972, 982, 992, 9, 11, 13, 19, 24, 32, 42, 53, 66, 82, 93, 106, 116, 126, 138,
149, 159, 169, 179, 190, 200, 211, 221, 231, 241, 251, 261, 271, 282, 292, 302,
312, 322, 332, 342, 352, 362, 373, 383, 393, 403, 413, 423, 433, 443, 453, 463,
473, 483, 493, 503, 513, 523, 533, 543, 553, 563, 573, 583, 593, 603, 613, 623,
633, 643, 653, 663, 673, 683, 693, 703, 713, 723, 733, 743, 753, 763, 773, 783,
793, 803, 813, 823, 833, 843, 853, 863, 873, 883, 893, 903, 913, 923, 933, 943,
953, 963, 973, 983, 993, 15, 17, 20, 23, 30, 40, 49, 60, 70, 84, 96, 108, 118,
130, 140, 150, 160, 171, 182, 192, 202, 212, 222, 232, 242, 253, 263, 273, 283,
293, 303, 313, 323, 334, 344, 354, 364, 374, 384, 394, 404, 414, 424, 434, 444,
454, 464, 474, 484, 494, 504, 514, 524, 534, 544, 554, 564, 574, 584, 594, 604,
614, 624, 634, 644, 654, 664, 674, 684, 694, 704, 714, 724, 734, 744, 754, 764,
774, 784, 794, 804, 814, 824, 834, 844, 854, 864, 874, 884, 894, 904, 914, 924,
934, 944, 954, 964, 974, 984, 994, 22, 26, 28, 31, 39, 46, 55, 68, 80, 92, 101,
113, 121, 132, 142, 153, 163, 173, 183, 193, 204, 214, 224, 234, 244, 254, 264,
274, 284, 295, 305, 315, 325, 335, 345, 355, 365, 375, 385, 395, 405, 415, 425,
435, 445, 455, 465, 475, 485, 495, 505, 515, 525, 535, 545, 555, 565, 575, 585,
595, 605, 615, 625, 635, 645, 655, 665, 675, 685, 695, 705, 715, 725, 735, 745,
755, 765, 775, 785, 795, 805, 815, 825, 835, 845, 855, 865, 875, 885, 895, 905,
915, 925, 935, 945, 955, 965, 975, 985, 995, 33, 35, 37, 41, 48, 54, 64, 77, 87,
98, 107, 117, 127, 137, 147, 158, 167, 176, 185, 195, 205, 215, 225, 236, 246,
256, 266, 276, 286, 296, 306, 316, 326, 336, 346, 356, 366, 376, 386, 396, 406,
416, 426, 436, 446, 456, 466, 476, 486, 496, 506, 516, 526, 536, 546, 556, 566,
576, 586, 596, 606, 616, 626, 636, 646, 656, 666, 676, 686, 696, 706, 716, 726,
736, 746, 756, 766, 776, 786, 796, 806, 816, 826, 836, 846, 856, 866, 876, 886,
896, 906, 916, 926, 936, 946, 956, 966, 976, 986, 996, 43, 45, 50, 52, 59, 67,
76, 85, 95, 105, 115, 123, 131, 141, 151, 161, 170, 181, 191, 201, 210, 220,
229, 239, 247, 257, 267, 277, 287, 297, 307, 317, 327, 337, 347, 357, 367, 377,
387, 397, 407, 417, 427, 437, 447, 457, 467, 477, 487, 497, 507, 517, 527, 537,
547, 557, 567, 577, 587, 597, 607, 617, 627, 637, 647, 657, 667, 677, 687, 697,
707, 717, 727, 737, 747, 757, 767, 777, 787, 797, 807, 817, 827, 837, 847, 857,
867, 877, 887, 897, 907, 917, 927, 937, 947, 957, 967, 977, 987, 997, 56, 58,
62, 65, 69, 79, 86, 94, 103, 111, 120, 129, 139, 148, 156, 164, 174, 184, 194,
203, 213, 223, 233, 243, 252, 262, 272, 281, 291, 301, 310, 319, 328, 338, 348,
358, 368, 378, 388, 398, 408, 418, 428, 438, 448, 458, 468, 478, 488, 498, 508,
518, 528, 538, 548, 558, 568, 578, 588, 598, 608, 618, 628, 638, 648, 658, 668,
678, 688, 698, 708, 718, 728, 738, 748, 758, 768, 778, 788, 798, 808, 818, 828,
838, 848, 858, 868, 878, 888, 898, 908, 918, 928, 938, 948, 958, 968, 978, 988,
998, 71, 73, 75, 81, 83, 91, 97, 104, 110, 119, 128, 136, 143, 152, 162, 172,
180, 189, 199, 207, 216, 226, 235, 245, 255, 265, 275, 285, 294, 304, 314, 324,
333, 343, 353, 363, 372, 382, 391, 401, 409, 419, 429, 439, 449, 459, 469, 479,
489, 499, 509, 519, 529, 539, 549, 559, 569, 579, 589, 599, 609, 619, 629, 639,
649, 659, 669, 679, 689, 699, 709, 719, 729, 739, 749, 759, 769, 779, 789, 799,
809, 819, 829, 839, 849, 859, 869, 879, 889, 899, 909, 919, 929, 939, 949, 959,
969, 979, 989, 999};

void testRcvPattern(std::deque<std::unique_ptr<FileDesc>> & file)
{
    for (size_t i = 0; i < file.size(); i++)
   {
        size_t l = 1;
        for (size_t j = 1; j < file[i]->lst.size(); j++, l++)
            EXPECT_EQ(file[i]->lst[j] - 1, file[i]->lst[j-1]);
        EXPECT_EQ(l, 1000+i);
    }
}

void testLineOffPattern(std::deque<std::unique_ptr<FileDesc>> & file)
{
    for (size_t i = 0; i < file.size(); i++)
    {
        size_t total = file[i]->lst.size();
        size_t l = 0;
        for (llint j = 0; j < 10U; j++)
            for (llint k = 0; k < total / 10; k++, l++)
                EXPECT_EQ(file[i]->lst[l], 10 * k + j) << i << " " << j << " " << k << " "  << l;
        EXPECT_EQ(l, 1000+i);
    }
}

void testOffLinePattern(std::deque<std::unique_ptr<FileDesc>> & file)
{
    for (size_t i = 0; i < file.size(); i++)
    {
        size_t l = 0;
        for (size_t j = 0; j < file[i]->lst.size(); j++, l++)
            EXPECT_EQ(sortOffLine[j], file[i]->lst[j]);
        EXPECT_EQ(l, 1000+i);
    }
}

void testSrcOffPattern(std::deque<std::unique_ptr<FileDesc>> & file)
{
    for (size_t i = 0; i < file.size(); i++)
    {
        size_t l = 1;
        for (size_t j = 1; j < file[i]->lst.size(); j++, l++)
            EXPECT_EQ(file[i]->lst[j] - 1, file[i]->lst[j-1]);
        EXPECT_EQ(l, 1000+i);
    }
}
void createTrace(size_t nt, size_t ns, float * trc)
{
    for (size_t i = 0; i < nt; i++)
        for (size_t j = 0; j < ns; j++)
    {
      trc[i*ns+j]=1;//(i+1);sin(M_PI*j*(i+1));
    }
}

TEST_F(SetTest, SortSrcX)
{
    init(1, 1, 1, 1, true);
    set->sort(SortType::SrcRcv);

    for (size_t i = 0; i < set->file.size(); i++)
    {
        size_t l = 1;
        for (size_t j = 1; j < set->file[i]->lst.size(); j++, l++)
            EXPECT_EQ(set->file[i]->lst[j] + 1, set->file[i]->lst[j-1]);
        EXPECT_EQ(l, 1000+i);
    }
}

TEST_F(SetTest, SortRcvX)
{
    init(1, 1, 1, 1, true);

    set->sort(SortType::RcvOff);
    testRcvPattern(set->file);
}

TEST_F(SetTest, SortRcvXR)
{
    init(1, 1, 1, 1, true);
    set->sort(SortType::RcvROff);
    testRcvPattern(set->file);
}

TEST_F(SetTest, SortLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::LineOff);
    testLineOffPattern(set->file);
}

TEST_F(SetTest, SortLineROff)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::LineROff);
    testLineOffPattern(set->file);
}

TEST_F(SetTest, SortOffLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::OffLine);
    testOffLinePattern(set->file);
}

TEST_F(SetTest, SortROffLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::ROffLine);
    testOffLinePattern(set->file);
}

TEST_F(SetTest, SortSortRcv)
{
    init(1, 1, 1, 2, true);

    set->sort(SortType::SrcRcv);
    set->sort(SortType::RcvOff);

    testSrcOffPattern(set->file);
}

TEST_F(SetTest, SortSortRcvROff)
{
    init(1, 1, 1, 2, true);

    set->sort(SortType::SrcRcv);
    set->sort(SortType::RcvROff);

    testSrcOffPattern(set->file);
}

TEST_F(SetTest, SortSrcXRcvY)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::SrcRcv);

    for (size_t i = 0; i < set->file.size(); i++)
    {
        size_t total = set->file[i]->lst.size();
        size_t l = 0;
        for (llint j = 0; j < 10U; j++)
            for (llint k = total / 10U - 1; k >= 0; k--, l++)
                EXPECT_EQ(set->file[i]->lst[l], 10 * k + j) << i << " " << j << " " << k << " "  << l;
        EXPECT_EQ(l, 1000+i);
    }
}

TEST_F(SetTest, getMinMax)
{
    init(1, 1, 1, 1, true);
    std::vector<CoordElem> minmax(4);
    set->getMinMax(Meta::xSrc, Meta::ySrc, minmax.data());
    EXPECT_EQ(minmax[0].val, 1001.);
    EXPECT_EQ(minmax[1].val, 2000.);
    EXPECT_EQ(minmax[2].val, 1001.);
    EXPECT_EQ(minmax[3].val, 2000.);
    EXPECT_EQ(minmax[0].num, 999);
    EXPECT_EQ(minmax[1].num, 0);
    EXPECT_EQ(minmax[2].num, 999);
    EXPECT_EQ(minmax[3].num, 0);
}

TEST_F(SetTest, getActive)
{
    init(1, 1000U, 10);

    EXPECT_EQ(set->getInNt(), 1000U);
    EXPECT_EQ(set->getLNt(), 1000U - 10U);
}

TEST_F(SetTest, getActive2)
{
    init(1, 3333, 1111);

    EXPECT_EQ(set->getInNt(), 3333U);
    EXPECT_EQ(set->getLNt(), 2222U);
}

TEST_F(SetTest, getActive3)
{
    init(2, 3333, 1111);

    EXPECT_EQ(set->getInNt(), 2U*3333U);
    EXPECT_EQ(set->getLNt(), 2U*2222U);
}

void testTaper(size_t nt, size_t ns, float * trc, std::function<float(float weight, float ramp)> func, size_t nTailLft, size_t nTailRt)
{
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
        {
            if ((wtLft < 1 ) && (trc[i*ns+j] != 0))
            {
                trc[i*ns+j]=trc[i*ns+j]*func(wtLft, nTailLft);
                ++wtLft;
            }
            else if (wtLft < nTailLft)
            {
                trc[i*ns+j]=trc[i*ns+j]*func(wtLft, nTailLft);
                ++wtLft;
            }
            else if ((ns-j) <= nTailRt)
            {
                --wtRt;
                trc[i*ns+j]=trc[i*ns+j]*func(wtRt, nTailRt);
            }
        }
    }
}

void testTaper(Set * set, size_t nt, size_t ns, size_t nTailLft, size_t nTailRt, ...)
{
    std::vector<trace_t> trc(nt * ns);
    std::vector<trace_t> origTrc(nt * ns);
    File::Param p(nt);

    std::fill(trc.begin(), trc.end(), 1.0f);
    std::fill(origTrc.begin(), origTrc.end(), 1.0f);

    set->taper(TaperType::Linear, nTailLft, nTailRt);
    set->modify(ns, &p, trc.data());

    //Continue this ...
}


TEST_F(SetTest, Taper2TailLin)
{
    size_t nt = 100;
    size_t ns = 200;
    std::vector<trace_t> trc(nt * ns);
    std::vector<trace_t> origTrc(nt * ns);
    size_t nTailLft = 50;
    size_t nTailRt = 75;
    File::Param p(nt);

    if (set.get() != nullptr)
      set.release();
    set = std::make_unique<Set>(piol);

    std::fill(trc.begin(), trc.end(), 1.0f);
    std::fill(origTrc.begin(), origTrc.end(), 1.0f);

    set->taper(TaperType::Linear, nTailLft, nTailRt);
    set->modify(ns, &p, trc.data());
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
        {
            if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
              origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((float(wtLft)-float(nTailLft))/static_cast<float>(nTailLft)));
                    ++wtLft;
            }
            else if (wtLft < nTailLft)
            {
              origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((float(wtLft)-float(nTailLft))/static_cast<float>(nTailLft)));
                ++wtLft;
            }
            else if ((ns-j) <= nTailRt)
            {
            --wtRt;
            origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtRt)-static_cast<float>(nTailRt))/static_cast<float>(nTailRt)));
            }
            EXPECT_FLOAT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper2TailCos)
{
    size_t nt = 100;
    size_t ns = 200;
    std::vector<trace_t> trc(nt * ns);
    std::vector<trace_t> origTrc(nt * ns);
    size_t nTailLft = 50;
    size_t nTailRt = 75;
    File::Param p(nt);

    if (set.get() != nullptr)
      set.release();
    set = std::make_unique<Set>(piol);

    std::fill(trc.begin(), trc.end(), 1.0f);
    std::fill(origTrc.begin(), origTrc.end(), 1.0f);

    set->taper(TaperType::Cos, nTailLft, nTailRt);
    set->modify(ns, &p, trc.data());
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
            origTrc[i*ns+j]=origTrc[i*ns+j]*(.5f + (.5f * cos(M_PI * (static_cast<float>(wtLft) -  static_cast<float>(nTailLft)) / static_cast<float>(nTailLft))));
             ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
            origTrc[i*ns+j]=origTrc[i*ns+j]*(.5f + .5f * cos(M_PI * (static_cast<float>(wtLft) -  static_cast<float>(nTailLft)) / static_cast<float>(nTailLft)));
            ++wtLft;
            }
        else if ((ns-j) <= nTailRt)
            {
            --wtRt;
            origTrc[i*ns+j]=origTrc[i*ns+j]*(.5f + .5f * cos(M_PI * (static_cast<float>(wtRt)  - static_cast<float>(nTailRt)) / static_cast<float>(nTailRt)));
            }
        }
    }
    for (size_t i = 0; i < nt; i++)
        for (size_t j = 0; j < ns; j++)
           EXPECT_FLOAT_EQ(trc[i*ns+j], origTrc[i*ns+j]) << i << " " << j << std::endl;
}

TEST_F(SetTest, Taper2TailCosSqr)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    size_t nTailRt = 75;
    File::Param * p = 0;

    if (set.get() != nullptr)
      set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(TaperType::CosSqr, nTailLft, nTailRt);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
    {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * pow(.5 + .5 * cos(M_PI * (static_cast<float>(wtLft)  - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft)), 2);
            ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
          origTrc[i*ns+j]=origTrc[i*ns+j] * pow(.5 + .5 * cos(M_PI * (static_cast<float>(wtLft)  - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft)), 2);
            ++wtLft;
            }
        else if ((ns-j) <= nTailRt)
            {
            --wtRt;
            origTrc[i*ns+j]=origTrc[i*ns+j]*pow(.5 + .5 * cos(M_PI * (static_cast<float>(wtRt)  - static_cast<float>(nTailRt)) / static_cast<float>(nTailRt)), 2);
            }
             EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
    }
    }
}

TEST_F(SetTest, Taper2TailCust)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    size_t nTailRt = 75;
    float tau = 8;
    File::Param * p = 0;

    if (set.get() != nullptr)
      set.release();
    set = std::make_unique<Set>(piol);

    std::function<float(float,float)> func = [tau](float wt, float ramp){return std::exp(-std::abs(wt-ramp)/tau);};
    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(func, nTailLft, nTailRt);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * std::exp(-std::abs(static_cast<float>(wtLft)-static_cast<float>(nTailLft))/tau);
            ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * std::exp(-std::abs(static_cast<float>(wtLft)-static_cast<float>(nTailLft))/tau);
            ++wtLft;
            }
        else if ((ns-j) <= nTailRt)
            {
            --wtRt;
            origTrc[i*ns+j]=origTrc[i*ns+j] * std::exp(-std::abs(static_cast<float>(wtRt)-static_cast<float>(nTailRt))/tau);
            }
        EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
    }
    }

}
TEST_F(SetTest, Taper1TailLin)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(TaperType::Linear, nTailLft);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * (1. - std::abs((static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft)));
            ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * (1. - std::abs((static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft)));
            ++wtLft;
            }
            EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper1TailCos)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(TaperType::Cos, nTailLft);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;

        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(.5 + (.5 * cos(M_PI * (static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft))));
                ++wtLft;
            }
             else if (wtLft < nTailLft)
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(.5 + (.5 * cos(M_PI * (static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft))));
                ++wtLft;
            }
            EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper1TailCosSqr)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(TaperType::CosSqr, nTailLft);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;

        for (size_t j=0; j<ns; j++)
    {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
        {
            origTrc[i*ns+j]=origTrc[i*ns+j]*pow(.5 + (.5 * cos(M_PI * (static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft))), 2);
            ++wtLft;
        }
            else if (wtLft < nTailLft)
            {
            origTrc[i*ns+j]=origTrc[i*ns+j]*pow(.5 + (.5 * cos(M_PI * (static_cast<float>(wtLft) - static_cast<float>(nTailLft)) / static_cast<float>(nTailLft))), 2);
                ++wtLft;
            }
            EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper1TailCust)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    float tau = 8;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    std::function<float(float,float)> func = [tau](float wt, float ramp){return std::exp(-std::abs(wt-ramp)/tau);};
    createTrace(nt, ns, trc);
    createTrace(nt, ns, origTrc);

    set->taper(func, nTailLft);
    set->modify(ns, p, trc);

    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * std::exp(-std::abs(static_cast<float>(wtLft)-static_cast<float>(nTailLft))/tau);
            ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
            origTrc[i*ns+j]=origTrc[i*ns+j] * std::exp(-std::abs(static_cast<float>(wtLft)-static_cast<float>(nTailLft))/tau);
            ++wtLft;
            }
        EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper2TailMuteLin)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 75;
    size_t nTailRt = 50;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    for (size_t i = 0; i < nt; i++)
        for (size_t j = 0; j < ns; j++)
    {
        if (j < ceil(i * 3 / nt))
                trc[i*ns+j]=0.;
        }
    origTrc = trc;

    set->taper(TaperType::Linear, nTailLft, nTailRt);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;
        size_t wtRt = nTailRt;
        for (size_t j=0; j<ns; j++)
        {
        if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0.))
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtLft)-static_cast<float>(nTailLft))/static_cast<float>(nTailLft)));
            ++wtLft;
            }
        else if (wtLft < nTailLft)
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtLft)-static_cast<float>(nTailLft))/static_cast<float>(nTailLft)));
            ++wtLft;
            }
        else if ((ns-j) <= nTailRt)
            {
            --wtRt;
            origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtRt)-static_cast<float>(nTailRt))/static_cast<float>(nTailRt)));
            }
        EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

TEST_F(SetTest, Taper1TailMuteLinear)
{
    size_t nt = 100;
    size_t ns = 200;
    float * trc = (float * )std::calloc(nt * ns, sizeof(float));
    float * origTrc = (float * )std::calloc(nt * ns, sizeof(float));
    size_t nTailLft = 50;
    File::Param * p = 0;

    if (set.get() != nullptr)
        set.release();
    set = std::make_unique<Set>(piol);

    createTrace(nt, ns, trc);
    for (size_t i = 0; i < nt; i++)
        for (size_t j = 0; j < ns; j++)
        {
        if (j < ceil(i * 3 / nt))
            trc[i*ns+j]=0.;
        }
    origTrc = trc;

    set->taper(TaperType::Linear, nTailLft);
    set->modify(ns, p, trc);
    for (size_t i=0; i < nt; i++)
    {
        size_t wtLft = 0;

        for (size_t j=0; j<ns; j++)
        {
            if ((wtLft < 1 ) && (origTrc[i*ns+j] != 0))
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtLft)-static_cast<float>(nTailLft))/static_cast<float>(nTailLft)));
                ++wtLft;
            }
            else if (wtLft < nTailLft)
            {
                origTrc[i*ns+j]=origTrc[i*ns+j]*(1.-std::abs((static_cast<float>(wtLft)-static_cast<float>(nTailLft))/static_cast<float>(nTailLft)));
                ++wtLft;
            }
            EXPECT_EQ(trc[i*ns+j],origTrc[i*ns+j]);
        }
    }
}

