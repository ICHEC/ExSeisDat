////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c ReadSEGYModel
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadSEGYModel.hh"

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

    llint il0 = getPrm<llint>(0LU, PIOL_META_il, &prm);
    llint xl0 = getPrm<llint>(0LU, PIOL_META_xl, &prm);

    llint ilInc = getPrm<llint>(1LU, PIOL_META_il, &prm) - il0;
    llint ilNum =
      (ilInc ? (getPrm<llint>(2LU, PIOL_META_il, &prm) - il0) / ilInc : 0LU);
    llint xlNum = (ReadSEGY::readNt() / (ilNum ? ilNum : 1LU));
    llint xlInc = (getPrm<llint>(2LU, PIOL_META_xl, &prm) - xl0) / xlNum;

    ilInc = (ilInc ? ilInc : 1LU);
    xlInc = (xlInc ? xlInc : 1LU);

    il = std::make_tuple(il0, ilNum, ilInc);
    xl = std::make_tuple(xl0, xlNum, xlInc);
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
        offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il))
                       * std::get<1>(xl)
                     + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
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
        offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il))
                       * std::get<1>(xl)
                     + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
    }

    readTraceNonContiguous(
      offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);

    return trc;
}

}  // namespace PIOL
