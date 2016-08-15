#include "filesegytest.hh"

//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegRead, SEGYReadHO)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    makeSEGY<false>(smallSEGYFile);

    piol->isErr();
    EXPECT_EQ(ns, file->readNs());
    piol->isErr();
    EXPECT_EQ(nt, file->readNt());
    piol->isErr();
    EXPECT_DOUBLE_EQ(double(20e-6), file->readInc());
}


