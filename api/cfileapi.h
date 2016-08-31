#ifndef PIOLCFILEAPI_INCLUDE_GUARD
#define PIOLCFILEAPI_INCLUDE_GUARD
#include <mpi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct PIOLWrapper * ExSeisHandle;      //!< A wrapper around a shared PIOL Object
typedef struct ExSeisFileWrapper * ExSeisFile;  //!< A wrapper around a File Layer pointer

/*
 * Relevant structures containing data
 */
/*! The structure for a coordinate point.
 */
typedef struct
{
    double x;   //!< The first coordinate.
    double y;   //!< The second coordinate.
} ccoord_t;

/*! The structure for a grid point.
 */
typedef struct
{
    int64_t il;     //!< The first grid value
    int64_t xl;     //!< The second grid value.
} cgrid_t;

/*! The options for various coordinate points associated with a trace.
 */
typedef enum
{
    Src,            //!< The source coordinate point.
    Rcv,            //!< The receiver coordinate point.
    CMP             //!< The common-midpoint.
} CCoord;

/*! The options for grid points associated with a trace.
 */
typedef enum
{
    Line            //!< The only current option for a grid. The inline/crossline pair.
} CGrid;

/*! A structure containing all known parameters
 */
typedef struct
{
    ccoord_t src;    //!< The Source coordinate point.
    ccoord_t rcv;    //!< The Receiver coordinate point.
    ccoord_t cmp;    //!< The common midpoint.
    cgrid_t line;    //!< The line grid (il, xl).
    size_t tn;       //!< The trace number.
} TraceParam;

/*
 * PIOL calls. Non-file specific
 */
/*!  Initialise the PIOL and MPI
 *  \return A handle to the PIOL.
 */
extern ExSeisHandle initMPIOL(void);
/*! close the PIOL (deinit MPI)
 * \param[in] piol A handle to the PIOL.
 */
extern void closePIOL(ExSeisHandle piol);

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
 *  \param[in] fh A handle for the file.
 */
extern void closeFile(ExSeisFile fh);

/*
 * Read binary and text headers
 */
/*! \brief Read the human readable text from the file
 *  \details When readText is called the ExSeisPIOL is responsible for
 *  the memory returned. The string should not be dereferenced after the
 *  associated file is closed.
 *  \param[in] fh A handle for the file.
 *  \return A string containing the text (in ASCII format)
 */
extern const char * readText(ExSeisFile fh);

/*! \brief Read the number of samples per trace
 *  \param[in] fh A handle for the file.
 *  \return The number of samples per trace
 */
extern size_t readNs(ExSeisFile fh);

/*! \brief Read the number of traces in the file
 *  \param[in] fh A handle for the file.
 *  \return The number of traces
 */
extern size_t readNt(ExSeisFile fh);

/*! \brief Read the number of increment between trace samples
 *  \param[in] fh A handle for the file.
 *  \return The increment between trace samples
 */
extern double readInc(ExSeisFile fh);

/*! \brief Write the human readable text from the file.
 *  \param[in] fh A handle for the file.
 *  \param[in] text The new null-terminated string containing the text (in ASCII format).
 */
extern void writeText(ExSeisFile fh, const char * text);

/*! \brief Write the number of samples per trace
 *  \param[in] fh A handle for the file.
 *  \param[in] ns The new number of samples per trace.
 */
extern void writeNs(ExSeisFile fh, size_t ns);

/*! \brief Write the number of traces in the file
 *  \param[in] fh A handle for the file.
 *  \param[in] nt The new number of traces.
 */
extern void writeNt(ExSeisFile fh, size_t nt);

/*! \brief Write the number of increment between trace samples.
 *  \param[in] fh A handle for the file.
 *  \param[in] inc The new increment between trace samples.
 */
extern void writeInc(ExSeisFile fh, double inc);

/*
 *    Reading data from the trace headers
 */
/*! \brief Read coordinate pairs from the ith-trace to i+sz.
 *  \param[in] fh A handle for the file.
 *  \param[in] item The coordinate pair of interest.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[out] buf The buffer which is sizeof(coord_t)*sz long.
 */
extern void readCoordPoint(ExSeisFile fh, CCoord item, size_t offset, size_t sz, ccoord_t * buf);

/*! \brief Read grid pairs from the ith-trace to i+sz.
 *  \param[in] fh A handle for the file.
 *  \param[in] item The grid pair of interest.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[out] buf The buffer which is sizeof(grid_t)*sz long.
 */
extern void readGridPoint(ExSeisFile fh, CGrid item, size_t offset, size_t sz, cgrid_t * buf);

/*! \brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  \param[in] fh A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
 *
 *  \details It is assumed that this operation is not an update. Any previous
 *  contents of the trace header will be overwritten.
 */
extern void writeTraceParam(ExSeisFile fh, size_t offset, size_t sz, const TraceParam * prm);

/*! \brief Write the trace parameters from offset to offset+sz to the respective
 *  trace headers.
 *  \param[in] fh A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process.
 *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
 *
 *  \details It is assumed that this operation is not an update. Any previous
 *  contents of the trace header will be overwritten.
 */
extern void readTraceParam(ExSeisFile fh, size_t offset, size_t sz, TraceParam * prm);

/*
 *    Reading the traces themselves
 */
/*! \brief Read the trace's from offset to offset+sz.
 *  \param[in] fh A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
 */
extern void readTrace(ExSeisFile fh, size_t offset, size_t sz, float * trace);

/*! \brief Read the trace's from offset to offset+sz.
 *  \param[in] fh A handle for the file.
 *  \param[in] offset The starting trace number.
 *  \param[in] sz The number of traces to process
 *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
 *  \warning This function is not thread safe.
 */
extern void writeTrace(ExSeisFile fh, size_t offset, size_t sz, float * trace);

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

/*!  Initialise the PIOL and optionally MPI.
 *  \param[in] logLevel The log level
 *  \param[in] mpiOpt The MPI options structure
 *  \return A handle to the PIOL.
 */
extern ExSeisHandle initPIOL(size_t logLevel, MPIOptions * mpiOpt);
/*! Open a file and return a handle for the file
 *  \param[in] piol A handle to the PIOL.
 *  \param[in] name The name of the file.
 *  \param[in] opt The SEG-Y options structure
 *  \param[in] mpiioOpt The MPI-IO options structure
 * \return A handle for the file.
 */
extern ExSeisFile openFile(ExSeisHandle piol, const char * name, SEGYOptions * opt, MPIIOOptions * mpiioOpt);

#ifdef __cplusplus
}
#endif
#endif
