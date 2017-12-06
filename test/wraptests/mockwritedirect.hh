#ifndef PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD
#define PIOLWRAPTESTSMOCKWRITEDIRECT_HEADER_GUARD

#include "gmock/gmock.h"
#include "cppfileapi.hh"

namespace PIOL {
using namespace File;

class MockWriteDirect;
MockWriteDirect& mockWriteDirect();

class MockWriteDirect
{
public :
    MOCK_METHOD3(ctor, void(WriteDirect*, const Piol piol, const std::string name));
    MOCK_METHOD2(ctor, void(WriteDirect*, std::shared_ptr<WriteInterface> file));
    MOCK_METHOD1(dtor, void(WriteDirect*));
    MOCK_METHOD2(writeText,  void(WriteDirect*, const std::string text_));
    MOCK_METHOD2(writeNs,    void(WriteDirect*, csize_t ns_));
    MOCK_METHOD2(writeNt,    void(WriteDirect*, csize_t nt_));
    MOCK_METHOD2(writeInc,   void(WriteDirect*, const geom_t inc_));
    MOCK_METHOD5(
        writeTrace,
        void(
            WriteDirect*,
            csize_t offset, csize_t sz, trace_t * trace, const Param * prm
        )
    );
    MOCK_METHOD4(
        writeParam,
        void(WriteDirect*, csize_t offset, csize_t sz, const Param * prm)
    );
    MOCK_METHOD5(
        writeTrace,
        void(
            WriteDirect*,
            csize_t sz, csize_t * offset, trace_t * trace, const Param * prm
        )
    );
    MOCK_METHOD4(
        writeParam,
        void(WriteDirect*, csize_t sz, csize_t * offset, const Param * prm)
    );
};

} // namespace PIOL

#endif
