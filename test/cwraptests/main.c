#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ExSeisDat/Flow.h"
#include "ExSeisDat/PIOL.h"

#include "wraptests/wraptests.h"


/* List of all the PIOL_Meta values */
const PIOL_Meta metas[] = {
  PIOL_META_COPY,      PIOL_META_ltn,       PIOL_META_gtn,
  PIOL_META_tnl,       PIOL_META_tnr,       PIOL_META_tn,
  PIOL_META_tne,       PIOL_META_ns,        PIOL_META_inc,
  PIOL_META_Tic,       PIOL_META_SrcNum,    PIOL_META_ShotNum,
  PIOL_META_VStack,    PIOL_META_HStack,    PIOL_META_Offset,
  PIOL_META_RGElev,    PIOL_META_SSElev,    PIOL_META_SDElev,
  PIOL_META_WtrDepSrc, PIOL_META_WtrDepRcv, PIOL_META_xSrc,
  PIOL_META_ySrc,      PIOL_META_xRcv,      PIOL_META_yRcv,
  PIOL_META_xCmp,      PIOL_META_yCmp,      PIOL_META_il,
  PIOL_META_xl,        PIOL_META_TransUnit, PIOL_META_TraceUnit,
  PIOL_META_dsdr,      PIOL_META_Misc1,     PIOL_META_Misc2,
  PIOL_META_Misc3,     PIOL_META_Misc4};


/* List of all the PIOL_SortType values */
const PIOL_SortType sort_types[] = {
  PIOL_SORTTYPE_SrcRcv,   PIOL_SORTTYPE_SrcOff,  PIOL_SORTTYPE_SrcROff,
  PIOL_SORTTYPE_RcvOff,   PIOL_SORTTYPE_RcvROff, PIOL_SORTTYPE_LineOff,
  PIOL_SORTTYPE_LineROff, PIOL_SORTTYPE_OffLine, PIOL_SORTTYPE_ROffLine};


/* List of all the PIOL_TaperType values */
const PIOL_TaperType taper_types[] = {PIOL_TAPERTYPE_Linear, PIOL_TAPERTYPE_Cos,
                                      PIOL_TAPERTYPE_CosSqr};


/* List of all the exseis_Gain_function values */
const exseis_Gain_function agc_types[] = {
  exseis_rectangular_RMS_gain, exseis_triangular_RMS_gain, exseis_mean_abs_gain,
  exseis_median_gain};


/* Functions for testing PIOL_Set_sort_fn */
bool set_sort_function_true(const PIOL_File_Param* param, size_t i, size_t j)
{
    if (param == NULL && i == 840 && j == 850) wraptest_ok();
    return true;
}
bool set_sort_function_false(const PIOL_File_Param* param, size_t i, size_t j)
{
    if (param == NULL && i == 860 && j == 870) wraptest_ok();
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

    PIOL_ExSeis* piol = PIOL_ExSeis_new(0);

    PIOL_ExSeis* piol_tmp_1 = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_ExSeis* piol_tmp_2 = PIOL_ExSeis_new(PIOL_VERBOSITY_MINIMAL);
    PIOL_ExSeis* piol_tmp_3 = PIOL_ExSeis_new(PIOL_VERBOSITY_EXTENDED);
    PIOL_ExSeis* piol_tmp_4 = PIOL_ExSeis_new(PIOL_VERBOSITY_VERBOSE);
    PIOL_ExSeis* piol_tmp_5 = PIOL_ExSeis_new(PIOL_VERBOSITY_MAX);

    if (PIOL_ExSeis_getRank(piol) == 0) wraptest_ok();
    if (PIOL_ExSeis_getNumRank(piol) == 10) wraptest_ok();
    PIOL_ExSeis_barrier(piol);

    if (PIOL_ExSeis_max(piol, 0) == 30) wraptest_ok();
    if (PIOL_ExSeis_max(piol, 40) == 50) wraptest_ok();

    PIOL_ExSeis_isErr(piol, NULL);
    PIOL_ExSeis_isErr(piol, "Test isErr message");

    PIOL_ExSeis_delete(piol_tmp_5);
    PIOL_ExSeis_delete(piol_tmp_4);
    PIOL_ExSeis_delete(piol_tmp_3);
    PIOL_ExSeis_delete(piol_tmp_2);
    PIOL_ExSeis_delete(piol_tmp_1);


    /*
    ** Testing Rule
    */
    printf("Testing Rule\n");
    fflush(stdout);

    PIOL_File_Rule* rule = PIOL_File_Rule_new(true);

    PIOL_File_Rule* rule_tmp = PIOL_File_Rule_new(false);

    PIOL_File_Rule* rule_tmp2 =
      PIOL_File_Rule_new_from_list(metas, sizeof(metas) / sizeof(metas[0]));

    if (PIOL_File_Rule_addRule_Meta(rule, PIOL_META_COPY) == true)
        wraptest_ok();
    if (PIOL_File_Rule_addRule_Meta(rule, PIOL_META_COPY) == false)
        wraptest_ok();
    PIOL_File_Rule_addRule_Rule(rule, rule_tmp);

    PIOL_File_Rule_addLong(rule, PIOL_META_COPY, PIOL_TR_SeqNum);
    PIOL_File_Rule_addSEGYFloat(
      rule, PIOL_META_COPY, PIOL_TR_SeqNum, PIOL_TR_SeqNum);
    PIOL_File_Rule_addShort(rule, PIOL_META_COPY, PIOL_TR_SeqNum);
    PIOL_File_Rule_addIndex(rule, PIOL_META_COPY);
    PIOL_File_Rule_addCopy(rule);
    PIOL_File_Rule_rmRule(rule, PIOL_META_COPY);
    if (PIOL_File_Rule_extent(rule) == 100) wraptest_ok();
    if (PIOL_File_Rule_memUsage(rule) == 110) wraptest_ok();
    if (PIOL_File_Rule_paramMem(rule) == 120) wraptest_ok();

    PIOL_File_Rule_delete(rule_tmp2);
    PIOL_File_Rule_delete(rule_tmp);


    /*
    ** Testing SEGY_utils
    */
    printf("Testing SEGY_utils\n");
    fflush(stdout);

    if (PIOL_SEGY_utils_getTextSz() != 3200u) {
        printf(
          "PIOL_SEGY_utils_getTextSz() expected return value of %u, got %lu\n",
          3200, PIOL_SEGY_utils_getTextSz());
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (PIOL_SEGY_utils_getFileSz(200, 210) != 219600u) {
        printf(
          "PIOL_SEGY_utils_getFileSz(200, 210) expected return value of %u, got %lu\n",
          219600u, PIOL_SEGY_utils_getFileSz(200, 210));
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (PIOL_SEGY_utils_getDFSz(220) != 880u) {
        printf(
          "PIOL_SEGY_utils_getDFSz(220) expected return value of %u, got %lu\n",
          880u, PIOL_SEGY_utils_getDFSz(220));
        fflush(stdout);
        return EXIT_FAILURE;
    }
    if (PIOL_SEGY_utils_getMDSz() != 240u) {
        printf(
          "PIOL_SEGY_utils_getMDSz() expected return value of %u, got %lu\n",
          240u, PIOL_SEGY_utils_getMDSz());
        fflush(stdout);
        return EXIT_FAILURE;
    }


    /*
    ** Param calls
    */
    printf("Testing Param\n");
    fflush(stdout);

    PIOL_File_Param* param     = PIOL_File_Param_new(rule, 300);
    PIOL_File_Param* param_tmp = PIOL_File_Param_new(NULL, 310);

    if (PIOL_File_Param_size(param) == 320) wraptest_ok();
    if (PIOL_File_Param_memUsage(param) == 330) wraptest_ok();

    if (PIOL_File_getPrm_short(340, PIOL_META_COPY, param) == 350)
        wraptest_ok();

    if (PIOL_File_getPrm_Integer(360, PIOL_META_COPY, param) == 370)
        wraptest_ok();
    if (
      fabs(PIOL_File_getPrm_double(380, PIOL_META_COPY, param) - 390.0)
      < 1e-5) {
        wraptest_ok();
    }

    PIOL_File_setPrm_short(400, PIOL_META_COPY, 410, param);
    PIOL_File_setPrm_Integer(420, PIOL_META_COPY, 430, param);
    PIOL_File_setPrm_double(440, PIOL_META_COPY, 450.0, param);

    PIOL_File_cpyPrm(460, param, 470, param_tmp);

    PIOL_File_Param_delete(param_tmp);


    /*
    ** Operations
    */
    printf("Testing Operations\n");
    fflush(stdout);

    struct PIOL_CoordElem coord_elem = {.val = 500.0, .num = 510};
    PIOL_File_getMinMax(
      piol, 520, 530, PIOL_META_COPY, PIOL_META_COPY, param, &coord_elem);
    if (fabs(coord_elem.val - 540.0) < 1e-5 && coord_elem.num == 550.0) {
        wraptest_ok();
    }

    /*
    ** ReadDirect
    */
    printf("Testing ReadDirect\n");
    fflush(stdout);

    PIOL_File_ReadDirect* read_direct =
      PIOL_File_ReadDirect_new(piol, "Test_ReadDirect_filename");

    if (
      strcmp(PIOL_File_ReadDirect_readText(read_direct), "Test ReadDirect Text")
      == 0) {
        wraptest_ok();
    }

    const size_t read_direct_ns = PIOL_File_ReadDirect_readNs(read_direct);
    if (read_direct_ns == 600) wraptest_ok();
    if (PIOL_File_ReadDirect_readNt(read_direct) == 610) wraptest_ok();
    if (fabs(PIOL_File_ReadDirect_readInc(read_direct) - 620.0) < 1e-5) {
        wraptest_ok();
    };

    PIOL_File_ReadDirect_readParam(read_direct, 630, 640, param);

    size_t* read_direct_offsets = malloc(650 * sizeof(size_t));
    for (size_t i = 0; i < 650; i++) {
        read_direct_offsets[i] = i;
    }
    PIOL_File_ReadDirect_readParamNonContiguous(
      read_direct, 650, read_direct_offsets, param);

    float* read_direct_trace = calloc(read_direct_ns * 670, sizeof(float));
    PIOL_File_ReadDirect_readTrace(
      read_direct, 660, 670, read_direct_trace, param);

    // readTrace sets trace[i] = i;
    int read_direct_trace_ok = 1;
    for (size_t i = 0; i < read_direct_ns * 670; i++) {
        if (fabs(read_direct_trace[i] - 1.0 * i) > 1e-5) {
            read_direct_trace_ok = 0;
        }
    }
    if (read_direct_trace_ok == 1) wraptest_ok();
    free(read_direct_trace);
    read_direct_trace = NULL;


    read_direct_offsets = malloc(680 * sizeof(size_t));
    for (size_t i = 0; i < 680; i++) {
        read_direct_offsets[i] = i;
    }
    read_direct_trace = calloc(read_direct_ns * 680, sizeof(float));
    PIOL_File_ReadDirect_readTraceNonContiguous(
      read_direct, 680, read_direct_offsets, read_direct_trace, param);
    for (size_t i = 0; i < read_direct_ns * 680; i++) {
        if (read_direct_trace[i] != i) read_direct_trace_ok = 0;
    }
    if (read_direct_trace_ok == 1) wraptest_ok();
    free(read_direct_trace);
    free(read_direct_offsets);

    read_direct_offsets = malloc(690 * sizeof(size_t));
    for (size_t i = 0; i < 690; i++) {
        read_direct_offsets[i] = i;
    }
    read_direct_trace = calloc(read_direct_ns * 690, sizeof(float));
    PIOL_File_ReadDirect_readTraceNonMonotonic(
      read_direct, 690, read_direct_offsets, read_direct_trace, param);
    for (size_t i = 0; i < read_direct_ns * 690; i++) {
        if (read_direct_trace[i] != i) read_direct_trace_ok = 0;
    }
    if (read_direct_trace_ok == 1) wraptest_ok();
    free(read_direct_trace);
    free(read_direct_offsets);


    PIOL_File_ReadDirect_delete(read_direct);


    /*
    ** WriteDirect
    */
    printf("Testing WriteDirect\n");
    fflush(stdout);

    PIOL_File_WriteDirect* write_direct =
      PIOL_File_WriteDirect_new(piol, "Test_WriteDirect_filename");

    PIOL_File_WriteDirect_writeText(write_direct, "Test WriteDirect Text");

    const int write_direct_ns = 700;
    PIOL_File_WriteDirect_writeNs(write_direct, write_direct_ns);

    PIOL_File_WriteDirect_writeNt(write_direct, 710);

    PIOL_File_WriteDirect_writeInc(write_direct, 720.0);

    PIOL_File_WriteDirect_writeParam(write_direct, 730, 740, param);

    size_t* write_direct_offsets = malloc(750 * sizeof(size_t));
    for (size_t i = 0; i < 750; i++) {
        write_direct_offsets[i] = i;
    }
    PIOL_File_WriteDirect_writeParamNonContiguous(
      write_direct, 750, write_direct_offsets, param);
    free(write_direct_offsets);

    float* write_direct_trace = malloc(write_direct_ns * 770 * sizeof(float));
    for (size_t i = 0; i < write_direct_ns * 770; i++) {
        write_direct_trace[i] = 1.0 * i;
    }
    PIOL_File_WriteDirect_writeTrace(
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
    PIOL_File_WriteDirect_writeTraceNonContiguous(
      write_direct, 780, write_direct_offsets, write_direct_trace, param);
    free(write_direct_trace);
    free(write_direct_offsets);

    PIOL_File_WriteDirect_delete(write_direct);


    /*
    ** Set
    */
    printf("Testing Set\n");
    fflush(stdout);

    PIOL_Set* set = PIOL_Set_new(piol, "Test_Set_pattern*.segy");

    struct PIOL_CoordElem set_coord_elem = {.val = 800.0, .num = 810};
    PIOL_Set_getMinMax(set, PIOL_META_COPY, PIOL_META_COPY, &set_coord_elem);
    if (fabs(set_coord_elem.val - 820.0) < 1e-5 && set_coord_elem.num == 830) {
        wraptest_ok();
    }

    for (size_t i = 0; i < sizeof(sort_types) / sizeof(sort_types[0]); i++) {
        PIOL_Set_sort(set, sort_types[i]);
    }

    PIOL_Set_sort_fn(set, set_sort_function_true);
    PIOL_Set_sort_fn(set, set_sort_function_false);

    for (size_t i = 0; i < sizeof(taper_types) / sizeof(taper_types[0]); i++) {
        PIOL_Set_taper(set, taper_types[i], 880, 890);
    }

    PIOL_Set_output(set, "Test_Set_output_name.segy");

    PIOL_Set_text(set, "Test Set text");

    PIOL_Set_summary(set);

    PIOL_Set_add(set, "Test Set add");

    for (size_t i = 0; i < sizeof(agc_types) / sizeof(agc_types[0]); i++) {
        PIOL_Set_AGC(set, agc_types[i], 900, 910.0);
    }

    PIOL_Set_delete(set);


    PIOL_File_Param_delete(param);
    PIOL_File_Rule_delete(rule);
    PIOL_ExSeis_delete(piol);

    printf("cwraptests: Done!\n");
    fflush(stdout);
    return 0;
}
