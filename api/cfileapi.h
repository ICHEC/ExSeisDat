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
#ifdef __cplusplus
using namespace PIOL;
extern "C" {
#endif

typedef struct PIOLWrapper * ExSeisHandle;      //!< A wrapper around a shared PIOL Object
typedef struct ExSeisFileWrapper * ExSeisFile;  //!< A wrapper around a File Layer pointer
typedef struct RuleWrapper * RuleHdl;           //!< A wrapper around a File Layer pointer
typedef struct ParamWrapper * CParam;           //!< A wrapper around a File Layer pointer

/*
 * PIOL calls. Non-file specific
 */
/*! Initialise the PIOL and MPI.
 *  \return A handle to the PIOL.
 */
extern ExSeisHandle initMPIOL();

/*! close the PIOL (deinit MPI)
 * \param[in] piol A handle to the PIOL.
 */
extern void freePIOL(ExSeisHandle piol);

/*! Get the rank of the process (in terms of the PIOL communicator)
 * \param[in] piol A handle to the PIOL.
 */
extern size_t getRank(ExSeisHandle piol);

/*! Get the number of processes (in terms of the PIOL communicator)
 * \param[in] piol A handle to the PIOL.
 */
extern size_t getNumRank(ExSeisHandle piol);

/*! Check if the PIOL has any error conditions
 * \param[in] piol A handle to the PIOL.
 */
extern void isErr(ExSeisHandle piol);

/*!  A barrier. All PIOL processes must call this.
 * \param[in] piol A handle to the PIOL.
 */
extern void barrier(ExSeisHandle piol);

//SEG-Y Size functions
/*! Get the size of the SEG-Y text field (3200 bytes)
 * \return The text size in bytes for SEG-Y
 */
extern size_t getSEGYTextSz(void);

/*! Get the size a SEGY file should be given the number of traces (\c nt) and sample size (\c ns)
 * \param[in] nt The number of traces
 * \param[in] ns The number of samples per trace
 * \return The corresponding file size in bytes for SEG-Y
 */
extern size_t getSEGYFileSz(size_t nt, size_t ns);

/*! Get the size a SEGY trace should be given the sample size (\c ns) and a type of float
 * \param[in] ns The number of samples per trace
 * \return The corresponding trace size in bytes
 */
extern size_t getSEGYTraceLen(size_t ns);

/*! Get the size of a SEGY trace header
 * \return The trace header size in bytes
 */
extern size_t getSEGYParamSz(void);

/*
 * Rule calls
 */
/*! Initialise a Rule structure
 *  \param[in] def Set default rules if true
 *  \return Return a handle for the Rule structure
 */
RuleHdl initRules(bool def);

/*! Free a Rule structure.
 *  \param[in] rule The Rule handle associated with the structure to free
 */
void freeRules(RuleHdl rule);

/*! Add a Rule for longs (int64_t)
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 */
void addLongRule(RuleHdl rule, Meta m, size_t loc);

/*! Add a Rule for shorts (int16_t)
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 */
void addShortRule(RuleHdl rule, Meta m, size_t loc);

/*! Add a Rule for floats
 *  \param[in] rule The Rule handle
 *  \param[in] m The parameter which one is providing a rule for
 *  \param[in] loc The location in the trace header for the rule.
 *  \param[in] scalLoc The location in the trace header for the shared scaler;
 */
void addSEGYFloatRule(RuleHdl rule, Meta m, size_t loc, size_t scalLoc);

/*! remove a rule for a parameter
 *  \param[in] rule The Rule handle associated with the structure to free
 *  \param[in] m The parameter which one is removing a rule for
 */
void rmRule(RuleHdl rule, Meta m);

/*!
 * Param calls
 */
/*! Define a new parameter structure
 *  \param[in] rule The Rule handle associated with the structure
 *  \param[in] sz The number of sets of parameters stored by the structure
 *  \return Return a handle for the parameter structure
 */
CParam initParam(RuleHdl rule, size_t sz);

/*! Define a new parameter structure using default rules
 *  \param[in] sz The number of sets of parameters stored by the structure
 *  \return Return a handle for the parameter structure
 */
CParam initDefParam(size_t sz);

/*! Free the given parameter structure
 *  \param[in] prm The parameter structure
 */
void freeParam(CParam prm);

/*! Get a short parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
int16_t getShortPrm(size_t i, Meta entry, CParam prm);

/*! Get a long parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
int64_t getLongPrm(size_t i, Meta entry, CParam prm);

/*! Get a double parameter which is in a particular set in a parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] prm The parameter structure
 *  \return The associated parameter
 */
double getFloatPrm(size_t i, Meta entry, CParam prm);

/*! Set a short parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void setShortPrm(size_t i, Meta entry, int16_t ret, CParam prm);

/*! Set a long parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void setLongPrm(size_t i, Meta entry, int64_t ret, CParam prm);

/*! Set a double parameter within the parameter structure.
 *  \param[in] i The parameter set number
 *  \param[in] entry The parameter entry
 *  \param[in] ret The value to set the parameter to
 *  \param[in] prm The parameter structure
 */
void setFloatPrm(size_t i, Meta entry, double ret, CParam prm);

/*! Copy parameter within the parameter structure.
 *  \param[in] i The parameter set number of the source
 *  \param[in] src The parameter structure of the source
 *  \param[in] j The parameter set number of the destination
 *  \param[in] dst The parameter structure of the destination
 */
void cpyPrm(size_t i, const CParam src, size_t j, CParam dst);

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
extern void getMinMax(ExSeisHandle piol, size_t offset, size_t sz, Meta m1, Meta m2, const CParam prm, CoordElem * minmax);
/*
 * Opening and closing files
 */
/*! Open a read-only file and return a handle for the file
 * \param[in] piol A handle to the PIOL.
 * \param[in] name The name of the file.
 * \return A handle for the file.
 */
extern ExSeisFile openReadFile(ExSeisHandle piol, const char * name);

/*! Open a write-only file and return a handle for the file
 * \param[in] piol A handle to the PIOL.
 * \param[in] name The name of the file.
 * \return A handle for the file.
 */
extern ExSeisFile openWriteFile(ExSeisHandle piol, const char * name);

/*! \brief Close the file associated with the handle
 *  \param[in] f A handle for the file.
 */
extern void closeFile(ExSeisFile f);

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
extern const char * readText(ExSeisFile f);

/*! \brief Read the number of samples per trace
 *  \param[in] f A handle for the file.
 *  \return The number of samples per trace
 */
extern size_t readNs(ExSeisFile f);

/*! \brief Read the number of traces in the file
 *  \param[in] f A handle for the file.
 *  \return The number of traces
 */
extern size_t readNt(ExSeisFile f);

/*! \brief Read the increment between trace samples
 *  \param[in] f A handle for the file.
 *  \return The increment between trace samples
 */
extern double readInc(ExSeisFile f);

/*! \brief Write the human readable text from the file.
 *  \param[in] f A handle for the file.
 *  \param[in] text The new null-terminated string containing the text (in ASCII format).
 */
extern void writeText(ExSeisFile f, const char * text);

/*! \brief Write the number of samples per trace
 *  \param[in] f A handle for the file.
 *  \param[in] ns The new number of samples per trace.
 */
extern void writeNs(ExSeisFile f, size_t ns);

/*! \brief Write the number of traces in the file
 *  \param[in] f A handle for the file.
 *  \param[in] nt The new number of traces.
 */
extern void writeNt(ExSeisFile f, size_t nt);

/*! \brief Write the increment between trace samples.
 *  \param[in] f A handle for the file.
 *  \param[in] inc The new increment between trace samples.
 */
extern void writeInc(ExSeisFile f, geom_t inc);

/*
 *    Reading data from the trace headers
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
extern void writeParam(ExSeisFile f, size_t offset, size_t sz, const CParam prm);

/*! \brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void readParam(ExSeisFile f, size_t offset, size_t sz, CParam prm);

/*
 *    Reading the traces themselves
 */
/*! \brief Read the traces from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 */
extern void readTrace(ExSeisFile f, size_t offset, size_t sz, float * trace);

/*! \brief Read the trace and trace parameters from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[out] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void readFullTrace(ExSeisFile f, size_t offset, size_t sz, float * trace, CParam prm);

/*! \brief Read the traces from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \warning This function is not thread safe.
 */
extern void writeTrace(ExSeisFile f, size_t offset, size_t sz, float * trace);

/*! \brief Read the traces and trace parameters from offset to offset+sz.
 *  \param[in] f A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 *  \warning This function is not thread safe.
 */
extern void writeFullTrace(ExSeisFile f, size_t offset, size_t sz, float * trace, const CParam prm);

//Lists

/*! \brief Write the traces and trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 */
extern void readListTrace(ExSeisFile f, size_t sz, size_t * offset, float * trace);

/*! \brief Write the traces corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \warning This function is not thread safe.
 */
extern void writeListTrace(ExSeisFile f, size_t sz, size_t * offset, float * trace);

/*! \brief Read the traces and trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[out] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void readFullListTrace(ExSeisFile f, size_t sz, size_t * offset, float * trace, CParam prm);

/*! \brief Write the traces corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(float))
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void writeFullListTrace(ExSeisFile f, size_t sz, size_t * offset, float * trace, const CParam prm);

/*! \brief Write the trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void writeListParam(ExSeisFile f, size_t sz, size_t * offset, const CParam prm);

/*! \brief Read the trace parameters corresponding to the list of trace numbers.
 *  \param[in] f A handle for the file.
 *  \param[in] sz The number of traces to process
 *  \param[in] offset A list of trace numbers.
 *  \param[in] prm An array of the parameter structures (size sizeof(CParam)*sz)
 */
extern void readListParam(ExSeisFile f, size_t sz, size_t * offset, CParam prm);

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
}
#endif
#endif
