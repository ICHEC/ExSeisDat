////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Primary C11 API header
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_H
#define EXSEISDAT_PIOL_H

#include "exseisdat/piol/metadata/Trace_metadata_key.h"
#include "exseisdat/piol/operations/minmax.h"
#include "exseisdat/piol/segy/Trace_header_offsets.h"
#include "exseisdat/utils/decomposition/block_decomposition.h"
#include "exseisdat/utils/logging/Verbosity.h"
#include "exseisdat/utils/types/typedefs.h"

#include <mpi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


//
// Forward declare types used for C API handles
//
#ifdef __cplusplus
#include "exseisdat/piol.hh"

#include <memory>

namespace exseis {
namespace piol {

/// @name C API
///
/// @{

/// @copydoc exseis::piol::configuration::ExSeis
typedef std::shared_ptr<ExSeis> piol_exseis;

/// @copydoc exseis::piol::metadata::rules::Rule
typedef Rule piol_file_rule;

/// @copydoc exseis::piol::metadata::Trace_metadata
typedef Trace_metadata piol_file_trace_metadata;

/// @copydoc exseis::piol::file::Input_file
typedef Input_file piol_file_read_interface;

/// @copydoc exseis::piol::file::Output_file
typedef Output_file piol_file_write_interface;

#else  // __cplusplus

// Forward declare opaque structs in C

/// @copydoc exseis::piol::configuration::ExSeis
typedef struct piol_exseis piol_exseis;

/// @copydoc exseis::piol::metadata::rules::Rule
typedef struct piol_file_rule piol_file_rule;

/// @copydoc piol::metadata::Trace_metadata
typedef struct piol_file_trace_metadata piol_file_trace_metadata;

/// @copydoc piol::file::input_file::Input_file
typedef struct piol_file_read_interface piol_file_read_interface;

/// @copydoc piol::file::output_file::Output_file
typedef struct piol_file_write_interface piol_file_write_interface;

#endif  // __cplusplus


#ifdef __cplusplus
// namespace exseis {
// namespace piol {

using namespace exseis::utils::logging;

// Everything from here on is C API functions needing C linkage.
extern "C" {
#endif  // __cplusplus


/*!
 * @name PIOL calls. Non-file specific
 *
 * @{
 */

/*! @brief Initialise the PIOL and MPI.
 *
 *  @param[in] verbosity The level of verbosity to log.
 *
 *  @return A handle to the PIOL.
 *
 *  @pre `verbosity` is in the enum `exseis::utils::Verbosity`
 */
piol_exseis* piol_exseis_new(exseis_Verbosity verbosity);

/*! @brief close the PIOL (deinit MPI)
 *
 *  @param[in,out] piol A handle to the PIOL.
 */
void piol_exseis_delete(piol_exseis* piol);

/*! @brief Get the rank of the process (in terms of the PIOL communicator)
 *
 *  @param[in] piol A handle to the PIOL.
 *
 *  @return The rank of the current process.
 */
size_t piol_exseis_get_rank(const piol_exseis* piol);

/*! @brief Get the number of processes (in terms of the PIOL communicator)
 *
 *  @param[in] piol A handle to the PIOL.
 *
 *  @return The number of ranks the \c piol is running on.
 */
size_t piol_exseis_get_num_rank(const piol_exseis* piol);

/*! @brief Check if the PIOL has any error conditions
 *
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] msg  The message to print to the log.
 */
void piol_exseis_assert_ok(const piol_exseis* piol, const char* msg);

/*! @brief A barrier. All PIOL processes must call this.
 *
 *  @param[in] piol A handle to the PIOL.
 */
void piol_exseis_barrier(const piol_exseis* piol);

/*! @brief Return the maximum value amongst the processes
 *
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] n    The value to take part in the reduction
 *
 *  @return Return the maximum value of (\c n) amongst the processes
 */
size_t piol_exseis_max(const piol_exseis* piol, size_t n);

/*! @} PIOL calls. Non-file specific*/

/*! @name SEG-Y Size functions
 *
 *  @{
 */

/*! @brief Get the size of the SEG-Y text field (3200 bytes)
 *
 *  @return The text size in bytes for SEG-Y
 */
size_t piol_segy_segy_text_header_size(void);

/*! @brief Get the size a SEGY file should be given the number of traces (\c nt)
 *         and sample size (\c ns)
 *
 *  @param[in] nt The number of traces
 *  @param[in] ns The number of samples per trace
 *
 *  @return The corresponding file size in bytes for SEG-Y
 */
size_t piol_segy_get_file_size(size_t nt, size_t ns);

/*! @brief Get the size a SEGY trace should be given the sample size (\c ns) and
 *         a type of float
 *
 *  @param[in] ns The number of samples per trace
 *
 *  @return The corresponding trace size in bytes
 */
size_t piol_segy_segy_trace_data_size(size_t ns);

/*! @brief Get the size of a SEGY trace header
 *
 *  @return The trace header size in bytes
 */
size_t piol_segy_segy_trace_header_size(void);


/*! @} SEG-Y Size functions */

/*! @name Rule calls
 *
 *  @{
 */

/*! @brief Initialise a Rule structure
 *
 *  @param[in] def Set default rules if true
 *
 *  @return Return a handle for the Rule structure
 */
piol_file_rule* piol_file_rule_new(bool def);

/*! @brief Initialise a Rule structure from a list of Metas.
 *
 *  @param[in] m List of Meta values (size n).
 *  @param[in] n Number of elements in m
 *
 *  @return Return a handle for the rule structure
 */
piol_file_rule* piol_file_rule_new_from_list(
    const exseis_Trace_metadata_key* m, size_t n);

/*! @brief Free a Rule structure.
 *
 *  @param[in,out] rule The Rule handle associated with the structure to free
 */
void piol_file_rule_delete(piol_file_rule* rule);

/*! @brief Add a pre-defined rule.
 *
 *  @param[in,out]  rule    The Rule handle
 *  @param[in]      m       The Meta entry.
 *
 *  @return Return true if the rule was added, otherwise false
 */
bool piol_file_rule_add_rule_meta(
    piol_file_rule* rule, exseis_Trace_metadata_key m);

/*! @brief Add all rules from the given handle
 *
 *  @param[in,out]  rule            The Rule handle
 *  @param[in]      rule_to_copy    The rule handle to copy the rules from.
 *
 *  @return Return true if no errors
 */
bool piol_file_rule_add_rule_rule(
    piol_file_rule* rule, const piol_file_rule* rule_to_copy);

/*! @brief Add a Rule for longs (int64_t)
 *
 *  @param[in,out]  rule    The Rule handle
 *  @param[in]      m       The parameter which one is providing a rule for
 *  @param[in]      loc     The location in the trace header for the rule.
 */
void piol_file_rule_add_long(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc);

/*! @brief Add a Rule for shorts (int16_t)
 *
 *  @param[in,out]  rule    The Rule handle
 *  @param[in]      m       The parameter which one is providing a rule for
 *  @param[in]      loc     The location in the trace header for the rule.
 */
void piol_file_rule_add_short(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc);

/*! @brief Add a Rule for floats
 *
 *  @param[in,out]  rule            The Rule handle
 *  @param[in]      m               The parameter which one is providing a
 *                                  rule for
 *  @param[in]      loc             The location in the trace header for the
 *                                  rule.
 *  @param[in]      scalar_location The location in the trace header for the
 *                                  shared scaler;
 */
void piol_file_rule_add_segy_float(
    piol_file_rule* rule,
    exseis_Trace_metadata_key m,
    exseis_Trace_header_offsets loc,
    exseis_Trace_header_offsets scalar_location);

/*! @brief Add a rule for an index.
 *
 *  @param[in,out]  rule The Rule handle
 *  @param[in]      m    The Meta entry.
 */
void piol_file_rule_add_index(
    piol_file_rule* rule, exseis_Trace_metadata_key m);


/*! @brief Add a rule to buffer the original trace header.
 *
 *  @param[in,out] rule The Rule handle
 */
void piol_file_rule_add_copy(piol_file_rule* rule);

/*! @brief Remove a rule for a parameter
 *
 *  @param[in,out]  rule   The Rule handle associated with the structure to free
 *  @param[in]      m      The parameter which one is removing a rule for
 */
void piol_file_rule_rm_rule(piol_file_rule* rule, exseis_Trace_metadata_key m);

/*! @brief Return the size of the buffer space required for the metadata items
 *         when converting to SEG-Y.
 *
 *  @param[in,out] rule The Rule handle
 *
 *  @return Return the size.
 */
size_t piol_file_rule_extent(piol_file_rule* rule);

/*! @brief Estimate of the total memory used
 *
 *  @param[in] rule The Rule handle
 *
 *  @return Return estimate in bytes.
 */
size_t piol_file_rule_memory_usage(const piol_file_rule* rule);

/*! @brief How much memory will each set of parameters require?
 *
 *  @param[in] rule The Rule handle
 *
 *  @return Amount of memory in bytes.
 */
size_t piol_file_rule_memory_usage_per_header(const piol_file_rule* rule);

/*! @} Rule calls */

/*! @name Trace_metadata calls
 *
 *  @{
 */

/*! @brief Define a new parameter structure
 *
 *  @param[in] rule The Rule handle associated with the structure (use NULL for
 *                  default rules)
 *                  A copy of this pointer is stored in
 *                  piol_file_trace_metadata, but it's safe to call
 *                  piol_file_rule_delete(rule) before deleting this, because
 *                  the underlying type is a shared_ptr.
 *  @param[in] sz   The number of sets of parameters stored by the structure
 *
 *  @return Return a handle for the parameter structure
 */
piol_file_trace_metadata* piol_file_trace_metadata_new(
    piol_file_rule* rule, size_t sz);

/*! @brief Free the given parameter structure
 *
 *  @param[in,out] param The parameter structure
 */
void piol_file_trace_metadata_delete(piol_file_trace_metadata* param);

/*! @brief Return the number of sets of trace parameters.
 *
 *  @param[in] param The parameter structure
 *
 *  @return Number of sets
 */
size_t piol_file_trace_metadata_size(const piol_file_trace_metadata* param);

/*! @brief Estimate of the total memory used
 *
 *  @param[in] param The parameter structure
 *
 *  @return Return estimate in bytes.
 */
size_t piol_file_trace_metadata_memory_usage(
    const piol_file_trace_metadata* param);

/*! @brief Get an index parameter which is in a particular set in a parameter
 *         structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *
 *  @return The associated parameter
 */
size_t piol_file_get_prm_index(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param);

/*! @brief Get a long parameter which is in a particular set in a parameter
 *         structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *
 *  @return The associated parameter
 */
exseis_Integer piol_file_get_prm_integer(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param);

/*! @brief Get a double parameter which is in a particular set in a parameter
 *         structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] param The parameter structure
 *
 *  @return The associated parameter
 */
exseis_Floating_point piol_file_get_prm_double(
    size_t i,
    exseis_Trace_metadata_key entry,
    const piol_file_trace_metadata* param);

/*! @brief Set a index parameter within the parameter structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void piol_file_set_prm_index(
    size_t i,
    exseis_Trace_metadata_key entry,
    size_t ret,
    piol_file_trace_metadata* param);

/*! @brief Set a long parameter within the parameter structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void piol_file_set_prm_integer(
    size_t i,
    exseis_Trace_metadata_key entry,
    exseis_Integer ret,
    piol_file_trace_metadata* param);

/*! @brief Set a double parameter within the parameter structure.
 *
 *  @param[in] i     The parameter set number
 *  @param[in] entry The parameter entry
 *  @param[in] ret   The value to set the parameter to
 *  @param[in] param The parameter structure
 */
void piol_file_set_prm_double(
    size_t i,
    exseis_Trace_metadata_key entry,
    exseis_Floating_point ret,
    piol_file_trace_metadata* param);

/*! @brief Copy parameter within the parameter structure.
 *
 *  @param[in] i       The parameter set number of the source
 *  @param[in] src     The parameter structure of the source
 *  @param[in] j       The parameter set number of the destination
 *  @param[in,out] dst The parameter structure of the destination
 */
void piol_file_cpy_prm(
    size_t i,
    const piol_file_trace_metadata* src,
    size_t j,
    piol_file_trace_metadata* dst);

/*! @} Trace_metadata calls */

/*! @name Operations
 *
 *  @{
 */


/*! @brief Find the traces with the min and max of a supplied set of coordinates
 *         within a file.
 *
 *  @param[in]  piol    A handle to the PIOL.
 *  @param[in]  offset  The starting trace number.
 *  @param[in]  sz      The number of local traces to process.
 *  @param[in]  m1      The first coordinate item of interest.
 *  @param[in]  m2      The second coordinate item of interest.
 *  @param[in]  param   A handle for the parameter structure.
 *  @param[out] minmax  Set \c minmax to structs corresponding to the minimum x,
 *                      maximum x, minimum y, maximum y in that order.
 */
void piol_file_get_min_max(
    const piol_exseis* piol,
    size_t offset,
    size_t sz,
    exseis_Trace_metadata_key m1,
    exseis_Trace_metadata_key m2,
    const piol_file_trace_metadata* param,
    struct PIOL_CoordElem* minmax);

/*! @} Operations */

/*! @name Opening and closing files
 *
 *  @{
 */

/*! @brief Open a read-only file and return a handle for the file
 *
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] name The name of the file.
 *
 *  @return A handle for the file.
 */
piol_file_read_interface* piol_file_read_segy_new(
    const piol_exseis* piol, const char* name);

/*! @brief Open a write-only file and return a handle for the file
 *
 *  @param[in] piol A handle to the PIOL.
 *  @param[in] name The name of the file.
 *
 *  @return A handle for the file.
 */
piol_file_write_interface* piol_file_write_segy_new(
    const piol_exseis* piol, const char* name);

/*! @brief Close the file associated with the handle
 *
 *  @param[in,out] read_direct A handle for the file.
 */
void piol_file_read_interface_delete(piol_file_read_interface* read_direct);

/*! @brief Close the file associated with the handle
 *
 *  @param[in,out] write_direct A handle for the file.
 */
void piol_file_write_interface_delete(piol_file_write_interface* write_direct);

/*! @} Opening and closing files */

/*! @name Read binary and text headers
 *
 *  @{
 */

/*! @brief Read the human readable text from the file
 *
 *  @details When read_text is called the ExSeisPIOL is responsible for
 *           the memory returned. The string should not be dereferenced after
 *           the associated file is closed.
 *
 *  @param[in] read_direct A handle for the file.
 *
 *  @return A string containing the text (in ASCII format)
 */
const char* piol_file_read_interface_read_text(
    const piol_file_read_interface* read_direct);

/*! @brief Read the number of samples per trace
 *
 *  @param[in] read_direct A handle for the file.
 *
 *  @return The number of samples per trace
 */
size_t piol_file_read_interface_read_ns(
    const piol_file_read_interface* read_direct);

/*! @brief Read the number of traces in the file
 *
 *  @param[in] read_direct A handle for the file.
 *
 *  @return The number of traces
 */
size_t piol_file_read_interface_read_nt(
    const piol_file_read_interface* read_direct);

/*! @brief Read the increment between trace samples
 *
 *  @param[in] read_direct A handle for the file.
 *
 *  @return The increment between trace samples
 */
double piol_file_read_interface_read_sample_interval(
    const piol_file_read_interface* read_direct);

/*! @brief Write the human readable text from the file.
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] text        The new null-terminated string containing the text
 *                         (in ASCII format).
 */
void piol_file_write_interface_write_text(
    piol_file_write_interface* write_direct, const char* text);

/*! @brief Write the number of samples per trace
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] ns          The new number of samples per trace.
 */
void piol_file_write_interface_write_ns(
    piol_file_write_interface* write_direct, size_t ns);

/*! @brief Write the number of traces in the file
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] nt          The new number of traces.
 */
void piol_file_write_interface_write_nt(
    piol_file_write_interface* write_direct, size_t nt);

/*! @brief Write the increment between trace samples.
 *
 *  @param[in] write_direct     A handle for the file.
 *  @param[in] sample_interval The new interval between trace samples.
 */
void piol_file_write_interface_write_sample_interval(
    piol_file_write_interface* write_direct,
    exseis_Floating_point sample_interval);

/*! @} Read binary and text headers */

/*! @name Reading/writing data from the trace headers
 *
 *  @{
 */

/*! @brief Write the trace parameters from offset to offset+sz to the respective
 *         trace headers.
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] offset      The starting trace number.
 *  @param[in] sz          The number of traces to process.
 *  @param[in] param       A handle for the parameter structure.
 *
 *  @details It is assumed that this operation is not an update. Any previous
 *           contents of the trace header will be overwritten.
 */
void piol_file_write_interface_write_param(
    piol_file_write_interface* write_direct,
    size_t offset,
    size_t sz,
    const piol_file_trace_metadata* param);

/*! @brief Write the trace parameters from offset to offset+sz to the respective
 *         trace headers.
 *
 *  @param[in] read_direct A handle for the file.
 *  @param[in] offset     The starting trace number.
 *  @param[in] sz         The number of traces to process.
 *  @param[in] param      A handle for the parameter structure.
 */
void piol_file_read_interface_read_param(
    const piol_file_read_interface* read_direct,
    size_t offset,
    size_t sz,
    piol_file_trace_metadata* param);

/*! @} Reading/writing data from the trace headers */

/*! @name Reading the traces themselves
 *
 *  @{
 */

/*! @brief Read the traces and trace parameters from offset to offset+sz.
 *
 *  @param[in]  read_direct A handle for the file.
 *  @param[in]  offset     The starting trace number.
 *  @param[in]  sz         The number of traces to process.
 *  @param[out] trace      A contiguous array of each trace (size sz*ns).
 *  @param[out] param      A handle for the parameter structure.
 */
void piol_file_read_interface_read_trace(
    const piol_file_read_interface* read_direct,
    size_t offset,
    size_t sz,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param);

/*! @brief Read the traces and trace parameters from offset to offset+sz.
 *
 *  @param[in]  write_direct A handle for the file.
 *  @param[in]  offset      The starting trace number.
 *  @param[in]  sz          The number of traces to process
 *  @param[out] trace       A contiguous array of each trace (size sz*ns).
 *  @param[in]  param       A handle for the parameter structure.
 *
 *  @warning This function is not thread safe.
 */
void piol_file_write_interface_write_trace(
    piol_file_write_interface* write_direct,
    size_t offset,
    size_t sz,
    exseis_Trace_value* trace,
    const piol_file_trace_metadata* param);

/*! @} Reading the traces themselves */

/*! @name Lists
 *
 *  @{
 */

/*! @brief Read the traces and trace parameters corresponding to the list of
 *         trace numbers.
 *
 *  @param[in]  read_direct A handle for the file.
 *  @param[in]  sz         The number of traces to process.
 *  @param[in]  offset     A list of trace numbers (size sz).
 *  @param[out] trace      A contiguous array of each trace (size sz*ns).
 *  @param[out] param      A handle for the parameter structure
 *                         (pass NULL to ignore).
 */
void piol_file_read_interface_read_trace_non_contiguous(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param);

/*! @brief Read the traces and trace parameters corresponding to the
 *         non-monotonic list of trace numbers.
 *
 *  @param[in]  read_direct A handle for the file.
 *  @param[in]  sz          The number of traces to process.
 *  @param[in]  offset      A non-monotonic list of trace numbers (size sz).
 *  @param[out] trace       A contiguous array of each trace (size sz*ns).
 *  @param[out] param       A handle for the parameter structure
 *                          (pass NULL to ignore).
 */
void piol_file_read_interface_read_trace_non_monotonic(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param);

/*! @brief Write the traces corresponding to the list of trace numbers.
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] sz           The number of traces to process.
 *  @param[in] offset       A list of trace numbers (size sz).
 *  @param[in] trace        A contiguous array of each trace (size sz*ns).
 *  @param[in] param        A handle to the parameter structure
 *                          (pass NULL to ignore).
 */
void piol_file_write_interface_write_trace_non_contiguous(
    piol_file_write_interface* write_direct,
    size_t sz,
    const size_t* offset,
    exseis_Trace_value* trace,
    piol_file_trace_metadata* param);

/*! @brief Write the trace parameters corresponding to the list of trace
 *         numbers.
 *
 *  @param[in] write_direct A handle for the file.
 *  @param[in] sz           The number of traces to process
 *  @param[in] offset       A list of trace numbers (size sz).
 *  @param[in] param        An handle to the parameter structure.
 */
void piol_file_write_interface_write_param_non_contiguous(
    piol_file_write_interface* write_direct,
    size_t sz,
    const size_t* offset,
    piol_file_trace_metadata* param);

/*! @brief Read the trace parameters corresponding to the list of trace numbers.
 *
 *  @param[in] read_direct A handle for the file.
 *  @param[in] sz          The number of traces to process
 *  @param[in] offset      A list of trace numbers (size sz).
 *  @param[in] param       An handle to the parameter structure.
 */
void piol_file_read_interface_read_param_non_contiguous(
    piol_file_read_interface* read_direct,
    size_t sz,
    const size_t* offset,
    piol_file_trace_metadata* param);

/*! @} Lists */

#ifdef DISABLED_OPTIONS

/*! @name Extended parameters
 *
 *  @{
 */

/*! @brief A list of the different modes of file access.
 */
enum Mode {
    /// @brief The file is opened as read only.
    ReadMode,
    /// @brief The file is opened as write only.
    WriteMode,
    /// @brief The file is opened as Read/Write.
    ReadWriteMode
};

/*! @brief A structure specifying all MPI-IO options
 */
typedef struct {
    /// @brief The file access mode
    enum Mode mode;
    /// @brief The MPI_Info object
    MPI_Info info;
    /// @brief The maximum size to write in an MPI-IO call.
    size_t max_size;
    /// @brief The MPI communicator which should be used.
    MPI_Comm fcomm;
} MPIIOOptions;

/*! @brief A structure specifying MPI options.
 */
typedef struct {
    /// @brief The MPI communicator
    MPI_Comm comm;
    /// @brief Whether the PIOL should initialise MPI or not
    bool initMPI;
} MPIOptions;

/*! @brief A structure specifying file layer options.
 */
typedef struct {
    /// @brief The interval factor which should be used with sample_interval.
    double sample_interval_factor;
} SEGYOptions;

/*! @} Extended parameters */

#endif  // DISABLED_OPTIONS

#ifdef __cplusplus
}  // extern "C"
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_H
