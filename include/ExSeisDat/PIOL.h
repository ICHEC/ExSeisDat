////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date Summer 2016
/// @brief
/// @details Primary C11 API header
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_H
#define EXSEISDAT_PIOL_H

#include "ExSeisDat/PIOL/anc/global.hh"
#include "ExSeisDat/PIOL/anc/verbosity.h"
#include "ExSeisDat/PIOL/share/api.hh"
#include "ExSeisDat/PIOL/share/decomp.hh"

#include <mpi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


//
// Forward declare types used for C API handles
//
#ifdef __cplusplus
#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/Rule.hh"
#include "ExSeisDat/PIOL/WriteDirect.hh"
#include "ExSeisDat/PIOL/share/param.hh"

#include <memory>

namespace PIOL {

class ExSeis;

struct Rule;
struct Param;
class ReadDirect;
class WriteDirect;

}  // namespace PIOL

typedef std::shared_ptr<PIOL::ExSeis> PIOL_ExSeis;
typedef std::shared_ptr<PIOL::Rule> PIOL_File_Rule;
typedef PIOL::Param PIOL_File_Param;
typedef PIOL::ReadDirect PIOL_File_ReadDirect;
typedef PIOL::WriteDirect PIOL_File_WriteDirect;

#else  // __cplusplus

// Forward declare opaque structs in C

/// @copydoc PIOL::ExSeis
typedef struct PIOL_ExSeis PIOL_ExSeis;

/// @copydoc PIOL::Rule
typedef struct PIOL_File_Rule PIOL_File_Rule;

/// @copydoc PIOL::Param
typedef struct PIOL_File_Param PIOL_File_Param;

/// @copydoc PIOL::ReadDirect
typedef struct PIOL_File_ReadDirect PIOL_File_ReadDirect;

/// @copydoc PIOL::WriteDirect
typedef struct PIOL_File_WriteDirect PIOL_File_WriteDirect;

#endif  // __cplusplus


#ifdef __cplusplus
// Everything from here on is C API functions needing C linkage.
extern "C" {
#endif  // __cplusplus


/*
 * PIOL calls. Non-file specific
 */
/*! Initialise the PIOL and MPI.
 *  @return A handle to the PIOL.
 */
PIOL_ExSeis* PIOL_ExSeis_new(PIOL_Verbosity);

/*! close the PIOL (deinit MPI)
 *  @param[in,out] piol A handle to the PIOL.
 */
void PIOL_ExSeis_delete(PIOL_ExSeis* piol);

/*! Get the rank of the process (in terms of the PIOL communicator)
 *  @param[in] piol A handle to the PIOL.
 *  @return The rank of the current process.
 */
size_t PIOL_ExSeis_getRank(const PIOL_ExSeis* piol);

/*! Get the number of processes (in terms of the PIOL communicator)
 *  @param[in] piol A handle to the PIOL.
 *  @return The number of ranks the \c piol is running on.
 */
size_t PIOL_ExSeis_getNumRank(const PIOL_ExSeis* piol);

/*! Check if the PIOL has any error conditions
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] msg  The message to print to the log.
 */
void PIOL_ExSeis_isErr(const PIOL_ExSeis* piol, const char* msg);

/*!  A barrier. All PIOL processes must call this.
 *  @param[in] piol A handle to the PIOL.
 */
void PIOL_ExSeis_barrier(const PIOL_ExSeis* piol);

/*! Return the maximum value amongst the processes
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] n The value to take part in the reduction
 *  @return Return the maximum value of (\c n) amongst the processes
 */
size_t PIOL_ExSeis_max(const PIOL_ExSeis* piol, size_t n);

// SEG-Y Size functions
/*! Get the size of the SEG-Y text field (3200 bytes)
 *  @return The text size in bytes for SEG-Y
 */
size_t PIOL_SEGSz_getTextSz(void);

/*! Get the size a SEGY file should be given the number of traces (\c nt) and
 *  sample size (\c ns)
 *  @param[in] nt The number of traces
 *  @param[in] ns The number of samples per trace
 *  @return The corresponding file size in bytes for SEG-Y
 */
size_t PIOL_SEGSz_getFileSz(size_t nt, size_t ns);

/*! Get the size a SEGY trace should be given the sample size (\c ns) and a type
 *  of float
 *  @param[in] ns The number of samples per trace
 *  @return The corresponding trace size in bytes
 */
size_t PIOL_SEGSz_getDFSz(size_t ns);

/*! Get the size of a SEGY trace header
 *  @return The trace header size in bytes
 */
size_t PIOL_SEGSz_getMDSz(void);

/*
 * Rule calls
 */
/*! Initialise a Rule structure
 *  @param[in] def Set default rules if true
 *  @return Return a handle for the Rule structure
 */
PIOL_File_Rule* PIOL_File_Rule_new(bool def);

/*! Initialise a Rule structure from a list of Metas.
 *  @param[in] m List of Meta values (size n).
 *  @param[in] n Number of elements in m
 *  @return Return a handle for the rule structure
 */
PIOL_File_Rule* PIOL_File_Rule_new_from_list(const PIOL_Meta* m, size_t n);

/*! Free a Rule structure.
 *  @param[in,out] rule The Rule handle associated with the structure to free
 */
void PIOL_File_Rule_delete(PIOL_File_Rule* rule);

/*! Add a pre-defined rule.
 *  @param[in,out] rule The Rule handle
 *  @param[in] m The Meta entry.
 *  @return Return true if the rule was added, otherwise false
 */
bool PIOL_File_Rule_addRule_Meta(PIOL_File_Rule* rule, PIOL_Meta m);

/*! Add all rules from the given handle
 *  @param[in,out] rule The Rule handle
 *  @param[in] ruleToCopy The rule handle to copy the rules from.
 *  @return Return true if no errors
 */
bool PIOL_File_Rule_addRule_Rule(
  PIOL_File_Rule* rule, const PIOL_File_Rule* ruleToCopy);

/*! Add a Rule for longs (int64_t)
 *  @param[in,out] rule The Rule handle
 *  @param[in] m The parameter which one is providing a rule for
 *  @param[in] loc The location in the trace header for the rule.
 */
void PIOL_File_Rule_addLong(PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc);

/*! Add a Rule for shorts (int16_t)
 *  @param[in,out] rule The Rule handle
 *  @param[in] m The parameter which one is providing a rule for
 *  @param[in] loc The location in the trace header for the rule.
 */
void PIOL_File_Rule_addShort(PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc);

/*! Add a Rule for floats
 *  @param[in,out] rule The Rule handle
 *  @param[in] m The parameter which one is providing a rule for
 *  @param[in] loc The location in the trace header for the rule.
 *  @param[in] scalLoc The location in the trace header for the shared scaler;
 */
void PIOL_File_Rule_addSEGYFloat(
  PIOL_File_Rule* rule, PIOL_Meta m, PIOL_Tr loc, PIOL_Tr scalLoc);

/*! Add a rule for an index.
 *  @param[in,out] rule The Rule handle
 *  @param[in] m The Meta entry.
 */
void PIOL_File_Rule_addIndex(PIOL_File_Rule* rule, PIOL_Meta m);


/*! Add a rule to buffer the original trace header.
 *  @param[in,out] rule The Rule handle
 */
void PIOL_File_Rule_addCopy(PIOL_File_Rule* rule);

/*! remove a rule for a parameter
 *  @param[in,out] rule The Rule handle associated with the structure to free
 *  @param[in] m The parameter which one is removing a rule for
 */
void PIOL_File_Rule_rmRule(PIOL_File_Rule* rule, PIOL_Meta m);

/*! Return the size of the buffer space required for the metadata items when
 *  converting to SEG-Y.
 *  @param[in,out] rule The Rule handle
 *  @return Return the size.
 */
size_t PIOL_File_Rule_extent(PIOL_File_Rule* rule);

/*! Estimate of the total memory used
 *  @param[in] rule The Rule handle
 *  @return Return estimate in bytes.
 */
size_t PIOL_File_Rule_memUsage(const PIOL_File_Rule* rule);

/*! How much memory will each set of parameters require?
 *  @param[in] rule The Rule handle
 *  @return Amount of memory in bytes.
 */
size_t PIOL_File_Rule_paramMem(const PIOL_File_Rule* rule);

/*!
 * Param calls
 */
/*! Define a new parameter structure
 *  @param[in] rule The Rule handle associated with the structure (use NULL for
 *      default rules)
 *      A copy of this pointer is stored in PIOL_File_Param, but it's safe
 *      to call PIOL_File_Rule_delete(rule) before deleting this, because the
 *      underlying type is a shared_ptr.
 *  @param[in] sz The number of sets of parameters stored by the structure
 *  @return Return a handle for the parameter structure
 */
PIOL_File_Param* PIOL_File_Param_new(PIOL_File_Rule* rule, size_t sz);

/*! Free the given parameter structure
 *  @param[in,out] param The parameter structure
 */
void PIOL_File_Param_delete(PIOL_File_Param* param);

/*! Return the number of sets of trace parameters.
 *  @param[in] param The parameter structure
 *  @return Number of sets
 */
size_t PIOL_File_Param_size(const PIOL_File_Param* param);

/*! Estimate of the total memory used
 *  @param[in] param The parameter structure
 *  @return Return estimate in bytes.
 */
size_t PIOL_File_Param_memUsage(const PIOL_File_Param* param);

/*! Get a short parameter which is in a particular set in a parameter structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *  @return The associated parameter
 */
int16_t PIOL_File_getPrm_short(
  size_t i, PIOL_Meta entry, const PIOL_File_Param* param);

/*! Get a long parameter which is in a particular set in a parameter structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *  @return The associated parameter
 */
PIOL_llint PIOL_File_getPrm_llint(
  size_t i, PIOL_Meta entry, const PIOL_File_Param* param);

/*! Get a double parameter which is in a particular set in a parameter
 *  structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *  @return The associated parameter
 */
PIOL_geom_t PIOL_File_getPrm_double(
  size_t i, PIOL_Meta entry, const PIOL_File_Param* param);

/*! Set a short parameter within the parameter structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void PIOL_File_setPrm_short(
  size_t i, PIOL_Meta entry, int16_t ret, PIOL_File_Param* param);

/*! Set a long parameter within the parameter structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void PIOL_File_setPrm_llint(
  size_t i, PIOL_Meta entry, PIOL_llint ret, PIOL_File_Param* param);

/*! Set a double parameter within the parameter structure.
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void PIOL_File_setPrm_double(
  size_t i, PIOL_Meta entry, PIOL_geom_t ret, PIOL_File_Param* param);

/*! Copy parameter within the parameter structure.
 *  @param[in] i       The parameter set number of the source
 *  @param[in] src     The parameter structure of the source
 *  @param[in] j       The parameter set number of the destination
 *  @param[in,out] dst The parameter structure of the destination
 */
void PIOL_File_cpyPrm(
  size_t i, const PIOL_File_Param* src, size_t j, PIOL_File_Param* dst);

/*
 * Operations
 */
/*! Find the traces with the min and max of a supplied set of coordinates within
 *  a file.
 *  @param[in]  piol    A handle to the PIOL.
 *  @param[in]  offset  The starting trace number.
 *  @param[in]  sz      The number of local traces to process.
 *  @param[in]  m1      The first coordinate item of interest.
 *  @param[in]  m2      The second coordinate item of interest.
 *  @param[in]  param   A handle for the parameter structure.
 *  @param[out] minmax  Set \c minmax to structs corresponding to the minimum x,
 *                      maximum x, minimum y, maximum y in that order.
 */
void PIOL_File_getMinMax(
  const PIOL_ExSeis* piol,
  size_t offset,
  size_t sz,
  PIOL_Meta m1,
  PIOL_Meta m2,
  const PIOL_File_Param* param,
  struct PIOL_CoordElem* minmax);

/*
 * Opening and closing files
 */
/*! Open a read-only file and return a handle for the file
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] name The name of the file.
 *  @return A handle for the file.
 */
PIOL_File_ReadDirect* PIOL_File_ReadDirect_new(
  const PIOL_ExSeis* piol, const char* name);

/*! Open a write-only file and return a handle for the file
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] name The name of the file.
 *  @return A handle for the file.
 */
PIOL_File_WriteDirect* PIOL_File_WriteDirect_new(
  const PIOL_ExSeis* piol, const char* name);

/*! @brief Close the file associated with the handle
 *  @param[in,out] readDirect A handle for the file.
 */
void PIOL_File_ReadDirect_delete(PIOL_File_ReadDirect* readDirect);

/*! @brief Close the file associated with the handle
 *  @param[in,out] writeDirect A handle for the file.
 */
void PIOL_File_WriteDirect_delete(PIOL_File_WriteDirect* writeDirect);

/*
 * Read binary and text headers
 */
/*! @brief Read the human readable text from the file
 *  @details When readText is called the ExSeisPIOL is responsible for
 *  the memory returned. The string should not be dereferenced after the
 *  associated file is closed.
 *  @param[in] readDirect A handle for the file.
 *  @return A string containing the text (in ASCII format)
 */
const char* PIOL_File_ReadDirect_readText(
  const PIOL_File_ReadDirect* readDirect);

/*! @brief Read the number of samples per trace
 *  @param[in] readDirect A handle for the file.
 *  @return The number of samples per trace
 */
size_t PIOL_File_ReadDirect_readNs(const PIOL_File_ReadDirect* readDirect);

/*! @brief Read the number of traces in the file
 *  @param[in] readDirect A handle for the file.
 *  @return The number of traces
 */
size_t PIOL_File_ReadDirect_readNt(const PIOL_File_ReadDirect* readDirect);

/*! @brief Read the increment between trace samples
 *  @param[in] readDirect A handle for the file.
 *  @return The increment between trace samples
 */
double PIOL_File_ReadDirect_readInc(const PIOL_File_ReadDirect* readDirect);

/*! @brief Write the human readable text from the file.
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] text        The new null-terminated string containing the text
 *                         (in ASCII format).
 */
void PIOL_File_WriteDirect_writeText(
  PIOL_File_WriteDirect* writeDirect, const char* text);

/*! @brief Write the number of samples per trace
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] ns          The new number of samples per trace.
 */
void PIOL_File_WriteDirect_writeNs(
  PIOL_File_WriteDirect* writeDirect, size_t ns);

/*! @brief Write the number of traces in the file
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] nt          The new number of traces.
 */
void PIOL_File_WriteDirect_writeNt(
  PIOL_File_WriteDirect* writeDirect, size_t nt);

/*! @brief Write the increment between trace samples.
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] inc         The new increment between trace samples.
 */
void PIOL_File_WriteDirect_writeInc(
  PIOL_File_WriteDirect* writeDirect, PIOL_geom_t inc);

/*
 *    Reading/writing data from the trace headers
 */

/*! @brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] offset      The starting trace number.
 *  @param[in] sz          The number of traces to process.
 *  @param[in] param       A handle for the parameter structure.
 *
 *  @details It is assumed that this operation is not an update. Any previous
 *  contents of the trace header will be overwritten.
 */
void PIOL_File_WriteDirect_writeParam(
  PIOL_File_WriteDirect* writeDirect,
  size_t offset,
  size_t sz,
  const PIOL_File_Param* param);

/*! @brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  @param[in] readDirect A handle for the file.
 *  @param[in] offset     The starting trace number.
 *  @param[in] sz         The number of traces to process.
 *  @param[in] param      A handle for the parameter structure.
 */
void PIOL_File_ReadDirect_readParam(
  const PIOL_File_ReadDirect* readDirect,
  size_t offset,
  size_t sz,
  PIOL_File_Param* param);

/*
 *    Reading the traces themselves
 */
/*! @brief Read the traces and trace parameters from offset to offset+sz.
 *  @param[in]  readDirect A handle for the file.
 *  @param[in]  offset     The starting trace number.
 *  @param[in]  sz         The number of traces to process.
 *  @param[out] trace      A contiguous array of each trace (size sz*ns).
 *  @param[out] param      A handle for the parameter structure.
 */
void PIOL_File_ReadDirect_readTrace(
  const PIOL_File_ReadDirect* readDirect,
  size_t offset,
  size_t sz,
  PIOL_trace_t* trace,
  PIOL_File_Param* param);

/*! @brief Read the traces and trace parameters from offset to offset+sz.
 *  @param[in]  writeDirect A handle for the file.
 *  @param[in]  offset      The starting trace number.
 *  @param[in]  sz          The number of traces to process
 *  @param[out] trace       A contiguous array of each trace (size sz*ns).
 *  @param[in]  param       A handle for the parameter structure.
 *  @warning This function is not thread safe.
 */
void PIOL_File_WriteDirect_writeTrace(
  PIOL_File_WriteDirect* writeDirect,
  size_t offset,
  size_t sz,
  PIOL_trace_t* trace,
  const PIOL_File_Param* param);

// Lists

/*! @brief Read the traces and trace parameters corresponding to the list of
 *         trace numbers.
 *  @param[in]  readDirect A handle for the file.
 *  @param[in]  sz         The number of traces to process.
 *  @param[in]  offset     A list of trace numbers (size sz).
 *  @param[out] trace      A contiguous array of each trace (size sz*ns).
 *  @param[out] param      A handle for the parameter structure
 *                         (pass NULL to ignore).
 */
void PIOL_File_ReadDirect_readTraceNonContiguous(
  PIOL_File_ReadDirect* readDirect,
  size_t sz,
  const size_t* offset,
  PIOL_trace_t* trace,
  PIOL_File_Param* param);

/*! @brief Read the traces and trace parameters corresponding to the
 *         non-monotonic list of trace numbers.
 *  @param[in]  readDirect A handle for the file.
 *  @param[in]  sz         The number of traces to process.
 *  @param[in]  offset     A non-monotonic list of trace numbers (size sz).
 *  @param[out] trace      A contiguous array of each trace (size sz*ns).
 *  @param[out] param      A handle for the parameter structure
 *                         (pass NULL to ignore).
 */
void PIOL_File_ReadDirect_readTraceNonMonotonic(
  PIOL_File_ReadDirect* readDirect,
  size_t sz,
  const size_t* offset,
  PIOL_trace_t* trace,
  PIOL_File_Param* param);

/*! @brief Write the traces corresponding to the list of trace numbers.
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] sz          The number of traces to process.
 *  @param[in] offset      A list of trace numbers (size sz).
 *  @param[in] trace       A contiguous array of each trace (size sz*ns).
 *  @param[in] param       A handle to the parameter structure
 *                         (pass NULL to ignore).
 */
void PIOL_File_WriteDirect_writeTraceNonContiguous(
  PIOL_File_WriteDirect* writeDirect,
  size_t sz,
  const size_t* offset,
  PIOL_trace_t* trace,
  PIOL_File_Param* param);

/*! @brief Write the trace parameters corresponding to the list of trace
 *         numbers.
 *  @param[in] writeDirect A handle for the file.
 *  @param[in] sz          The number of traces to process
 *  @param[in] offset      A list of trace numbers (size sz).
 *  @param[in] param       An handle to the parameter structure.
 */
void PIOL_File_WriteDirect_writeParamNonContiguous(
  PIOL_File_WriteDirect* writeDirect,
  size_t sz,
  const size_t* offset,
  PIOL_File_Param* param);

/*! @brief Read the trace parameters corresponding to the list of trace numbers.
 *  @param[in] readDirect A handle for the file.
 *  @param[in] sz         The number of traces to process
 *  @param[in] offset     A list of trace numbers (size sz).
 *  @param[in] param      An handle to the parameter structure.
 */
void PIOL_File_ReadDirect_readParamNonContiguous(
  PIOL_File_ReadDirect* readDirect,
  size_t sz,
  const size_t* offset,
  PIOL_File_Param* param);

#ifdef DISABLED_OPTIONS
/*
 *     Extended parameters
 */
/*! A list of the different modes of file access.
 */
enum Mode {
    /// The file is opened as read only.
    ReadMode,
    /// The file is opened as write only.
    WriteMode,
    /// The file is opened as Read/Write.
    ReadWriteMode
};

/*! A structure specifying all MPI-IO options
 */
typedef struct {
    /// The file access mode
    enum Mode mode;
    /// The MPI_Info object
    MPI_Info info;
    /// The maximum size to write in an MPI-IO call.
    size_t maxSize;
    /// The MPI communicator which should be used.
    MPI_Comm fcomm;
} MPIIOOptions;

/*! A structure specifying MPI options.
 */
typedef struct {
    /// The MPI communicator
    MPI_Comm comm;
    /// Whether the PIOL should initialise MPI or not
    bool initMPI;
} MPIOptions;

/*! A structure specifying file layer options.
 */
typedef struct {
    /// The increment factor which should be used with inc.
    double incFactor;
} SEGYOptions;

#endif  // DISABLED_OPTIONS

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_H
