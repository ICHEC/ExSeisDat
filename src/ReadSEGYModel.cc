////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c ReadSEGYModel
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/ReadSEGYModel.hh"
#include "ExSeisDat/PIOL/param_utils.hh"

namespace exseis {
namespace PIOL {

ReadSEGYModel::ReadSEGYModel(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<ObjectInterface> obj_) :
    ReadSEGYModel(piol_, name_, ReadSEGYModel::Opt(), obj_)
{
}

ReadSEGYModel::ReadSEGYModel(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const ReadSEGYModel::Opt& opt,
  std::shared_ptr<ObjectInterface> obj_) :
    ReadSEGY(piol_, name_, opt, obj_)
{
    std::vector<size_t> vlist = {0LU, 1LU, ReadSEGY::readNt() - 1LU};
    Param prm(vlist.size());
    readParamNonContiguous(vlist.size(), vlist.data(), &prm);

    exseis::utils::Integer il_start =
      param_utils::getPrm<exseis::utils::Integer>(0LU, PIOL_META_il, &prm);
    exseis::utils::Integer xl_start =
      param_utils::getPrm<exseis::utils::Integer>(0LU, PIOL_META_xl, &prm);

    exseis::utils::Integer il_increment =
      param_utils::getPrm<exseis::utils::Integer>(1LU, PIOL_META_il, &prm)
      - il_start;
    exseis::utils::Integer il_count =
      (il_increment ?
         (param_utils::getPrm<exseis::utils::Integer>(2LU, PIOL_META_il, &prm)
          - il_start)
           / il_increment :
         0LU);
    exseis::utils::Integer xl_count =
      (ReadSEGY::readNt() / (il_count ? il_count : 1LU));
    exseis::utils::Integer xl_increment =
      (param_utils::getPrm<exseis::utils::Integer>(2LU, PIOL_META_xl, &prm)
       - xl_start)
      / xl_count;

    il_increment = (il_increment ? il_increment : 1LU);
    xl_increment = (xl_increment ? xl_increment : 1LU);

    il = CoordinateParameters(il_start, il_count, il_increment);
    xl = CoordinateParameters(xl_start, xl_count, xl_increment);
}

std::vector<exseis::utils::Trace_value> ReadSEGYModel::readModel(
  const size_t offset,
  const size_t sz,
  const utils::Distributed_vector<Gather_info>& gather)
{
    std::vector<exseis::utils::Trace_value> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val = gather[offset + i];
        /* The below can be translated to:
         * trace number = ilNumber * xlInc + xlNumber
         * much like indexing in a 2d array.
         */
        offsets[i] = ((val.inline_ - il.start) / il.increment) * xl.count
                     + ((val.crossline - xl.start) / xl.increment);
    }

    readTraceNonContiguous(
      offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);

    return trc;
}

std::vector<exseis::utils::Trace_value> ReadSEGYModel::readModel(
  const size_t sz,
  const size_t* offset,
  const utils::Distributed_vector<Gather_info>& gather)
{
    std::vector<exseis::utils::Trace_value> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val   = gather[offset[i]];
        offsets[i] = ((val.inline_ - il.start) / il.increment) * xl.count
                     + ((val.crossline - xl.start) / xl.increment);
    }

    readTraceNonContiguous(
      offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);

    return trc;
}

}  // namespace PIOL
}  // namespace exseis
