////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c ReadSEGYModel
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/Param.hh"
#include "ExSeisDat/PIOL/ReadSEGYModel.hh"
#include "ExSeisDat/PIOL/param_utils.hh"

namespace PIOL {

ReadSEGYModel::ReadSEGYModel(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  std::shared_ptr<Obj::Interface> obj_) :
    ReadSEGYModel(piol_, name_, ReadSEGYModel::Opt(), obj_)
{
}

ReadSEGYModel::ReadSEGYModel(
  std::shared_ptr<ExSeisPIOL> piol_,
  const std::string name_,
  const ReadSEGYModel::Opt& opt,
  std::shared_ptr<Obj::Interface> obj_) :
    ReadSEGY(piol_, name_, opt, obj_)
{
    std::vector<size_t> vlist = {0LU, 1LU, ReadSEGY::readNt() - 1LU};
    Param prm(vlist.size());
    readParamNonContiguous(vlist.size(), vlist.data(), &prm);

    llint il_start = getPrm<llint>(0LU, PIOL_META_il, &prm);
    llint xl_start = getPrm<llint>(0LU, PIOL_META_xl, &prm);

    llint il_increment = getPrm<llint>(1LU, PIOL_META_il, &prm) - il_start;
    llint il_count =
      (il_increment ?
         (getPrm<llint>(2LU, PIOL_META_il, &prm) - il_start) / il_increment :
         0LU);
    llint xl_count = (ReadSEGY::readNt() / (il_count ? il_count : 1LU));
    llint xl_increment =
      (getPrm<llint>(2LU, PIOL_META_xl, &prm) - xl_start) / xl_count;

    il_increment = (il_increment ? il_increment : 1LU);
    xl_increment = (xl_increment ? xl_increment : 1LU);

    il = CoordinateParameters(il_start, il_count, il_increment);
    xl = CoordinateParameters(xl_start, xl_count, xl_increment);
}

std::vector<trace_t> ReadSEGYModel::readModel(
  const size_t offset,
  const size_t sz,
  const Uniray<size_t, llint, llint>& gather)
{
    std::vector<trace_t> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val = gather[offset + i];
        /* The below can be translated to:
         * trace number = ilNumber * xlInc + xlNumber
         * much like indexing in a 2d array.
         */
        offsets[i] = ((std::get<1>(val) - il.start) / il.increment) * xl.count
                     + ((std::get<2>(val) - xl.start) / xl.increment);
    }

    readTraceNonContiguous(
      offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);

    return trc;
}

std::vector<trace_t> ReadSEGYModel::readModel(
  const size_t sz,
  const size_t* offset,
  const Uniray<size_t, llint, llint>& gather)
{
    std::vector<trace_t> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val   = gather[offset[i]];
        offsets[i] = ((std::get<1>(val) - il.start) / il.increment) * xl.count
                     + ((std::get<2>(val) - xl.start) / xl.increment);
    }

    readTraceNonContiguous(
      offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);

    return trc;
}

}  // namespace PIOL
