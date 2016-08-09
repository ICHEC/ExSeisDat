#include "filesegytest.hh"

#warning Add read bigger traces and coordinates

//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYReadHO)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    makeSEGY(smallSEGYFile);

    piol->isErr();
    EXPECT_EQ(ns, file->readNs());
    piol->isErr();
    EXPECT_EQ(nt, file->readNt());
    piol->isErr();
    EXPECT_DOUBLE_EQ(double(20e-6), file->readInc());
}

//TODO: Add test same as above for big files

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteReadHO)
{
    SCOPED_TRACE("SEGYWriteHO");
    csize_t ns = 261U;
    csize_t nt = 400U;

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    makeSEGY(tempFile);
    piol->isErr();

    file->writeNs(ns);
    piol->isErr();
    file->writeNt(nt);
    piol->isErr();
    file->writeText(testString);
    piol->isErr();

    obj = file->obj;         //steal object layer for read

    makeSEGY(tempFile, obj);

    EXPECT_EQ(ns, file->readNs());
    piol->isErr();
    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    std::string temp = file->readText();
    piol->isErr();
    ASSERT_TRUE(testString.size() <= temp.size());
    for (size_t i = 0; i < testString.size(); i++)
        EXPECT_EQ(testString[i], temp[i]);
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteReadTraceGrid)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    File::grid_t grid(ilNum(201), xlNum(201));

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;

    makeSEGY(tempFile);
    piol->isErr();
    file->ns = ns;
    file->writeNt(nt);
    piol->isErr();

    TraceParam prm;
    prm.line = grid;
    file->writeTraceParam(201U, 1U, &prm);
    obj = file->obj;         //steal object layer for read

    makeSEGY(tempFile, obj);
    grid_t grd;
    file->readGridPoint(File::Grid::Line, 201, 1U, &grd);
    EXPECT_EQ(grid, grd);
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileIntegrationTest, SEGYWriteReadTraceCoord)
{
    csize_t ns = 261U;
    csize_t nt = 400U;
    File::coord_t coord(1600, 2000);

    std::shared_ptr<Obj::Interface> obj;
    dataOpt.mode = MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE;
    makeSEGY(tempFile);
    piol->isErr();
    file->ns = ns;
    file->writeNt(nt);
    piol->isErr();

    TraceParam prm;
    prm.cmp = coord;
    file->writeTraceParam(200U, 1U, &prm);

    obj = file->obj;         //steal object layer for read
    makeSEGY(tempFile, obj);

    File::coord_t crd;
    file->readCoordPoint(File::Coord::CMP, 200, 1U, &crd);
    EXPECT_EQ(coord, crd);
}

