#include "global.hh"
#include "cfileapi.h"
#include "cppfileapi.hh"
#include "flow.h"
#include "flow.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <functional>

extern std::function<
    void(PIOL::Piol piol_, std::string pattern, std::string outfix_,
         std::shared_ptr<PIOL::File::Rule> rule_)
> PIOL_Set_new_1_cb;

extern std::function<
    void(PIOL::Piol piol_, std::shared_ptr<PIOL::File::Rule> rule_)
> PIOL_Set_new_2_cb;


extern std::function<void()> PIOL_Set_delete_cb;

extern std::function<void(PIOL::CompareP)> PIOL_Set_sort_1_cb;

extern std::function<void(std::shared_ptr<PIOL::File::Rule>, PIOL::CompareP)> PIOL_Set_sort_2_cb;

extern std::function<std::vector<std::string>(std::string)> PIOL_Set_output_cb;

extern std::function<
    void(MinMaxFunc<PIOL::File::Param> xlam,
         MinMaxFunc<PIOL::File::Param> ylam, CoordElem * minmax)
> PIOL_Set_getMinMax_1_cb;

extern std::function<void(Meta m1, Meta m2, CoordElem * minmax)> PIOL_Set_getMinMax_2_cb;


extern std::function<void(std::string outmsg_)> PIOL_Set_text_cb;

extern std::function<void(void)> PIOL_Set_summary_cb;

extern std::function<void(std::unique_ptr<PIOL::File::ReadInterface> in)> PIOL_Set_add_1_cb;

extern std::function<void(std::string name)> PIOL_Set_add_2_cb;

extern std::function<
    void(std::string vmName, csize_t vBin, csize_t oGSz, geom_t oInc)
> PIOL_Set_toAngle_cb;

extern std::function<void(SortType type)> PIOL_Set_sort_cb;

extern std::function<void(TaperType type, size_t nTailLft, size_t nTailRt)> PIOL_Set_taper_1_cb;

extern std::function<void(TaperFunc tapFunc, size_t nTailLft, size_t nTailRt)> PIOL_Set_taper_2_cb;

extern std::function<void(AGCType type, size_t window, trace_t normR)> PIOL_Set_AGC_1_cb;

extern std::function<void(AGCFunc agcFunc, size_t window, trace_t normR)> PIOL_Set_AGC_2_cb;

extern std::function<
    void(PIOL::FltrType type, PIOL::FltrDmn domain, PIOL::PadType pad,
         trace_t fs, std::vector<trace_t> corners,
         size_t nw, size_t winCntr)
> PIOL_Set_temporalFilter_1_cb;

extern std::function<
    void(PIOL::FltrType type, PIOL::FltrDmn domain, PIOL::PadType pad,
         trace_t fs, size_t N,
         std::vector<trace_t> corners, size_t nw, size_t winCntr)
> PIOL_Set_temporalFilter_2_cb;
