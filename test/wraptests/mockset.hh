#ifndef PIOLWRAPTESTMOCKSET_HEADER_GUARD
#define PIOLWRAPTESTMOCKSET_HEADER_GUARD

#include "gmock/gmock.h"
#include "flow/set.hh"
#include "file/dynsegymd.hh"

namespace PIOL {

class MockSet;
::testing::StrictMock<MockSet>& mockSet();

class MockSet
{
public:
    MOCK_METHOD5(
        ctor,
        void(
            Set*, Piol piol_, std::string pattern, std::string outfix_,
            std::shared_ptr<File::Rule> rule_
        )
    );

    MOCK_METHOD3(ctor, void(Set*, Piol piol_, std::shared_ptr<File::Rule> rule_));

    MOCK_METHOD1(dtor, void(Set*));

    MOCK_METHOD2(sort, void(Set*, CompareP sortFunc));

    MOCK_METHOD3(
        sort, void(Set*, std::shared_ptr<File::Rule> r, CompareP sortFunc)
    );

    MOCK_METHOD2(output, std::vector<std::string>(Set*, std::string oname));

    MOCK_METHOD4(
        getMinMax,
        void(
            Set*, MinMaxFunc<File::Param> xlam, MinMaxFunc<File::Param> ylam,
            CoordElem * minmax
        )
    );

    MOCK_METHOD4(
        taper, void(Set*, TaperFunc tapFunc, size_t nTailLft, size_t nTailRt)
    );

    MOCK_METHOD4(
        AGC,   void(Set*, AGCFunc agcFunc, size_t window, trace_t normR)
    );

    MOCK_METHOD2(text, void(Set*, std::string outmsg_));

    MOCK_CONST_METHOD1(summary, void(const Set*));

    MOCK_METHOD2(add_impl, void(Set*, std::unique_ptr<File::ReadInterface>& in));
    void add(Set* set, std::unique_ptr<File::ReadInterface> in)
    {
        add_impl(set, in);
    }

    MOCK_METHOD2(add, void(Set*, std::string name));

    MOCK_METHOD5(
        toAngle,
        void(Set*, std::string vmName, csize_t vBin, csize_t oGSz, geom_t oInc)
    );

    MOCK_METHOD2(sort, void(Set*, SortType type));

    MOCK_METHOD4(getMinMax, void(Set*, Meta m1, Meta m2, CoordElem * minmax));

    MOCK_METHOD4(
        taper, void(Set*, TaperType type, size_t nTailLft, size_t nTailRt)
    );

    MOCK_METHOD4(AGC, void(Set*, AGCType type, size_t window, trace_t normR));

    MOCK_METHOD8(
        temporalFilter,
        void(
            Set*,
            FltrType type, FltrDmn domain, PadType pad, trace_t fs,
            std::vector<trace_t> corners, size_t nw, size_t winCntr
        )
    );

    MOCK_METHOD9(
        temporalFilter,
        void(
            Set*,
            FltrType type, FltrDmn domain, PadType pad, trace_t fs,
            size_t N, std::vector<trace_t> corners, size_t nw, size_t winCntr
        )
    );
};

} // namespace PIOL

#endif
