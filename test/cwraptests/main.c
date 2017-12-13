#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cfileapi.h"
#include "flow.h"

#include "wraptests/wraptests.h"


/* List of all the PIOL_Meta values */
const PIOL_Meta meta_list[] = {
    PIOL_META_COPY,
    PIOL_META_ltn,
    PIOL_META_gtn,
    PIOL_META_tnl,
    PIOL_META_tnr,
    PIOL_META_tn,
    PIOL_META_tne,
    PIOL_META_ns,
    PIOL_META_inc,
    PIOL_META_Tic,
    PIOL_META_SrcNum,
    PIOL_META_ShotNum,
    PIOL_META_VStack,
    PIOL_META_HStack,
    PIOL_META_Offset,
    PIOL_META_RGElev,
    PIOL_META_SSElev,
    PIOL_META_SDElev,
    PIOL_META_WtrDepSrc,
    PIOL_META_WtrDepRcv,
    PIOL_META_xSrc,
    PIOL_META_ySrc,
    PIOL_META_xRcv,
    PIOL_META_yRcv,
    PIOL_META_xCmp,
    PIOL_META_yCmp,
    PIOL_META_il,
    PIOL_META_xl,
    PIOL_META_TransUnit,
    PIOL_META_TraceUnit,
    PIOL_META_dsdr,
    PIOL_META_Misc1,
    PIOL_META_Misc2,
    PIOL_META_Misc3,
    PIOL_META_Misc4
};


int main()
{
    printf("cwraptests: Initializing!\n");
    init_wraptests();

    /*
    ** Testing ExSeis
    */
    PIOL_ExSeis* piol = PIOL_ExSeis_new(0);

    PIOL_ExSeis* piol_tmp_1 = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_ExSeis* piol_tmp_2 = PIOL_ExSeis_new(PIOL_VERBOSITY_MINIMAL);
    PIOL_ExSeis* piol_tmp_3 = PIOL_ExSeis_new(PIOL_VERBOSITY_EXTENDED);
    PIOL_ExSeis* piol_tmp_4 = PIOL_ExSeis_new(PIOL_VERBOSITY_VERBOSE);
    PIOL_ExSeis* piol_tmp_5 = PIOL_ExSeis_new(PIOL_VERBOSITY_MAX);

    if(PIOL_ExSeis_getRank(piol)    == 0)  wraptest_ok();
    if(PIOL_ExSeis_getNumRank(piol) == 10) wraptest_ok();
    PIOL_ExSeis_barrier(piol);

    if(PIOL_ExSeis_max(piol, 0)  == 30) wraptest_ok();
    if(PIOL_ExSeis_max(piol, 40) == 50) wraptest_ok();

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

    PIOL_File_Rule* rule = PIOL_File_Rule_new(true);

    PIOL_File_Rule* rule_tmp = PIOL_File_Rule_new(false);

    PIOL_File_Rule* rule_tmp2 = PIOL_File_Rule_new_from_list(
        meta_list, sizeof(meta_list)/sizeof(meta_list[0])
    );
    
    if(PIOL_File_Rule_addRule_Meta(rule, PIOL_META_COPY) == true)  wraptest_ok();
    if(PIOL_File_Rule_addRule_Meta(rule, PIOL_META_COPY) == false) wraptest_ok();
    PIOL_File_Rule_addRule_Rule(rule, rule_tmp);

    PIOL_File_Rule_addLong(rule, PIOL_META_COPY, PIOL_TR_SeqNum);
    PIOL_File_Rule_addSEGYFloat(rule, PIOL_META_COPY, PIOL_TR_SeqNum, PIOL_TR_SeqNum);
    PIOL_File_Rule_addShort(rule, PIOL_META_COPY, PIOL_TR_SeqNum);
    PIOL_File_Rule_addIndex(rule, PIOL_META_COPY);
    PIOL_File_Rule_addCopy(rule);
    PIOL_File_Rule_rmRule(rule, PIOL_META_COPY);
    if(PIOL_File_Rule_extent(rule)   == 100) wraptest_ok();
    if(PIOL_File_Rule_memUsage(rule) == 110) wraptest_ok();
    if(PIOL_File_Rule_paramMem(rule) == 120) wraptest_ok();

    PIOL_File_Rule_delete(rule_tmp2);
    PIOL_File_Rule_delete(rule_tmp);


    /*
    ** Testing SEGSz
    */

    if(PIOL_SEGSz_getTextSz() != 3200u) {
        printf(
            "PIOL_SEGSz_getTextSz() expected return value of %u, got %lu\n",
            3200, PIOL_SEGSz_getTextSz()
        );
        return EXIT_FAILURE;
    }
    if(PIOL_SEGSz_getFileSz(200, 210) != 219600u) {
        printf(
            "PIOL_SEGSz_getFileSz(200, 210) expected return value of %u, got %lu\n",
            219600u, PIOL_SEGSz_getFileSz(200, 210)
        );
        return EXIT_FAILURE;
    }
    if(PIOL_SEGSz_getDFSz(220) != 880u) {
        printf(
            "PIOL_SEGSz_getDFSz(220) expected return value of %u, got %lu\n",
            880u, PIOL_SEGSz_getDFSz(220)
        );
        return EXIT_FAILURE;
    }
    if(PIOL_SEGSz_getMDSz() != 240u) {
        printf(
            "PIOL_SEGSz_getMDSz() expected return value of %u, got %lu\n",
            240u, PIOL_SEGSz_getMDSz()
        );
        return EXIT_FAILURE;
    }

    /*
    ** Param calls
    */
    PIOL_File_Param* param     = PIOL_File_Param_new(rule, 300);
    PIOL_File_Param* param_tmp = PIOL_File_Param_new(NULL, 310);
    if(PIOL_File_getPrm_short(320, PIOL_META_COPY, param) == 330) wraptest_ok();

    if(PIOL_File_getPrm_llint(340, PIOL_META_COPY, param) == 350) wraptest_ok();
    if(
        abs(PIOL_File_getPrm_double(360, PIOL_META_COPY, param) - 370.0) < 1e-5
    ) {
        wraptest_ok();
    }

    PIOL_File_setPrm_short(380, PIOL_META_COPY, 390, param);
    PIOL_File_setPrm_llint(400, PIOL_META_COPY, 410, param);
    PIOL_File_setPrm_double(420, PIOL_META_COPY, 430.0, param);

    //void PIOL_File_cpyPrm(
    //    size_t i, const PIOL_File_Param* src,
    //    size_t j, PIOL_File_Param* dst
    //);
    //void PIOL_File_Param_delete(PIOL_File_Param* param);

    ///*
    //** Operations
    //*/
    //void PIOL_File_getMinMax(
    //    PIOL_ExSeis* piol,
    //    size_t offset, size_t sz, PIOL_Meta m1, PIOL_Meta m2,
    //    PIOL_File_Param* param, CoordElem * minmax
    //);

    ///*
    //** Opening and closing files
    //*/
    //PIOL_File_ReadDirect* read_direct = PIOL_File_ReadDirect_new(
    //    piol, "hello"
    //);
    //PIOL_File_WriteDirect* PIOL_File_WriteDirect_new(
    //    PIOL_ExSeis* piol, const char * name
    //);
    //void PIOL_File_ReadDirect_delete(PIOL_File_ReadDirect* readDirect);
    //void PIOL_File_WriteDirect_delete(PIOL_File_WriteDirect* writeDirect);
    //const char * PIOL_File_ReadDirect_readText(
    //    PIOL_File_ReadDirect* readDirect
    //);
    //size_t PIOL_File_ReadDirect_readNs(PIOL_File_ReadDirect* readDirect);
    //size_t PIOL_File_ReadDirect_readNt(PIOL_File_ReadDirect* readDirect);
    //double PIOL_File_ReadDirect_readInc(PIOL_File_ReadDirect* readDirect);
    //void PIOL_File_WriteDirect_writeText(
    //    PIOL_File_WriteDirect* writeDirect, const char * text
    //);
    //void PIOL_File_WriteDirect_writeNs(
    //    PIOL_File_WriteDirect* writeDirect, size_t ns
    //);
    //void PIOL_File_WriteDirect_writeNt(
    //    PIOL_File_WriteDirect* writeDirect, size_t nt
    //);
    //void PIOL_File_WriteDirect_writeInc(
    //    PIOL_File_WriteDirect* writeDirect, geom_t inc
    //);
    //void PIOL_File_WriteDirect_writeParam(
    //    PIOL_File_WriteDirect* writeDirect,
    //    size_t offset, size_t sz, PIOL_File_Param* param
    //);
    //void PIOL_File_ReadDirect_readParam(
    //    PIOL_File_ReadDirect* readDirect, size_t offset, size_t sz,
    //    PIOL_File_Param* param
    //);
    //void PIOL_File_ReadDirect_readTrace(
    //    PIOL_File_ReadDirect* readDirect,
    //    size_t offset, size_t sz, float * trace, PIOL_File_Param* param
    //);
    //void PIOL_File_WriteDirect_writeTrace(
    //    PIOL_File_WriteDirect* writeDirect,
    //    size_t offset, size_t sz, float * trace,
    //    PIOL_File_Param* param
    //);

    //#warning TODO: add readTrace for non-contiguous
    //void PIOL_File_ReadDirect_readTrace(
    //    PIOL_File_ReadDirect* readDirect,
    //    size_t sz, size_t * offset, float * trace, PIOL_File_Param* param
    //);
    //#warning TODO: add writeTrace for non-contiguous
    //void PIOL_File_WriteDirect_writeTrace(
    //    PIOL_File_WriteDirect* writeDirect,
    //    size_t sz, size_t * offset, float * trace, PIOL_File_Param* param
    //);

    //#warning TODO: add writeParam for non-contiguous
    //void PIOL_File_WriteDirect_writeParam(
    //    PIOL_File_WriteDirect* writeDirect,
    //    size_t sz, size_t * offset, PIOL_File_Param* param
    //);
    //#warning TODO: add readParam for non-contiguous
    //void PIOL_File_ReadDirect_readParam(
    //    PIOL_File_ReadDirect* readDirect,
    //    size_t sz, size_t * offset, PIOL_File_Param* param
    //);

    printf("cwraptests: Done!\n");
    return 0;
}
