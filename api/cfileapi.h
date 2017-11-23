/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date Summer 2016
 *   \brief
 *   \details Primary C11 API header
 *//*******************************************************************************************/
#ifndef PIOLCFILEAPI_INCLUDE_GUARD
#define PIOLCFILEAPI_INCLUDE_GUARD

#include <mpi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "share/api.hh"
#include "anc/verbosity.h"


//
// Include or forward declare XXXPointer types used for C API handles
//
#ifdef __cplusplus
// Include concrete implementations for XXXPionter types in C++
#include "cppfileapi.hh"
#else
// Forward declare opaque structs in C
typedef struct PIOL_ExSeisPointer PIOL_ExSeisPointer;
typedef struct PIOL_File_RulePointer  PIOL_File_RulePointer;
typedef struct PIOL_File_ParamPointer PIOL_File_ParamPointer;
typedef struct PIOL_File_ReadDirectPointer  PIOL_File_ReadDirectPointer;
typedef struct PIOL_File_WriteDirectPointer PIOL_File_WriteDirectPointer;
#endif


#ifdef __cplusplus
// Need "using" for these enums for the C++ and C prototypes to match here.
// TODO: rename Meta etc. to PIOL_Meta so including this header
//     doesn't add Meta unnamespaced into the global scope.
using PIOL::Meta;
using PIOL::CoordElem;

// Everything from here on is C API functions needing C linkage.
extern "C"
{
#endif


//!< A wrapper around a PIOL::ExSeis object
typedef PIOL_ExSeisPointer* PIOL_ExSeisHandle;

//!< A wrapper around a PIOL::File::Rule object
typedef PIOL_File_RulePointer* PIOL_File_RuleHandle;

//!< A wrapper around a PIOL::File::Param object
typedef PIOL_File_ParamPointer* PIOL_File_ParamHandle;

//!< A wrapper around a PIOL::File::ReadDirect object
typedef PIOL_File_ReadDirectPointer* PIOL_File_ReadDirectHandle;

//!< A wrapper around a PIOL::File::WriteDirect object
typedef PIOL_File_WriteDirectPointer* PIOL_File_WriteDirectHandle;


/*
 * PIOL calls. Non-file specific
 */
/*! Initialise the PIOL and MPI.
 *  \return A handle to the PIOL.
 */
PIOL_ExSeisHandle PIOL_ExSeis_new(PIOL_Verbosity);

/*! close the PIOL (deinit MPI)
 * \param[in] piol A handle to the PIOL.
 */
void PIOL_ExSeis_delete(PIOL_ExSeisHandle piol);

/*! Get the rank of the process (in terms of the PIOL communicator)
 * \param[in] piol A handle to the PIOL.
 */
size_t PIOL_ExSeis_getRank(PIOL_ExSeisHandle piol);

/*! Get the number of processes (in terms of the PIOL communicator)
 * \param[in] piol A handle to the PIOL.
 */
size_t PIOL_ExSeis_getNumRank(PIOL_ExSeisHandle piol);

/*! Check if the PIOL has any error conditions
 * \param[in] piol A handle to the PIOL.
 */
void PIOL_ExSeis_isErr(PIOL_ExSeisHandle piol, const char* msg);

/*!  A barrier. All PIOL processes must call this.
 * \param[in] piol A handle to the PIOL.
 */
void PIOL_ExSeis_barrier(PIOL_ExSeisHandle piol);

/*! Return the maximum value amongst the processes
 * \param[in] piol A handle to the PIOL.
 * \param[in] n The value to take part in the reduction
 * \return Return the maximum value of (\c n) amongst the processes
 */
size_t PIOL_ExSeis_max(PIOL_ExSeisHandle piol, size_t n);

//SEG-Y Size functions
/*! Get the size of the SEG-Y text field (3200 bytes)
 * \return The text size in bytes for SEG-Y
 */
size_t PIOL_SEGSz_getTextSz(void);

/*! Get the size a SEGY file should be given the number of traces (\c nt) and sample size (\c ns)
 * \param[in] nt The number of traces
 * \param[in] ns The number of samples per trace
 * \return The corresponding file size in bytes for SEG-Y
 */
size_t PIOL_SEGSz_getFileSz(size_t nt, size_t ns);

/*! Get the size a SEGY trace should be given the sample size (\c ns) and a type of float
 * \param[in] ns The number of samples per trace
 * \return The corresponding trace size in bytes
 */
size_t PIOL_SEGSz_getDFSz(size_t ns);

/*! Get the size of a SEGY trace header
 * \return The trace header size in bytes
 */
size_t PIOL_SEGSz_getMDSz(void);

/*
 * Rule calls
 */
/*! Initialise a Rule structure
 *  \param[in] def Set default rules if true
 *  \return Return a handle for the Rule structure
 */
PIOL_File_RuleHandle PIOL_File_Rule_new(bool def);

/*! Free a Rule structure.
 *  \param[in] rule The Rule handle associated with the structure to free
 */
void PIOL_File_Rule_delete(PIOL_File_RuleHandle rule);

/*! Add a Rule for longs (int64_t)
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 */
void PIOL_File_Rule_addLong(PIOL_File_RuleHandle rule, Meta m, size_t loc);

/*! Add a Rule for shorts (int16_t)
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 */
void PIOL_File_Rule_addShort(PIOL_File_RuleHandle rule, Meta m, size_t loc);

/*! Add a Rule for floats
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 *  \param[in] scalLoc The location in the trace header for the shared scaler;
 */
void PIOL_File_Rule_addSEGYFloat(
    PIOL_File_RuleHandle rule, Meta m, size_t loc, size_t scalLoc
);

/*! remove a rule for a parameter
 *  \param[in] rule The Rule handle associated with the structure to free
 *  \param[in] m The parameter which one is removing a rule for
 */
void PIOL_File_Rule_rmRule(PIOL_File_RuleHandle rule, Meta m);

/*!
 * Param calls
 */
/*! Define a new parameter structure
 *  \param[in] rule The Rule handle associated with the structure (use NULL for
 *      default rules)
 *  \param[in] sz The number of sets of parameters stored by the structure
 *  \return Return a handle for the parameter structure
 */
PIOL_File_ParamHandle PIOL_File_Param_new(PIOL_File_RuleHandle rule, size_t sz);

/*! Free the given parameter structure
 *  \param[in] prm The parameter structure
 */
void PIOL_File_Param_delete(PIOL_File_ParamHandle param);

/*! Get a short parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
int16_t PIOL_File_getPrm_short(
    size_t i, Meta entry, PIOL_File_ParamHandle param
);

/*! Get a long parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
int64_t PIOL_File_getPrm_llint(
    size_t i, Meta entry, PIOL_File_ParamHandle param
);

/*! Get a double parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
double PIOL_File_getPrm_double(
    size_t i, Meta entry, PIOL_File_ParamHandle param
);

/*! Set a short parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void PIOL_File_setPrm_short(
    size_t i, Meta entry, int16_t ret, PIOL_File_ParamHandle param
);

/*! Set a long parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void PIOL_File_setPrm_llint(
    size_t i, Meta entry, int64_t ret, PIOL_File_ParamHandle param
);

/*! Set a double parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void PIOL_File_setPrm_double(
    size_t i, Meta entry, double ret, PIOL_File_ParamHandle param
);

/*! Copy parameter within the parameter structure.
 *  \param[in] i The parameter set number of the source
 *  \param[in] src The parameter structure of the source
 *  \param[in] j The parameter set number of the destination
 *  \param[in] dst The parameter structure of the destination
 */
void PIOL_File_cpyPrm(
    size_t i, PIOL_File_ParamHandle src,
    size_t j, PIOL_File_ParamHandle dst
);

/*
 * Operations
 */
/*! Find the traces with the min and max of a supplied set of coordinates within a file.
 *  \param[in] piol A handle to the PIOL.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of local traces to process.
 *  \param[in] coord The array of local coordinates which one wants to process
 *  \param[out] minmax Set \c minmax to structs corresponding to the minimum x, maximum x, minimum y, maximum y in that order.
 */
void PIOL_File_getMinMax(
    PIOL_ExSeisHandle piol,
    size_t offset, size_t sz, Meta m1, Meta m2,
    PIOL_File_ParamHandle param, CoordElem * minmax
);

/*
 * Opening and closing files
 */
/*! Open a read-only file and return a handle for the file
 * \param[in] piol A handle to the PIOL.
 * \param[in] name The name of the file.
 * \return A handle for the file.
 */
PIOL_File_ReadDirectHandle PIOL_File_ReadDirect_new(
    PIOL_ExSeisHandle piol, const char * name
);

/*! Open a write-only file and return a handle for the file
 * \param[in] piol A handle to the PIOL.
 * \param[in] name The name of the file.
 * \return A handle for the file.
 */
PIOL_File_WriteDirectHandle PIOL_File_WriteDirect_new(
    PIOL_ExSeisHandle piol, const char * name
);

/*! \brief Close the file associated with the handle
 *  \param[in] f A handle for the file.
 */
void PIOL_File_ReadDirect_delete(PIOL_File_ReadDirectHandle readDirect);

/*! \brief Close the file associated with the handle
 *  \param[in] f A handle for the file.
 */
void PIOL_File_WriteDirect_delete(PIOL_File_WriteDirectHandle writeDirect);

/*
 * Read binary and text headers
 */
/*! \brief Read the human readable text from the file
 *  \details When readText is called the ExSeisPIOL is responsible for
 *  the memory returned. The string should not be dereferenced after the
 *  associated file is closed.
 *  \param[in] f A handle for the file.
 *  \return A string containing the text (in ASCII format)
 */
const char * PIOL_File_ReadDirect_readText(
    PIOL_File_ReadDirectHandle readDirect
);

/*! \brief Read the number of samples per trace
 *  \param[in] f A handle for the file.
 *  \return The number of samples per trace
 */
size_t PIOL_File_ReadDirect_readNs(PIOL_File_ReadDirectHandle readDirect);

/*! \brief Read the number of traces in the file
 *  \param[in] f A handle for the file.
 *  \return The number of traces
 */
size_t PIOL_File_ReadDirect_readNt(PIOL_File_ReadDirectHandle readDirect);

/*! \brief Read the increment between trace samples
 *  \param[in] f A handle for the file.
 *  \return The increment between trace samples
 */
double PIOL_File_ReadDirect_readInc(PIOL_File_ReadDirectHandle readDirect);

/*! \brief Write the human readable text from the file.
 *  \param[in] f A handle for the file.
 *  \param[in] text The new null-terminated string containing the text (in ASCII format).
 */
void PIOL_File_WriteDirect_writeText(
    PIOL_File_WriteDirectHandle writeDirect, const char * text
);

/*! \brief Write the number of samples per trace
 *  \param[in] f A handle for the file.
 *  \param[in] ns The new number of samples per trace.
 */
void PIOL_File_WriteDirect_writeNs(
    PIOL_File_WriteDirectHandle writeDirect, size_t ns
);

/*! \brief Write the number of traces in the file
 *  \param[in] f A handle for the file.
 *  \param[in] nt The new number of traces.
 */
void PIOL_File_WriteDirect_writeNt(
    PIOL_File_WriteDirectHandle writeDirect, size_t nt
);

/*! \brief Write the increment between trace samples.
 *  \param[in] f A handle for the file.
 *  \param[in] inc The new increment between trace samples.
 */
void PIOL_File_WriteDirect_writeInc(
    PIOL_File_WriteDirectHandle writeDirect, geom_t inc
);

/*
 *    Reading/writing data from the trace headers
 */

/*! \brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 *
 *  \details It is assumed that this operation is not an update. Any previous
 *  contents of the trace header will be overwritten.
 */
void PIOL_File_WriteDirect_writeParam(
    PIOL_File_WriteDirectHandle writeDirect,
    size_t offset, size_t sz, PIOL_File_ParamHandle param
);

/*! \brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
void PIOL_File_ReadDirect_readParam(
    PIOL_File_ReadDirectHandle readDirect, size_t offset, size_t sz,
    PIOL_File_ParamHandle param
);

/*
 *    Reading the traces themselves
 */
/*! \brief Read the traces and trace parameters from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[out] prm An array of the parameter structures (size
 *      sizeof(CParam)*sz) (pass NULL to ignore)
 */
void PIOL_File_ReadDirect_readTrace(
    PIOL_File_ReadDirectHandle readDirect,
    size_t offset, size_t sz, float * trace, PIOL_File_ParamHandle param
);

/*! \brief Read the traces and trace parameters from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 *      (pass NULL to ignore)
 *  \warning This function is not thread safe.
 */
void PIOL_File_WriteDirect_writeTrace(
    PIOL_File_WriteDirectHandle writeDirect,
    size_t offset, size_t sz, float * trace,
    PIOL_File_ParamHandle param
);

//Lists

/*! \brief Read the traces and trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[out] prm An array of the parameter structures (size
 *  sizeof(CParam)*sz) (pass NULL to ignore)
 */
#warning TODO: add readTrace for non-contiguous
//void PIOL_File_ReadDirect_readTrace(
//    PIOL_File_ReadDirectHandle readDirect,
//    size_t sz, size_t * offset, float * trace, PIOL_File_ParamHandle param
//);

/*! \brief Write the traces corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 *      (pass NULL to ignore)
 */
#warning TODO: add writeTrace for non-contiguous
//void PIOL_File_WriteDirect_writeTrace(
//    PIOL_File_WriteDirectHandle writeDirect,
//    size_t sz, size_t * offset, float * trace, PIOL_File_ParamHandle param
//);

/*! \brief Write the trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
#warning TODO: add writeParam for non-contiguous
//void PIOL_File_WriteDirect_writeParam(
//    PIOL_File_WriteDirectHandle writeDirect,
//    size_t sz, size_t * offset, PIOL_File_ParamHandle param
//);

/*! \brief Read the trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
#warning TODO: add readParam for non-contiguous
//void PIOL_File_ReadDirect_readParam(
//    PIOL_File_ReadDirectHandle readDirect,
//    size_t sz, size_t * offset, PIOL_File_ParamHandle param
//);

#ifdef DISABLED_OPTIONS
/*
 *     Extended parameters
 */
/*! A list of the different modes of file access.
 */
enum Mode
{
    ReadMode,       //!< The file is opened as read only.
    WriteMode,      //!< The file is opened as write only.
    ReadWriteMode   //!< The file is opened as Read/Write.
};

/*! A structure specifying all MPI-IO options
 */
typedef struct
{
    enum Mode mode; //!< The file access mode
    MPI_Info info;  //!< The MPI_Info object
    size_t maxSize; //!< The maximum size to write in an MPI-IO call.
    MPI_Comm fcomm; //!< The MPI communicator which should be used.
} MPIIOOptions;

/*! A structure specifying MPI options.
 */
typedef struct
{
    MPI_Comm comm;  //!< The MPI communicator
    bool initMPI;   //!< Whether the PIOL should initialise MPI or not
} MPIOptions;

/*! A structure specifying file layer options.
 */
typedef struct
{
    double incFactor;   //!< The increment factor which should be used with inc.
} SEGYOptions;
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
