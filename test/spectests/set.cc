/* These unit tests might be fairly redundant since they are basically the same
 * as the system tests.
 */
#include "settest.hh"

#include <string>
#include <vector>

const std::vector<exseis::utils::Trace_value> lowpass_time = {
  0.422621,   0.530021,  0.632801,   0.722758,  0.791212,  0.829939,  0.832105,
  0.793097,   0.711253,  0.588246,   0.429078,  0.241856,  0.0372786, -0.172197,
  -0.373282,  -0.552796, -0.698696,  -0.801013, -0.852533, -0.849352, -0.791238,
  -0.681619,  -0.527352, -0.338336,  -0.126833, 0.0933952, 0.30795,   0.502739,
  0.664977,   0.783999,  0.851935,   0.864286,  0.820235,  0.722647,  0.577927,
  0.395643,   0.187832,  -0.0317768, -0.248623, -0.448336, -0.617733, -0.745662,
  -0.823804,  -0.847317, -0.815159,  -0.730171, -0.598976, -0.431485, -0.240139,
  -0.0390237, 0.157239,  0.334753,   0.481512,  0.588441,  0.650331,  0.666335,
  0.639985,   0.57875,   0.492984};

const std::vector<exseis::utils::Trace_value> lowpass_freq = {
  0.359491,   0.214919,  0.0781371,  0.525465,   -0.175032,  0.404092,
  0.115523,   0.0978552, 0.430723,   -0.231177,  0.42044,    -0.00717656,
  0.113677,   0.314939,  -0.254676,  0.42335,    -0.112378,  0.149612,
  0.213695,   -0.230535, 0.421703,   -0.182199,  0.203526,   0.131522,
  -0.179577,  0.402108,  -0.230853,  0.250903,   0.0503087,  -0.132621,
  0.352328,   -0.271207, 0.282046,   -0.0323321, -0.0913711, 0.290405,
  -0.286138,  0.31825,   -0.0910975, -0.0287472, 0.252366,   -0.247559,
  0.384012,   -0.105317, 0.0703506,  0.252984,   -0.156939,  0.470727,
  -0.0906565, 0.185677,  0.269209,   -0.0514363, 0.541974,   -0.0869746,
  0.285883,   0.264967,  0.0310879,  0.565055,   -0.117917};

const std::vector<exseis::utils::Trace_value> bandpass_time = {
  0.0115611,  -0.248154,   -0.29143,   -0.0883583,  0.0774772,  0.118759,
  0.136654,   0.137429,    0.121763,   0.133754,    0.118454,   0.0458482,
  0.00828449, 0.00423072,  -0.0429585, -0.0805662,  -0.0689311, -0.082975,
  -0.107178,  -0.0778039,  -0.0576672, -0.07228,    -0.0444857, -0.00422329,
  -0.0118015, 0.000364362, 0.0439488,  0.0418584,   0.0333791,  0.0681351,
  0.0727694,  0.0471208,   0.0648092,  0.0758725,   0.0407095,  0.0363369,
  0.0499899,  0.0131497,   -0.0153474, -0.00594081, -0.0353369, -0.0789926,
  -0.0756111, -0.0892643,  -0.1303,    -0.116654,   -0.0882243, -0.104921,
  -0.0892009, -0.0161884,  0.0444233,  0.100936,    0.15324,    0.159191,
  0.227195,   0.421919,    0.461863,   0.104624,    -0.385033};

const std::vector<exseis::utils::Trace_value> bandpass_freq = {
  0.72428,    0.331669,  0.239852,  0.502776,  -0.409657, 0.144657,   -0.456118,
  -0.357781,  -0.243053, -0.883959, -0.272044, -1.01206,  -0.713787,  -0.649706,
  -1.04527,   -0.367378, -1.07208,  -0.536751, -0.47469,  -0.62768,   0.169675,
  -0.475295,  0.256944,  0.177763,  0.213481,  0.904333,  0.235598,   0.900613,
  0.534085,   0.50306,   0.875472,  0.0886262, 0.594238,  -0.0739399, -0.207737,
  -0.0302071, -0.842162, -0.256339, -1.02178,  -0.860615, -0.682687,  -1.23377,
  -0.383137,  -0.985877, -0.333746, -0.150852, -0.392555, 0.543525,   0.0571215,
  0.844476,   0.821583,  0.672036,  1.38692,   0.719453,  1.23663,    0.986613,
  0.81142,    1.25001,   0.325807};

const std::vector<size_t> sort_off_line = {
  0,   2,   5,   10,  16,  25,  34,  44,  57,  72,  88,  99,  109, 122, 133,
  144, 154, 165, 175, 186, 196, 206, 217, 227, 237, 248, 258, 268, 278, 288,
  298, 308, 318, 329, 339, 349, 359, 369, 379, 389, 399, 410, 420, 430, 440,
  450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
  600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740,
  750, 760, 770, 780, 790, 800, 810, 820, 830, 840, 850, 860, 870, 880, 890,
  900, 910, 920, 930, 940, 950, 960, 970, 980, 990, 1,   3,   7,   12,  18,
  27,  36,  47,  61,  74,  89,  100, 112, 124, 134, 145, 155, 166, 177, 187,
  197, 208, 218, 228, 238, 249, 259, 269, 279, 289, 299, 309, 320, 330, 340,
  350, 360, 370, 380, 390, 400, 411, 421, 431, 441, 451, 461, 471, 481, 491,
  501, 511, 521, 531, 541, 551, 561, 571, 581, 591, 601, 611, 621, 631, 641,
  651, 661, 671, 681, 691, 701, 711, 721, 731, 741, 751, 761, 771, 781, 791,
  801, 811, 821, 831, 841, 851, 861, 871, 881, 891, 901, 911, 921, 931, 941,
  951, 961, 971, 981, 991, 4,   6,   8,   14,  21,  29,  38,  51,  63,  78,
  90,  102, 114, 125, 135, 146, 157, 168, 178, 188, 198, 209, 219, 230, 240,
  250, 260, 270, 280, 290, 300, 311, 321, 331, 341, 351, 361, 371, 381, 392,
  402, 412, 422, 432, 442, 452, 462, 472, 482, 492, 502, 512, 522, 532, 542,
  552, 562, 572, 582, 592, 602, 612, 622, 632, 642, 652, 662, 672, 682, 692,
  702, 712, 722, 732, 742, 752, 762, 772, 782, 792, 802, 812, 822, 832, 842,
  852, 862, 872, 882, 892, 902, 912, 922, 932, 942, 952, 962, 972, 982, 992,
  9,   11,  13,  19,  24,  32,  42,  53,  66,  82,  93,  106, 116, 126, 138,
  149, 159, 169, 179, 190, 200, 211, 221, 231, 241, 251, 261, 271, 282, 292,
  302, 312, 322, 332, 342, 352, 362, 373, 383, 393, 403, 413, 423, 433, 443,
  453, 463, 473, 483, 493, 503, 513, 523, 533, 543, 553, 563, 573, 583, 593,
  603, 613, 623, 633, 643, 653, 663, 673, 683, 693, 703, 713, 723, 733, 743,
  753, 763, 773, 783, 793, 803, 813, 823, 833, 843, 853, 863, 873, 883, 893,
  903, 913, 923, 933, 943, 953, 963, 973, 983, 993, 15,  17,  20,  23,  30,
  40,  49,  60,  70,  84,  96,  108, 118, 130, 140, 150, 160, 171, 182, 192,
  202, 212, 222, 232, 242, 253, 263, 273, 283, 293, 303, 313, 323, 334, 344,
  354, 364, 374, 384, 394, 404, 414, 424, 434, 444, 454, 464, 474, 484, 494,
  504, 514, 524, 534, 544, 554, 564, 574, 584, 594, 604, 614, 624, 634, 644,
  654, 664, 674, 684, 694, 704, 714, 724, 734, 744, 754, 764, 774, 784, 794,
  804, 814, 824, 834, 844, 854, 864, 874, 884, 894, 904, 914, 924, 934, 944,
  954, 964, 974, 984, 994, 22,  26,  28,  31,  39,  46,  55,  68,  80,  92,
  101, 113, 121, 132, 142, 153, 163, 173, 183, 193, 204, 214, 224, 234, 244,
  254, 264, 274, 284, 295, 305, 315, 325, 335, 345, 355, 365, 375, 385, 395,
  405, 415, 425, 435, 445, 455, 465, 475, 485, 495, 505, 515, 525, 535, 545,
  555, 565, 575, 585, 595, 605, 615, 625, 635, 645, 655, 665, 675, 685, 695,
  705, 715, 725, 735, 745, 755, 765, 775, 785, 795, 805, 815, 825, 835, 845,
  855, 865, 875, 885, 895, 905, 915, 925, 935, 945, 955, 965, 975, 985, 995,
  33,  35,  37,  41,  48,  54,  64,  77,  87,  98,  107, 117, 127, 137, 147,
  158, 167, 176, 185, 195, 205, 215, 225, 236, 246, 256, 266, 276, 286, 296,
  306, 316, 326, 336, 346, 356, 366, 376, 386, 396, 406, 416, 426, 436, 446,
  456, 466, 476, 486, 496, 506, 516, 526, 536, 546, 556, 566, 576, 586, 596,
  606, 616, 626, 636, 646, 656, 666, 676, 686, 696, 706, 716, 726, 736, 746,
  756, 766, 776, 786, 796, 806, 816, 826, 836, 846, 856, 866, 876, 886, 896,
  906, 916, 926, 936, 946, 956, 966, 976, 986, 996, 43,  45,  50,  52,  59,
  67,  76,  85,  95,  105, 115, 123, 131, 141, 151, 161, 170, 181, 191, 201,
  210, 220, 229, 239, 247, 257, 267, 277, 287, 297, 307, 317, 327, 337, 347,
  357, 367, 377, 387, 397, 407, 417, 427, 437, 447, 457, 467, 477, 487, 497,
  507, 517, 527, 537, 547, 557, 567, 577, 587, 597, 607, 617, 627, 637, 647,
  657, 667, 677, 687, 697, 707, 717, 727, 737, 747, 757, 767, 777, 787, 797,
  807, 817, 827, 837, 847, 857, 867, 877, 887, 897, 907, 917, 927, 937, 947,
  957, 967, 977, 987, 997, 56,  58,  62,  65,  69,  79,  86,  94,  103, 111,
  120, 129, 139, 148, 156, 164, 174, 184, 194, 203, 213, 223, 233, 243, 252,
  262, 272, 281, 291, 301, 310, 319, 328, 338, 348, 358, 368, 378, 388, 398,
  408, 418, 428, 438, 448, 458, 468, 478, 488, 498, 508, 518, 528, 538, 548,
  558, 568, 578, 588, 598, 608, 618, 628, 638, 648, 658, 668, 678, 688, 698,
  708, 718, 728, 738, 748, 758, 768, 778, 788, 798, 808, 818, 828, 838, 848,
  858, 868, 878, 888, 898, 908, 918, 928, 938, 948, 958, 968, 978, 988, 998,
  71,  73,  75,  81,  83,  91,  97,  104, 110, 119, 128, 136, 143, 152, 162,
  172, 180, 189, 199, 207, 216, 226, 235, 245, 255, 265, 275, 285, 294, 304,
  314, 324, 333, 343, 353, 363, 372, 382, 391, 401, 409, 419, 429, 439, 449,
  459, 469, 479, 489, 499, 509, 519, 529, 539, 549, 559, 569, 579, 589, 599,
  609, 619, 629, 639, 649, 659, 669, 679, 689, 699, 709, 719, 729, 739, 749,
  759, 769, 779, 789, 799, 809, 819, 829, 839, 849, 859, 869, 879, 889, 899,
  909, 919, 929, 939, 949, 959, 969, 979, 989, 999};

void test_rcv_pattern(
  std::deque<std::shared_ptr<FileDesc>>& file, std::shared_ptr<ExSeis>& piol)
{
    for (size_t i = 0; i < file.size(); i++) {
        size_t l = 1;
        for (size_t j = 1; j < file[i]->olst.size(); j++, l++) {
            EXPECT_EQ(file[i]->olst[j] - 1, file[i]->olst[j - 1]);
        }
        EXPECT_EQ(piol->comm->sum(l), 1000 + i);
    }
}

void test_line_off_pattern(
  std::deque<std::shared_ptr<FileDesc>>& file, std::shared_ptr<ExSeis>& piol)
{
    for (size_t i = 0; i < file.size(); i++) {
        auto totals  = piol->comm->gather(file[i]->olst.size());
        size_t total = piol->comm->sum(file[i]->olst.size());

        std::vector<size_t> offsets(totals.size() + 1);
        std::partial_sum(totals.begin(), totals.end(), offsets.begin() + 1);

        std::vector<size_t> global_olst_order;
        for (size_t j = 0; j < 10U; j++) {
            for (size_t k = 0; k < total / 10; k++) {
                global_olst_order.push_back(10 * k + j);
            }
        }

        for (size_t l = 0; l < file[i]->olst.size(); l++) {
            size_t global_l = l + offsets[piol->get_rank()];
            EXPECT_EQ(file[i]->olst[l], global_olst_order[global_l]);
        }

        EXPECT_EQ(total, 1000 + i);
    }
}

void test_off_line_pattern(
  std::deque<std::shared_ptr<FileDesc>>& file, std::shared_ptr<ExSeis>& piol)
{
    for (size_t i = 0; i < file.size(); i++) {
        auto totals = piol->comm->gather(file[i]->olst.size());
        std::vector<size_t> offsets(totals.size() + 1);
        std::partial_sum(totals.begin(), totals.end(), offsets.begin() + 1);

        size_t l = 0;
        for (size_t j = 0; j < file[i]->olst.size(); j++, l++) {
            EXPECT_EQ(
              sort_off_line[j + offsets[piol->get_rank()]], file[i]->olst[j]);
        }
        EXPECT_EQ(piol->comm->sum(l), 1000 + i);
    }
}

void test_src_off_pattern(
  std::deque<std::shared_ptr<FileDesc>>& file, std::shared_ptr<ExSeis>& piol)
{
    for (size_t i = 0; i < file.size(); i++) {
        size_t l = 1;
        for (size_t j = 1; j < file[i]->olst.size(); j++, l++) {
            EXPECT_EQ(file[i]->olst[j] - 1, file[i]->olst[j - 1]);
        }
        EXPECT_EQ(piol->comm->sum(l), 1000 + i);
    }
}

void muting(size_t nt, size_t ns, exseis::utils::Trace_value* trc, size_t mute)
{
    for (size_t i = 0; i < nt; i++) {
        for (size_t j = 0; j < mute; j++) {
            trc[i * ns + j] = 0;
        }
    }
}

void taper_man(
  size_t nt,
  size_t ns,
  exseis::utils::Trace_value* trc,
  Taper_function func,
  size_t n_tail_lft,
  size_t n_tail_rt)
{
    for (size_t i = 0; i < nt; i++) {
        size_t wt_lft = 0;
        size_t wt_rt  = n_tail_rt;
        for (size_t j = 0; j < ns; j++) {
            if ((wt_lft < 1) && (trc[i * ns + j] != 0.0f)) {
                ++wt_lft;
                trc[i * ns + j] = trc[i * ns + j] * func(wt_lft, n_tail_lft);
            }
            else if ((wt_lft > 0) && (wt_lft < n_tail_lft)) {
                ++wt_lft;
                trc[i * ns + j] *= func(wt_lft, n_tail_lft);
            }
            else if ((ns - j) <= n_tail_rt) {
                trc[i * ns + j] *= func(wt_rt, n_tail_rt);
                --wt_rt;
            }
        }
    }
}

TEST_F(SetTest, SortSrcX)
{
    init(1, 1, 1, 1, true);

    set->sort(SortType::SrcRcv);
    set->calc_func(set->m_func.begin(), set->m_func.end());

    for (size_t i = 0; i < set->m_file.size(); i++) {
        size_t l = 1;
        for (size_t j = 1; j < set->m_file[i]->olst.size(); j++, l++) {
            EXPECT_EQ(set->m_file[i]->olst[j] + 1, set->m_file[i]->olst[j - 1]);
        }
        EXPECT_EQ(piol->comm->sum(l), 1000 + i);
    }
}

TEST_F(SetTest, SortRcvX)
{
    init(1, 1, 1, 1, true);

    set->sort(SortType::RcvOff);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_rcv_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortRcvXR)
{
    init(1, 1, 1, 1, true);
    set->sort(SortType::RcvROff);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_rcv_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::LineOff);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_line_off_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortLineROff)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::LineROff);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_line_off_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortOffLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::OffLine);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_off_line_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortROffLine)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::ROffLine);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    test_off_line_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortSortRcv)
{
    init(1, 1, 1, 2, true);

    set->sort(SortType::SrcRcv);
    set->sort(SortType::RcvOff);
    set->calc_func(set->m_func.begin(), set->m_func.end());

    test_src_off_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortSortRcvROff)
{
    init(1, 1, 1, 2, true);

    set->sort(SortType::SrcRcv);
    set->sort(SortType::RcvROff);
    set->calc_func(set->m_func.begin(), set->m_func.end());

    test_src_off_pattern(set->m_file, piol);
}

TEST_F(SetTest, SortSrcXRcvY)
{
    init(1, 1, 1, 1, false);

    set->sort(SortType::SrcRcv);
    set->calc_func(set->m_func.begin(), set->m_func.end());
    piol->assert_ok();

    for (size_t i = 0; i < set->m_file.size(); i++) {
        std::vector<size_t> totals =
          piol->comm->gather(set->m_file[i]->olst.size());
        size_t total = std::accumulate(totals.begin(), totals.end(), size_t(0));

        std::vector<size_t> l_offsets(totals.size() + 1);
        l_offsets[0] = 0;
        std::partial_sum(totals.begin(), totals.end(), l_offsets.begin() + 1);

        std::vector<size_t> global_olst_order;
        for (exseis::utils::Integer j = 0; j < 10U; j++) {
            for (exseis::utils::Integer k = total / 10U - 1; k >= 0; k--) {
                global_olst_order.push_back(static_cast<size_t>(10 * k + j));
            }
        }

        for (size_t l = 0; l < set->m_file[i]->olst.size(); l++) {
            size_t global_l = l + l_offsets[piol->get_rank()];
            EXPECT_EQ(set->m_file[i]->olst[l], global_olst_order[global_l]);
        }
    }
}

TEST_F(SetTest, get_min_max)
{
    init(1, 1, 1, 1, true);
    std::vector<CoordElem> minmax(4);
    set->get_min_max(Meta::x_src, Meta::y_src, minmax.data());
    EXPECT_EQ(minmax[0].val, 1001.);
    EXPECT_EQ(minmax[1].val, 2000.);
    EXPECT_EQ(minmax[2].val, 1001.);
    EXPECT_EQ(minmax[3].val, 2000.);
    EXPECT_EQ(minmax[0].num, static_cast<size_t>(999));
    EXPECT_EQ(minmax[1].num, static_cast<size_t>(0));
    EXPECT_EQ(minmax[2].num, static_cast<size_t>(999));
    EXPECT_EQ(minmax[3].num, static_cast<size_t>(0));
}

TEST_F(SetTest, getActive)
{
    init(1, 1000U, 10);

    size_t nt = 0U;
    for (auto& f : set->m_file) {
        nt += f->ifc->read_nt();
    }
    EXPECT_EQ(nt, 1000U);

    // EXPECT_EQ(set->getLNt(), 1000U - 10U);
}

TEST_F(SetTest, getActive2)
{
    init(1, 3333, 1111);

    size_t nt = 0U;
    for (auto& f : set->m_file) {
        nt += f->ifc->read_nt();
    }
    EXPECT_EQ(nt, 3333U);
    // EXPECT_EQ(set->getLNt(), 2222U);
}

TEST_F(SetTest, getActive3)
{
    init(2, 3333, 1111);

    size_t nt = 0U;
    for (auto& f : set->m_file) {
        nt += f->ifc->read_nt();
    }
    EXPECT_EQ(nt, 2U * 3333U);
    // EXPECT_EQ(set->getLNt(), 2U*2222U);
}

TEST_F(SetTest, Taper2TailLin)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp) {
          return 1.0f - std::abs((wt - ramp) / ramp);
      },
      linear_taper, 50, 60);
}

TEST_F(SetTest, Taper1TailLin)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp) {
          return 1.0f - std::abs((wt - ramp) / ramp);
      },
      linear_taper, 50, 0);
}
TEST_F(SetTest, Taper2TailCos)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          const float pi = M_PI;
          return 0.5f + 0.5 * cos(pi * (wt - ramp) / ramp);
      },
      cosine_taper, 50, 60);
}

TEST_F(SetTest, Taper1TailCos)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          const float pi = M_PI;
          return 0.5f + 0.5 * cos(pi * (wt - ramp) / ramp);
      },
      cosine_taper, 50, 0);
}
TEST_F(SetTest, Taper2TailCosSq)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          const float pi = M_PI;
          return pow(0.5f + 0.5 * cos(pi * (wt - ramp) / ramp), 2.0f);
      },
      cosine_square_taper, 50, 60);
}
TEST_F(SetTest, Taper1TailCosSq)
{
    taper_test(
      100, 200, 0,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          const float pi = M_PI;
          return pow(0.5f + 0.5 * cos(pi * (wt - ramp) / ramp), 2.0f);
      },
      cosine_square_taper, 50, 0);
}

TEST_F(SetTest, Taper2TailLinMute)
{
    taper_test(
      100, 200, 30,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          return 1.0f - std::abs((wt - ramp) / ramp);
      },
      linear_taper, 25, 30);
}

TEST_F(SetTest, Taper1TailLinMute)
{
    taper_test(
      100, 200, 30,
      [](exseis::utils::Trace_value wt, exseis::utils::Trace_value ramp)
        -> exseis::utils::Trace_value {
          return 1.0f - std::abs((wt - ramp) / ramp);
      },
      linear_taper, 50, 0);
}

TEST_F(SetTest, agcRMS)
{
    auto agc_func = [](size_t window, exseis::utils::Trace_value* trc, size_t) {
        exseis::utils::Trace_value amp = 0.0f;
        for (size_t i = 0; i < window; i++) {
            amp += pow(trc[i], 2.0f);
        }
        size_t num = std::count_if(
          &trc[0], &trc[window],
          [](exseis::utils::Trace_value j) { return j != 0.0f; });
        if (num < 1) {
            num = 1;
        }
        return std::sqrt(amp / num);
    };
    agc_test(100, 1000, rectangular_rms_gain, agc_func, 25, 1.0f);
}

TEST_F(SetTest, agcRMSTri)
{
    auto agc_func =
      [](size_t window, exseis::utils::Trace_value* trc, size_t win_cntr) {
          exseis::utils::Trace_value amp = 0.0f;
          exseis::utils::Trace_value win_full_tail =
            std::max(win_cntr, window - win_cntr - 1);
          for (size_t j = 0; j < window; j++) {
              const float scaling =
                (1.0f
                 - exseis::utils::Trace_value(
                     abs(exseis::utils::Integer(j - win_cntr)))
                     / win_full_tail);
              amp += pow(trc[j] * scaling, 2.0f);
          }
          size_t num = std::count_if(
            &trc[0], &trc[window],
            [](exseis::utils::Trace_value i) { return i != 0.0f; });
          if (num < 1) {
              num = 1;
          }
          return std::sqrt(amp / num);
      };
    agc_test(100, 1000, triangular_rms_gain, agc_func, 25, 1.0f);
}

TEST_F(SetTest, agcMeanAbs)
{
    auto agc_func = [](size_t window, exseis::utils::Trace_value* trc, size_t) {
        exseis::utils::Trace_value amp = 0.0f;
        for (size_t i = 0; i < window; i++) {
            amp += trc[i];
        }
        size_t num = std::count_if(
          &trc[0], &trc[window],
          [](exseis::utils::Trace_value j) { return j != 0.0f; });
        if (num < 1) {
            num = 1;
        }
        return std::abs(amp) / num;
    };
    agc_test(100, 1000, mean_abs_gain, agc_func, 25, 1.0f);
}

TEST_F(SetTest, agcMedian)
{
    auto agc_func = [](size_t window, exseis::utils::Trace_value* trc, size_t) {
        std::sort(&trc[0], &trc[window]);
        if (window % 2 == 0) {
            return (trc[window / 2U] + trc[(window / 2U) + 1U]) / 2.0f;
        }
        else {
            return trc[window / 2U];
        }
    };
    agc_test(100, 1000, median_gain, agc_func, 25, 1.0f);
}

TEST_F(SetTest, FilterOneTailTime)
{
    std::vector<exseis::utils::Trace_value> c = {1.667, 0};
    ASSERT_EQ(lowpass_time.size(), static_cast<size_t>(59));
    filter_test(FltrType::Lowpass, FltrDmn::Time, c, lowpass_time);
}

TEST_F(SetTest, FilterOneTailFreq)
{
    std::vector<exseis::utils::Trace_value> c = {1.667, 0};
    ASSERT_EQ(lowpass_freq.size(), static_cast<size_t>(59));
    filter_test(FltrType::Lowpass, FltrDmn::Freq, c, lowpass_freq);
}

TEST_F(SetTest, FilterTwoTailTime)
{
    std::vector<exseis::utils::Trace_value> c = {1.667, 6.5};
    ASSERT_EQ(bandpass_time.size(), static_cast<size_t>(59));
    filter_test(FltrType::Bandpass, FltrDmn::Time, c, bandpass_time);
}

TEST_F(SetTest, FilterTwoTailFreq)
{
    std::vector<exseis::utils::Trace_value> c = {1.667, 6.5};
    ASSERT_EQ(bandpass_freq.size(), static_cast<size_t>(59));
    filter_test(FltrType::Bandpass, FltrDmn::Freq, c, bandpass_freq);
}
