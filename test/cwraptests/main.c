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
    printf("Testing ExSeis\n");

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
    printf("Testing Rule\n");

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
    printf("Testing SEGSz\n");

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
    printf("Testing Param\n");

    PIOL_File_Param* param     = PIOL_File_Param_new(rule, 300);
    PIOL_File_Param* param_tmp = PIOL_File_Param_new(NULL, 310);

    if(PIOL_File_Param_size(param)     == 320) wraptest_ok();
    if(PIOL_File_Param_memUsage(param) == 330) wraptest_ok();

    if(PIOL_File_getPrm_short(340, PIOL_META_COPY, param) == 350) wraptest_ok();

    if(PIOL_File_getPrm_llint(360, PIOL_META_COPY, param) == 370) wraptest_ok();
    if(
        fabs(PIOL_File_getPrm_double(380, PIOL_META_COPY, param) - 390.0) < 1e-5
    ) {
        wraptest_ok();
    }

    PIOL_File_setPrm_short(400, PIOL_META_COPY, 410, param);
    PIOL_File_setPrm_llint(420, PIOL_META_COPY, 430, param);
    PIOL_File_setPrm_double(440, PIOL_META_COPY, 450.0, param);

    PIOL_File_cpyPrm(460, param, 470, param_tmp);

    PIOL_File_Param_delete(param_tmp);


    /*
    ** Operations
    */
    printf("Testing Operations\n");

    struct PIOL_CoordElem coord_elem = {.val = 500.0, .num = 510 };
    PIOL_File_getMinMax(
        piol, 520, 530, PIOL_META_COPY, PIOL_META_COPY, param, &coord_elem
    );
    if(
        fabs(coord_elem.val -  540.0) < 1e-5
        &&   coord_elem.num == 550.0
    ) {
        wraptest_ok();
    }

    /*
    ** ReadDirect
    */
    printf("Testing ReadDirect\n");

    PIOL_File_ReadDirect* read_direct = PIOL_File_ReadDirect_new(
        piol, "Test_ReadDirect_filename"
    );

    if(
        strcmp(
            PIOL_File_ReadDirect_readText(read_direct),
            "Test ReadDirect Text"
        ) == 0
    ) {
        wraptest_ok();
    }

    const size_t read_direct_ns = PIOL_File_ReadDirect_readNs(read_direct);
    if(read_direct_ns == 600) wraptest_ok();
    if(PIOL_File_ReadDirect_readNt(read_direct) == 610) wraptest_ok();
    if(fabs(PIOL_File_ReadDirect_readInc(read_direct) - 620.0) < 1e-5) {
        wraptest_ok();
    };

    PIOL_File_ReadDirect_readParam(read_direct, 630, 640, param);

    //#warning TODO: add readParam for non-contiguous
    //void PIOL_File_ReadDirect_readParam(
    //    PIOL_File_ReadDirect* readDirect,
    //    size_t sz, size_t * offset, PIOL_File_Param* param
    //);

    float* read_direct_trace = malloc(read_direct_ns*660*sizeof(float));
    for(size_t i=0; i<read_direct_ns*660; i++) {
        read_direct_trace[i] = 1.0*i;
    }
    PIOL_File_ReadDirect_readTrace(
        read_direct, 650, 660, read_direct_trace, param
    );

    // readTrace sets trace[i] = 2*trace[i];
    int read_direct_trace_ok = 1;
    for(size_t i=0; i<read_direct_ns*600; i++) {
        if(fabs(read_direct_trace[i] - 2.0*i) > 1e-5) {
            read_direct_trace_ok = 0;
        }
    }
    if(read_direct_trace_ok == 1) wraptest_ok();

    free(read_direct_trace);
    read_direct_trace = NULL;

    //#warning TODO: add readTrace for non-contiguous
    //void PIOL_File_ReadDirect_readTrace(
    //    PIOL_File_ReadDirect* readDirect,
    //    size_t sz, size_t * offset, float * trace, PIOL_File_Param* param
    //);

    PIOL_File_ReadDirect_delete(read_direct);


    /*
    ** WriteDirect
    */
    printf("Testing WriteDirect\n");

    PIOL_File_WriteDirect* write_direct = PIOL_File_WriteDirect_new(
        piol, "Test_WriteDirect_filename"
    );

    PIOL_File_WriteDirect_writeText(write_direct, "Test WriteDirect Text");

    const int write_direct_ns = 700;
    PIOL_File_WriteDirect_writeNs(write_direct, write_direct_ns);

    PIOL_File_WriteDirect_writeNt(write_direct, 710);

    PIOL_File_WriteDirect_writeInc(write_direct, 720.0);

    PIOL_File_WriteDirect_writeParam(write_direct, 730, 740, param);

    float* write_direct_trace = malloc(write_direct_ns*760*sizeof(float));
    for(size_t i=0; i<write_direct_ns*760; i++) {
        write_direct_trace[i] = 1.0*i;
    }
    PIOL_File_WriteDirect_writeTrace(
        write_direct, 750, 760, write_direct_trace, param
    );

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

    PIOL_File_WriteDirect_delete(write_direct);



    // /*! Initialise the set.
    //  *  \param[in] piol The PIOL handle
    //  *  \param[in] pattern The file-matching pattern
    //  */
    // PIOL_Set* PIOL_Set_new(PIOL_ExSeis* piol, const char * ptrn);
    // 
    // /*! Free (deinit) the set.
    //  *  \param[in] s The set handle
    //  */
    // void PIOL_Set_delete(PIOL_Set* set);
    // 
    // /*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
    //  *  the collective min and max across all processes (which also must all call this file).
    //  *  \param[in] s The set handle
    //  *  \param[in] m1 The first parameter type
    //  *  \param[in] m2 The second parameter type
    //  *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
    //  *  and their respective trace numbers.
    //  */
    // void PIOL_Set_getMinMax(
    //     PIOL_Set* set, PIOL_Meta m1, PIOL_Meta m2, struct PIOL_CoordElem * minmax
    // );
    // 
    // /*! Sort the set by the specified sort type.
    //  *  \param[in] s The set handle
    //  *  \param[in] type The sort type
    //  */
    // void PIOL_Set_sort(PIOL_Set* set, PIOL_SortType type);
    // 
    // /*! Sort the set using a custom comparison function
    //  *  \param[in] s A handle for the set.
    //  *  \param[in] func The custom comparison function to sort set
    //  */
    // void PIOL_Set_sort_fn(
    //     PIOL_Set* set,
    //     bool (* func)(const PIOL_File_Param* param, size_t i, size_t j)
    // );
    // 
    // /*! Preform tailed taper on a set of traces
    //  * \param[in] s A handle for the set
    //  * \param[in] type The type of taper to be applied to traces.
    //  * \param[in] ntpstr The length of left-tail taper ramp.
    //  * \param[in] ntpend The length of right-tail taper ramp (pass 0 for no ramp).
    //  */
    // void PIOL_Set_taper(
    //     PIOL_Set* set, TaperType type, size_t ntpstr, size_t ntpend
    // );
    // 
    // /*! Output using the given output prefix
    //  *  \param[in] s The set handle
    //  *  \param[in] oname The output prefix
    //  */
    // void PIOL_Set_output(PIOL_Set* set, const char * oname);
    // 
    // /*! Set the text-header of the output
    //  *  \param[in] s The set handle
    //  *  \param[in] outmsg The output message
    //  */
    // void PIOL_Set_text(PIOL_Set* set, const char * outmsg);
    // 
    // /*! Summarise the current status by whatever means the PIOL instrinsically supports
    //  *  \param[in] s The set handle
    //  */
    // void PIOL_Set_summary(PIOL_Set* set);
    // 
    // /*! Add a file to the set based on the name given
    //  *  \param[in] s The set handle
    //  *  \param[in] name The input name
    //  */
    // void PIOL_Set_add(PIOL_Set* set, const char * name);
    // 
    // /*! Scale traces using automatic gain control for visualization
    //  * \param[in] s The set handle
    //  * \param[in] type They type of agc scaling function used
    //  * \param[in] window Length of the agc window
    //  * \param[in] normR Normalization value
    //  */
    // void PIOL_Set_AGC(PIOL_Set* set, AGCType type, size_t window, PIOL_trace_t normR);


    printf("cwraptests: Done!\n");
    return 0;
}
