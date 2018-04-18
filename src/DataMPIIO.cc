////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c DataMPIIO
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/utils/mpi/MPI_error_to_string.hh"
#include "ExSeisDat/utils/mpi/MPI_max_array_length.hh"
#include "ExSeisDat/utils/mpi/MPI_type.hh"

#include <algorithm>
#include <assert.h>
#include <string>

using namespace std::string_literals;

namespace exseis {
namespace PIOL {

/////////////////////////////       Non-Class      /////////////////////////////

/*! Set a view on a file so that a read of blocks separated by (stride-block)
 *  bytes appears contiguous
 *  @param[in] file The MPI-IO file handle
 *  @param[in] info The info structure to use
 *  @param[in] offset The offset in bytes from the start of the file
 *  @param[in] block The block size in bytes
 *  @param[in] stride The stride size in bytes block start to block start
 *  @param[in] count The number of blocks
 *  @param[out] type The datatype which will have been used to create a view
 *  @return Return an MPI error code.
 */
int strideView(
  MPI_File file,
  MPI_Info info,
  MPI_Offset offset,
  int block,
  MPI_Aint stride,
  int count,
  MPI_Datatype* type)
{
    int err = MPI_Type_create_hvector(count, block, stride, MPI_CHAR, type);
    if (err != MPI_SUCCESS) return err;

    err = MPI_Type_commit(type);
    if (err != MPI_SUCCESS) return err;

    return MPI_File_set_view(file, offset, MPI_CHAR, *type, "native", info);
}

/*! Set a view on a file so that a read of random traces appears contiguous
 *  @param[in] file The MPI-IO file handle
 *  @param[in] info The info structure to use
 *  @param[in] count The number of offsets
 *  @param[in] block The block size in bytes
 *  @param[in] offset An array of offsets in bytes from the start of the file of
 *                    size count
 *  @param[out] type The datatype which will have been used to create a view
 *  @return Return an MPI error code.
 */
int randBlockView(
  MPI_File file,
  MPI_Info info,
  int count,
  int block,
  const MPI_Aint* offset,
  MPI_Datatype* type)
{
#ifndef HINDEXED_BLOCK_WORKS
    std::vector<int> bl(count);
    for (int i = 0; i < count; i++)
        bl[i] = block;
    assert(
      size_t(count)
      < std::numeric_limits<int>::max() / (sizeof(int) + sizeof(MPI_Aint)));

    int err =
      MPI_Type_create_hindexed(count, bl.data(), offset, MPI_CHAR, type);
#else
    int err =
      MPI_Type_create_hindexed_block(count, block, offset, MPI_CHAR, type);
#endif
    if (err != MPI_SUCCESS) return err;

    err = MPI_Type_commit(type);
    if (err != MPI_SUCCESS) return err;

    return MPI_File_set_view(file, 0, MPI_BYTE, *type, "native", info);
}

/*! @brief This function exists to hide the const from the MPI_File_write_at
 *         function signature
 *  @param[in] f The MPI file handle
 *  @param[in] o The offset in bytes from the current internal shared pointer
 *  @param[in] d The array to read data output from
 *  @param[in] s The amount of data to write to disk in terms of datatypes
 *  @param[in] da The MPI datatype
 *  @param[in] st The MPI status structure
 *  @return Returns the associated MPI error code.
 */
int mpiio_write_at(
  MPI_File f, MPI_Offset o, void* d, int s, MPI_Datatype da, MPI_Status* st)
{
    return MPI_File_write_at(f, o, d, s, da, st);
}

/*! @brief This function exists to hide the const from the MPI_File_write_at_all
 *         function signature
 *  @param[in] f The MPI file handle
 *  @param[in] o The offset in bytes from the current internal shared pointer
 *  @param[in] d The array to read data output from
 *  @param[in] s The amount of data to write to disk in terms of datatypes
 *  @param[in] da The MPI datatype
 *  @param[in] st The MPI status structure
 *  @return Returns the associated MPI error code.
 */
int mpiio_write_at_all(
  MPI_File f, MPI_Offset o, void* d, int s, MPI_Datatype da, MPI_Status* st)
{
    return MPI_File_write_at_all(f, o, d, s, da, st);
}

/*! Perform list-based I/O by setting a view then performing the I/O
 *  @param[in] fn A contiguous I/O function
 *  @param[in] file The MPI file handle
 *  @param[in] info The MPI info object
 *  @param[in] bsz The block size
 *  @param[in] chunk The number of blocks to read
 *  @param[in] offset The list of offsets in the file
 *  @param[in, out] d The I/O buffer
 *  @param[in] stat The MPI status object
 *  @return Return the MPI error status
 */
int iol(
  const MFp<MPI_Status> fn,
  MPI_File file,
  MPI_Info info,
  int bsz,
  int chunk,
  const MPI_Aint* offset,
  unsigned char* d,
  MPI_Status* stat)
{
    // Set a view so that MPI_File_read... functions only see contiguous data.
    MPI_Datatype type;
    int err = randBlockView(file, info, chunk, bsz, offset, &type);
    if (err != MPI_SUCCESS) return err;

    fn(file, 0, d, chunk * bsz, MPI_CHAR, stat);
    if (err != MPI_SUCCESS) return err;

    // Reset the view.
    err = MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
    if (err != MPI_SUCCESS) return err;

    return MPI_Type_free(&type);
}

/////////////////////////////    Class functions    ////////////////////////////

//////////////////////      Constructor & Destructor      //////////////////////
DataMPIIO::Opt::Opt(void)
{
#ifdef EXSEISDAT_MPIIO_COLLECTIVES
    coll = true;
#else
    coll = false;
#endif
    fcomm = MPI_COMM_WORLD;
    info  = MPI_INFO_NULL;
    MPI_Info_create(&info);
    //    MPI_Info_set(info, "access_style", "read_once");
    //    MPI_Info_set(info, "romio_cb_read", "false");
    //    MPI_Info_set(info, "romio_cb_write", "false");
    //    MPI_Info_set(info, "ind_rd_buffer_size", "0");
    //    MPI_Info_set(info, "ind_wr_buffer_size", "0");
    //    MPI_Info_set(info, "direct_read", "true");
    //    MPI_Info_set(info, "direct_write", "true");

    //    MPI_Info_set(info, "cb_block_size", "");  see spec for more
    //    MPI_Info_set(info, "chunked", "");        see spec for more
    //    MPI_Info_set(info, "nb_proc", "");
    //    MPI_Info_set(info, "num_io_nodes", "");
    //    MPI_Info_set(info, "striping_factor", "10");
    //    MPI_Info_set(info, "striping_unit", "2097152");

    //    // ROMIO has this on by default. Annoying.
    //    MPI_Info_set(info, "panfs_concurrent_write", "false");
    maxSize = exseis::utils::MPI_max_array_length<int32_t>();
}

DataMPIIO::Opt::~Opt(void)
{
    if (info != MPI_INFO_NULL) MPI_Info_free(&info);
}

/*! Get an MPI mode flag
 *  @param[in] mode The generic input mode.
 *  @return The MPI mode flag associated with the inpute enum
 */
int getMPIMode(FileMode mode)
{
    switch (mode) {
        default:
        case FileMode::Read:
            return MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN;
        case FileMode::Write:
            return MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_UNIQUE_OPEN;
        case FileMode::ReadWrite:
            return MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN;
        case FileMode::Test:
            return MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    }
}

DataMPIIO::DataMPIIO(
  std::shared_ptr<ExSeisPIOL> piol,
  const std::string name,
  const DataMPIIO::Opt& opt,
  FileMode mode) :
    PIOL::DataInterface(piol, name)
{
    Init(opt, mode);
}

DataMPIIO::DataMPIIO(
  std::shared_ptr<ExSeisPIOL> piol, const std::string name, FileMode mode) :
    PIOL::DataInterface(piol, name)
{
    const DataMPIIO::Opt opt;
    Init(opt, mode);
}

DataMPIIO::~DataMPIIO(void)
{
    if (file != MPI_FILE_NULL) {
        int err = MPI_File_close(&file);
        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              "MPI_File_close error: "s
                + exseis::utils::MPI_error_to_string(err),
              PIOL_VERBOSITY_NONE);
        }
    }
    if (info != MPI_INFO_NULL) {
        int err = MPI_Info_free(&info);

        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              "MPI_Info_free error: "s
                + exseis::utils::MPI_error_to_string(err),
              PIOL_VERBOSITY_NONE);
        }
    }
}

void DataMPIIO::Init(const DataMPIIO::Opt& opt, FileMode mode)
{
    coll    = opt.coll;
    maxSize = opt.maxSize;
    file    = MPI_FILE_NULL;

    MPI_Aint lb  = 0;
    MPI_Aint esz = 0;

    int err = MPI_Type_get_true_extent(MPI_CHAR, &lb, &esz);
    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "Getting MPI extent error: "s
            + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }

    if (esz != 1) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "MPI_CHAR extent is bigger than one.", PIOL_VERBOSITY_NONE);
    }

    fcomm = opt.fcomm;

    int flags = getMPIMode(mode);

    if (opt.info != MPI_INFO_NULL) {
        err = MPI_Info_dup(opt.info, &info);
        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              "MPI_Info_dup error: "s + exseis::utils::MPI_error_to_string(err),
              PIOL_VERBOSITY_NONE);
        }
    }
    else {
        info = MPI_INFO_NULL;
    }

    err = MPI_File_open(fcomm, name_.data(), flags, info, &file);
    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "MPI_File_open error: "s + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }

    if (err == MPI_SUCCESS) {
        int err =
          MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);

        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              "DataMPIIO Constructor failed to set a view error: "s
                + exseis::utils::MPI_error_to_string(err),
              PIOL_VERBOSITY_NONE);
        }
    }
}

/////////////////////////       Member functions      //////////////////////////

bool DataMPIIO::isFileNull() const
{
    return file == MPI_FILE_NULL;
}

size_t DataMPIIO::getFileSz() const
{
    MPI_Offset fsz = 0;
    int err        = MPI_File_get_size(file, &fsz);

    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "MPI_File_size error: "s + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }

    return size_t(fsz);
}

void DataMPIIO::setFileSz(const size_t sz) const
{
    int err = MPI_File_set_size(file, MPI_Offset(sz));

    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "MPI_File_set_size error: "s
            + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }
}

void DataMPIIO::read(
  const size_t offset, const size_t sz, unsigned char* d) const
{
    contigIO(
      (coll ? MPI_File_read_at_all : MPI_File_read_at), offset, sz, d,
      " non-collective read Failure\n");
}

void DataMPIIO::readv(
  const size_t offset,
  const size_t bsz,
  const size_t osz,
  const size_t nb,
  unsigned char* d) const
{
    if (nb * osz > size_t(maxSize)) {
        std::string msg = "(nb, bsz, osz) = (" + std::to_string(nb) + ", "
                          + std::to_string(bsz) + ", " + std::to_string(osz)
                          + ")";
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "Read overflows MPI settings: " + msg, PIOL_VERBOSITY_NONE);
    }

    // Set a view so that MPI_File_read... functions only see contiguous data.
    MPI_Datatype view;
    int err = strideView(file, info, offset, bsz, osz, nb, &view);
    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "Failed to set a view for reading: "s
            + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }

    read(0LU, nb * bsz, d);

    // Reset the view.
    MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
    MPI_Type_free(&view);
}

void DataMPIIO::read(
  const size_t offset,
  const size_t bsz,
  const size_t osz,
  const size_t nb,
  unsigned char* d) const
{
    auto viewIO = [this, bsz, osz](
                    MPI_File, MPI_Offset off, void* d, int numb, MPI_Datatype,
                    MPI_Status*) -> int {
        readv(off, bsz, osz, size_t(numb), static_cast<unsigned char*>(d));
        return MPI_SUCCESS;
    };

    contigIO(
      viewIO, offset, nb, d, "Failed to read data over the integer limit.", bsz,
      osz);
}


void DataMPIIO::contigIO(
  const MFp<MPI_Status> fn,
  const size_t offset,
  const size_t sz,
  unsigned char* d,
  std::string msg,
  const size_t bsz,
  const size_t osz) const
{
    MPI_Status stat;
    size_t max     = maxSize / osz;
    size_t remCall = 0;
    auto vec       = piol_->comm->gather<size_t>(sz);
    remCall        = *std::max_element(vec.begin(), vec.end());
    remCall = remCall / max + (remCall % max > 0) - sz / max - (sz % max > 0);

    for (size_t i = 0; i < sz; i += max) {
        size_t chunk = std::min(sz - i, max);
        int err      = fn(
          file, MPI_Offset(offset + osz * i), &d[bsz * i], chunk,
          exseis::utils::MPI_type<unsigned char>(), &stat);

        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              msg + exseis::utils::MPI_error_to_string(err, &stat),
              PIOL_VERBOSITY_NONE);
        }
    }

    for (size_t i = 0; i < remCall; i++) {
        int err =
          fn(file, 0, NULL, 0, exseis::utils::MPI_type<unsigned char>(), &stat);

        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              msg + exseis::utils::MPI_error_to_string(err, &stat),
              PIOL_VERBOSITY_NONE);
        }
    }
}

// Perform I/O to acquire data corresponding to fixed-size blocks of data
// located  according to a list of offsets.
void DataMPIIO::listIO(
  const MFp<MPI_Status> fn,
  const size_t bsz,
  const size_t sz,
  const size_t* offset,
  unsigned char* d,
  std::string msg) const
{
    // TODO: More accurately determine a real limit for setting a view.
    //       Is the problem strides that are too big?
    size_t max     = maxSize / (bsz ? bsz * 2LU : 1LU);
    size_t remCall = 0;
    {
        auto vec = piol_->comm->gather<size_t>(sz);
        remCall  = *std::max_element(vec.begin(), vec.end());
        remCall =
          remCall / max + (remCall % max > 0) - (sz / max) - (sz % max > 0);
    }

    MPI_Status stat;
    for (size_t i = 0; i < sz; i += max) {

        size_t chunk = std::min(sz - i, max);

        int err = iol(
          fn, file, info, bsz, chunk,
          reinterpret_cast<const MPI_Aint*>(&offset[i]), &d[i * bsz], &stat);

        // Log and break on failure
        if (err != MPI_SUCCESS) {
            log_->record(
              name_, Logger::Layer::Data, Logger::Status::Error,
              msg + exseis::utils::MPI_error_to_string(err, &stat),
              PIOL_VERBOSITY_NONE);

            break;
        }
    }

    if (remCall) {
        for (size_t i = 0; i < remCall; i++) {
            int err = iol(fn, file, info, 0, 0, nullptr, nullptr, &stat);

            if (err != MPI_SUCCESS) {
                log_->record(
                  name_, Logger::Layer::Data, Logger::Status::Error,
                  msg + exseis::utils::MPI_error_to_string(err, &stat),
                  PIOL_VERBOSITY_NONE);
            }
        }
    }
}

void DataMPIIO::read(
  const size_t bsz,
  const size_t sz,
  const size_t* offset,
  unsigned char* d) const
{
    listIO(
      (coll ? MPI_File_read_at_all : MPI_File_read_at), bsz, sz, offset, d,
      "list read failure");
}

void DataMPIIO::write(
  const size_t bsz,
  const size_t sz,
  const size_t* offset,
  const unsigned char* d) const
{
    listIO(
      (coll ? mpiio_write_at_all : mpiio_write_at), bsz, sz, offset,
      const_cast<unsigned char*>(d), "list write failure");
}

void DataMPIIO::writev(
  const size_t offset,
  const size_t bsz,
  const size_t osz,
  const size_t nb,
  const unsigned char* d) const
{
    if (nb * osz > size_t(maxSize)) {
        std::string msg = "(nb, bsz, osz) = (" + std::to_string(nb) + ", "
                          + std::to_string(bsz) + ", " + std::to_string(osz)
                          + ")";
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "Write overflows MPI settings: " + msg, PIOL_VERBOSITY_NONE);
    }

    // Set a view so that MPI_File_read... functions only see contiguous data.
    MPI_Datatype view;
    int err = strideView(file, info, offset, bsz, osz, nb, &view);

    if (err != MPI_SUCCESS) {
        log_->record(
          name_, Logger::Layer::Data, Logger::Status::Error,
          "Failed to set a view for reading: "s
            + exseis::utils::MPI_error_to_string(err),
          PIOL_VERBOSITY_NONE);
    }

    write(0LU, nb * bsz, d);

    // Reset the view.
    MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
    MPI_Type_free(&view);
}

void DataMPIIO::write(
  const size_t offset, const size_t sz, const unsigned char* d) const
{
    contigIO(
      (coll ? mpiio_write_at_all : mpiio_write_at), offset, sz,
      const_cast<unsigned char*>(d), "Non-collective write failure.");
}

void DataMPIIO::write(
  const size_t offset,
  const size_t bsz,
  const size_t osz,
  const size_t nb,
  const unsigned char* d) const
{
    auto viewIO = [this, bsz, osz](
                    MPI_File, MPI_Offset off, void* d, int numb, MPI_Datatype,
                    MPI_Status*) -> int {
        writev(off, bsz, osz, size_t(numb), static_cast<unsigned char*>(d));
        return MPI_SUCCESS;
    };

    contigIO(
      viewIO, offset, nb, const_cast<unsigned char*>(d),
      "Failed to read data over the integer limit.", bsz, osz);
}

}  // namespace PIOL
}  // namespace exseis
