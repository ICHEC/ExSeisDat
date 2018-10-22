#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exseisdat/flow.h"
#include "exseisdat/piol.h"

#include "wraptests/wraptests.h"


/* List of all the exseis_Meta values */
const exseis_Meta metas[] = {
  exseis_meta_Copy,      exseis_meta_ltn,       exseis_meta_gtn,
  exseis_meta_tnl,       exseis_meta_tnr,       exseis_meta_tn,
  exseis_meta_tne,       exseis_meta_ns,        exseis_meta_sample_interval,
  exseis_meta_Tic,       exseis_meta_SrcNum,    exseis_meta_ShotNum,
  exseis_meta_VStack,    exseis_meta_HStack,    exseis_meta_Offset,
  exseis_meta_RGElev,    exseis_meta_SSElev,    exseis_meta_SDElev,
  exseis_meta_WtrDepSrc, exseis_meta_WtrDepRcv, exseis_meta_x_src,
  exseis_meta_y_src,     exseis_meta_x_rcv,     exseis_meta_y_rcv,
  exseis_meta_xCmp,      exseis_meta_yCmp,      exseis_meta_il,
  exseis_meta_xl,        exseis_meta_TransUnit, exseis_meta_TraceUnit,
  exseis_meta_dsdr,      exseis_meta_Misc1,     exseis_meta_Misc2,
  exseis_meta_Misc3,     exseis_meta_Misc4};


/* List of all the exseis_SortType values */
const exseis_SortType sort_types[] = {
  exseis_sorttype_SrcRcv,   exseis_sorttype_SrcOff,  exseis_sorttype_SrcROff,
  exseis_sorttype_RcvOff,   exseis_sorttype_RcvROff, exseis_sorttype_LineOff,
  exseis_sorttype_LineROff, exseis_sorttype_OffLine, exseis_sorttype_ROffLine};


/* List of all the PIOL_TaperType values */
const exseis_Taper_function taper_types[] = {
  exseis_linear_taper, exseis_cosine_taper, exseis_cosine_square_taper};


/* List of all the exseis_Gain_function values */
const exseis_Gain_function agc_types[] = {
  exseis_rectangular_rms_gain, exseis_triangular_rms_gain, exseis_mean_abs_gain,
  exseis_median_gain};


/* Functions for testing piol_set_sort_fn */
bool set_sort_function_true(
  const piol_file_trace_metadata* param, size_t i, size_t j)
{
    (void)param;

    if (i == 840 && j == 850) {
        wraptest_ok();
    }

    return true;
}
bool set_sort_function_false(
  const piol_file_trace_metadata* param, size_t i, size_t j)
{
    (void)param;

    if (i == 860 && j == 870) {
        wraptest_ok();
    }

    return false;
}


int main()
{
    printf("cwraptests: Initializing!\n");
    fflush(stdout);
    init_wraptests();


    /*
    ** Testing ExSeis
    */
    printf("Testing ExSeis\n");
    fflush(stdout);

    piol_exseis* piol = piol_exseis_new(0);

    piol_exseis* piol_tmp_1 = piol_exseis_new(exseis_verbosity_none);
    piol_exseis* piol_tmp_2 = piol_exseis_new(exseis_verbosity_minimal);
    piol_exseis* piol_tmp_3 = piol_exseis_new(exseis_verbosity_extended);
    piol_exseis* piol_tmp_4 = piol_exseis_new(exseis_verbosity_verbose);
    piol_exseis* piol_tmp_5 = piol_exseis_new(exseis_verbosity_max);

    if (piol_exseis_get_rank(piol) == 0) {
        wraptest_ok();
    }
    if (piol_exseis_get_num_rank(piol) == 10) {
        wraptest_ok();
    }
    piol_exseis_barrier(piol);

    if (piol_exseis_max(piol, 0) == 30) {
        wraptest_ok();
    }
    if (piol_exseis_max(piol, 40) == 50) {
        wraptest_ok();
    }

    piol_exseis_assert_ok(piol, NULL);
    piol_exseis_assert_ok(piol, "Test assert_ok message");

    piol_exseis_delete(piol_tmp_5);
    piol_exseis_delete(piol_tmp_4);
    piol_exseis_delete(piol_tmp_3);
    piol_exseis_delete(piol_tmp_2);
    piol_exseis_delete(piol_tmp_1);


    /*
    ** Testing Rule
    */
    printf("Testing Rule\n");
    fflush(stdout);

    piol_file_rule* rule = piol_file_rule_new(true);

    piol_file_rule* rule_tmp = piol_file_rule_new(false);

    piol_file_rule* rule_tmp2 =
      piol_file_rule_new_from_list(metas, sizeof(metas) / sizeof(metas[0]));

    if (piol_file_rule_add_rule_meta(rule, exseis_meta_Copy) == true) {
        wraptest_ok();
    }
    if (piol_file_rule_add_rule_meta(rule, exseis_meta_Copy) == false) {
        wraptest_ok();
    }
    piol_file_rule_add_rule_rule(rule, rule_tmp);

    piol_file_rule_add_long(rule, exseis_meta_Copy, exseis_tr_SeqNum);
    piol_file_rule_add_segy_float(
      rule, exseis_meta_Copy, exseis_tr_SeqNum, exseis_tr_SeqNum);
    piol_file_rule_add_short(rule, exseis_meta_Copy, exseis_tr_SeqNum);
    piol_file_rule_add_index(rule, exseis_meta_Copy);
    piol_file_rule_add_copy(rule);
    piol_file_rule_rm_rule(rule, exseis_meta_Copy);
    if (piol_file_rule_extent(rule) == 100) {
        wraptest_ok();
    }
    if (piol_file_rule_memory_usage(rule) == 110) {
        wraptest_ok();
    }
    if (piol_file_rule_memory_usage_per_header(rule) == 120) {
        wraptest_ok();
    }

    piol_file_rule_delete(rule_tmp2);
    piol_file_rule_delete(rule_tmp);


    /*
    ** Testing segy
    */
    printf("Testing segy\n");
    fflush(stdout);

    if (piol_segy_segy_text_header_size() != 3200u) {
        printf(
          "piol_segy_segy_text_header_size() expected return value of %u, got %lu\n",
          3200, piol_segy_segy_text_header_size());
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (piol_segy_get_file_size(200, 210) != 219600u) {
        printf(
          "piol_segy_get_file_size(200, 210) expected return value of %u, got %lu\n",
          219600u, piol_segy_get_file_size(200, 210));
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (piol_segy_segy_trace_data_size(220) != 880u) {
        printf(
          "piol_segy_segy_trace_data_size(220) expected return value of %u, got %lu\n",
          880u, piol_segy_segy_trace_data_size(220));
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (piol_segy_segy_trace_header_size() != 240u) {
        printf(
          "piol_segy_segy_trace_header_size() expected return value of %u, got %lu\n",
          240u, piol_segy_segy_trace_header_size());
        fflush(stdout);
        return EXIT_FAILURE;
    }


    /*
    ** Trace_metadata calls
    */
    printf("Testing Trace_metadata\n");
    fflush(stdout);

    piol_file_trace_metadata* param = piol_file_trace_metadata_new(rule, 300);
    piol_file_trace_metadata* param_tmp =
      piol_file_trace_metadata_new(NULL, 310);

    if (piol_file_trace_metadata_size(param) == 320) {
        wraptest_ok();
    }
    if (piol_file_trace_metadata_memory_usage(param) == 330) {
        wraptest_ok();
    }

    if (piol_file_get_prm_integer(340, exseis_meta_Copy, param) == 350) {
        wraptest_ok();
    }

    if (piol_file_get_prm_index(360, exseis_meta_Copy, param) == 370) {
        wraptest_ok();
    }

    if (
      fabs(piol_file_get_prm_double(380, exseis_meta_Copy, param) - 390.0)
      < 1e-5) {
        wraptest_ok();
    }

    piol_file_set_prm_integer(400, exseis_meta_Copy, 410, param);
    piol_file_set_prm_index(420, exseis_meta_Copy, 430, param);
    piol_file_set_prm_double(440, exseis_meta_Copy, 450.0, param);

    piol_file_cpy_prm(470, param_tmp, 460, param);

    piol_file_trace_metadata_delete(param_tmp);


    /*
    ** Operations
    */
    printf("Testing Operations\n");
    fflush(stdout);

    struct PIOL_CoordElem coord_elem = {.val = 500.0, .num = 510};
    piol_file_get_min_max(
      piol, 520, 530, exseis_meta_Copy, exseis_meta_Copy, param, &coord_elem);
    if (fabs(coord_elem.val - 540.0) < 1e-5 && coord_elem.num == 550.0) {
        wraptest_ok();
    }

    /*
    ** ReadInterface
    */
    printf("Testing ReadInterface\n");
    fflush(stdout);

    piol_file_read_interface* read_direct =
      piol_file_read_segy_new(piol, "Test_ReadInterface_filename");

    if (
      strcmp(
        piol_file_read_interface_read_text(read_direct),
        "Test ReadInterface Text")
      == 0) {
        wraptest_ok();
    }

    const size_t read_direct_ns = piol_file_read_interface_read_ns(read_direct);
    if (read_direct_ns == 600) {
        wraptest_ok();
    }
    if (piol_file_read_interface_read_nt(read_direct) == 610) {
        wraptest_ok();
    }
    if (
      fabs(piol_file_read_interface_read_sample_interval(read_direct) - 620.0)
      < 1e-5) {
        wraptest_ok();
    };

    piol_file_read_interface_read_param(read_direct, 630, 640, param);

    size_t* read_direct_offsets = malloc(650 * sizeof(size_t));
    for (size_t i = 0; i < 650; i++) {
        read_direct_offsets[i] = i;
    }
    piol_file_read_interface_read_param_non_contiguous(
      read_direct, 650, read_direct_offsets, param);

    float* read_direct_trace = calloc(read_direct_ns * 670, sizeof(float));
    piol_file_read_interface_read_trace(
      read_direct, 660, 670, read_direct_trace, param);

    // read_trace sets trace[i] = i;
    int read_direct_trace_ok = 1;
    for (size_t i = 0; i < read_direct_ns * 670; i++) {
        if (fabs(read_direct_trace[i] - 1.0 * i) > 1e-5) {
            read_direct_trace_ok = 0;
        }
    }
    if (read_direct_trace_ok == 1) {
        wraptest_ok();
    }
    free(read_direct_trace);
    read_direct_trace = NULL;


    read_direct_offsets = malloc(680 * sizeof(size_t));
    for (size_t i = 0; i < 680; i++) {
        read_direct_offsets[i] = i;
    }
    read_direct_trace = calloc(read_direct_ns * 680, sizeof(float));
    piol_file_read_interface_read_trace_non_contiguous(
      read_direct, 680, read_direct_offsets, read_direct_trace, param);
    for (size_t i = 0; i < read_direct_ns * 680; i++) {
        if (read_direct_trace[i] != i) {
            read_direct_trace_ok = 0;
        }
    }
    if (read_direct_trace_ok == 1) {
        wraptest_ok();
    }
    free(read_direct_trace);
    free(read_direct_offsets);

    read_direct_offsets = malloc(690 * sizeof(size_t));
    for (size_t i = 0; i < 690; i++) {
        read_direct_offsets[i] = i;
    }
    read_direct_trace = calloc(read_direct_ns * 690, sizeof(float));
    piol_file_read_interface_read_trace_non_monotonic(
      read_direct, 690, read_direct_offsets, read_direct_trace, param);
    for (size_t i = 0; i < read_direct_ns * 690; i++) {
        if (read_direct_trace[i] != i) {
            read_direct_trace_ok = 0;
        }
    }
    if (read_direct_trace_ok == 1) {
        wraptest_ok();
    }
    free(read_direct_trace);
    free(read_direct_offsets);


    piol_file_read_interface_delete(read_direct);


    /*
    ** WriteInterface
    */
    printf("Testing WriteInterface\n");
    fflush(stdout);

    piol_file_write_interface* write_direct =
      piol_file_write_segy_new(piol, "Test_WriteInterface_filename");

    piol_file_write_interface_write_text(
      write_direct, "Test WriteInterface Text");

    const int write_direct_ns = 700;
    piol_file_write_interface_write_ns(write_direct, write_direct_ns);

    piol_file_write_interface_write_nt(write_direct, 710);

    piol_file_write_interface_write_sample_interval(write_direct, 720.0);

    piol_file_write_interface_write_param(write_direct, 730, 740, param);

    size_t* write_direct_offsets = malloc(750 * sizeof(size_t));
    for (size_t i = 0; i < 750; i++) {
        write_direct_offsets[i] = i;
    }
    piol_file_write_interface_write_param_non_contiguous(
      write_direct, 750, write_direct_offsets, param);
    free(write_direct_offsets);

    float* write_direct_trace = malloc(write_direct_ns * 770 * sizeof(float));
    for (size_t i = 0; i < write_direct_ns * 770; i++) {
        write_direct_trace[i] = 1.0 * i;
    }
    piol_file_write_interface_write_trace(
      write_direct, 760, 770, write_direct_trace, param);
    free(write_direct_trace);

    write_direct_offsets = malloc(780 * sizeof(size_t));
    write_direct_trace   = malloc(write_direct_ns * 780 * sizeof(float));
    for (size_t i = 0; i < 780; i++) {
        write_direct_offsets[i] = i;
    }
    for (size_t i = 0; i < write_direct_ns * 780; i++) {
        write_direct_trace[i] = 1.0 * i;
    }
    piol_file_write_interface_write_trace_non_contiguous(
      write_direct, 780, write_direct_offsets, write_direct_trace, param);
    free(write_direct_trace);
    free(write_direct_offsets);

    piol_file_write_interface_delete(write_direct);


    /*
    ** Set
    */
    printf("Testing Set\n");
    fflush(stdout);

    PIOL_Set* set = piol_set_new(piol, "Test_Set_pattern*.segy");

    struct PIOL_CoordElem set_coord_elem = {.val = 800.0, .num = 810};
    piol_set_get_min_max(
      set, exseis_meta_Copy, exseis_meta_Copy, &set_coord_elem);
    if (fabs(set_coord_elem.val - 820.0) < 1e-5 && set_coord_elem.num == 830) {
        wraptest_ok();
    }

    for (size_t i = 0; i < sizeof(sort_types) / sizeof(sort_types[0]); i++) {
        piol_set_sort(set, sort_types[i]);
    }

    piol_set_sort_fn(set, set_sort_function_true);
    piol_set_sort_fn(set, set_sort_function_false);

    for (size_t i = 0; i < sizeof(taper_types) / sizeof(taper_types[0]); i++) {
        piol_set_taper(set, taper_types[i], 880, 890);
    }

    piol_set_output(set, "Test_Set_output_name.segy");

    piol_set_text(set, "Test Set text");

    piol_set_summary(set);

    piol_set_add(set, "Test Set add");

    for (size_t i = 0; i < sizeof(agc_types) / sizeof(agc_types[0]); i++) {
        piol_set_agc(set, agc_types[i], 900, 910.0);
    }

    piol_set_delete(set);


    piol_file_trace_metadata_delete(param);
    piol_file_rule_delete(rule);
    piol_exseis_delete(piol);

    printf("cwraptests: Done!\n");
    fflush(stdout);
    return 0;
}
