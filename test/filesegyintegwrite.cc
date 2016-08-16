#include "filesegytest.hh"

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegWrite, SEGYWriteReadHO)
{
    ns = 261U;
    nt = 400U;

    makeSEGY<true>(tempFile);

    EXPECT_EQ(ns, file->readNs());
    piol->isErr();

    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    std::string text = file->readText();
    piol->isErr();
    ASSERT_TRUE(testString.size() <= text.size());
    for (size_t i = 0; i < testString.size(); i++)
        ASSERT_EQ(testString[i], text[i]);
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegWrite, SEGYWriteReadTraceGrid)
{
    ns = 261U;
    nt = 400U;
    File::grid_t grid(ilNum(201), xlNum(201));

    makeSEGY<true>(tempFile);

    piol->isErr();
    file->ns = ns;
    file->writeNt(nt);
    piol->isErr();

    TraceParam prm;
    prm.line = grid;
    file->writeTraceParam(201U, 1U, &prm);

    grid_t grd;
    file->readGridPoint(File::Grid::Line, 201, 1U, &grd);
    EXPECT_EQ(grid, grd);
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegWrite, SEGYWriteReadTraceCoord)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    File::coord_t coord(1600, 2000);

    std::shared_ptr<Obj::Interface> obj;
    makeSEGY<true>(tempFile);
    piol->isErr();
    file->ns = ns;
    file->writeNt(nt);
    piol->isErr();

    TraceParam prm;
    prm.cmp = coord;
    file->writeTraceParam(200U, 1U, &prm);

    File::coord_t crd;
    file->readCoordPoint(File::Coord::CMP, 200, 1U, &crd);
    EXPECT_EQ(coord, crd);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(0);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(10);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigOffset)
{
    nt = 10;
    ns = 3000;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(3728270);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(0);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(10);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeSEGY<true>(tempFile);
    writeTraceTest<false>(10);
}

