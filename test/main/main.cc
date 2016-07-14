#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
using namespace PIOL;
int main(int argc, char ** argv)
{
    Comm::MPIOpt opt;
    ExSeisPIOL piol(opt);

    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

