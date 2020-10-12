#include "exseis/piol/operations/minmax.hh"

#include "exseis/test/catch2.hh"

#include "exseis/piol/operations/minmax.hh"
#include "exseis/piol/operations/sort.hh"
#include "exseis/utils/communicator/Communicator.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"
#include "exseis/utils/signal_processing/AGC.hh"
#include "exseis/utils/signal_processing/mute.hh"
#include "exseis/utils/signal_processing/taper.hh"


struct Coord {
    /// The x coordinate
    exseis::Floating_point x = 0;
    /// The y coordinate
    exseis::Floating_point y = 0;
};

/*! Get the min and the max of a set of parameters passed. This is a parallel
 *  operation. It is the collective min and max across all processes (which also
 *  must all call this file).
 *  @param[in,out] piol The PIOL object
 *  @param[in] offset The starting trace number (local).
 *  @param[in] lnt The local number of traces to process.
 *  @param[in] coord
 *  @param[out] minmax An array of structures containing the minimum item.x,
 *                     maximum item.x, minimum item.y, maximum item.y and their
 *                     respective trace numbers.
 */
void get_min_max(
    const exseis::Communicator& communicator,
    size_t offset,
    size_t lnt,
    const Coord* coord,
    exseis::CoordElem* minmax)
{
    auto xlam = [](const Coord& a) -> exseis::Floating_point { return a.x; };
    auto ylam = [](const Coord& a) -> exseis::Floating_point { return a.y; };
    exseis::get_min_max<Coord>(
        communicator, offset, lnt, coord, xlam, ylam, minmax);
}

TEST_CASE("get_min_max", "[piol][get_min_max]")
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    SECTION ("simple") {
        std::vector<Coord> coord(1000);
        for (int i = 0; i < 1000; i++) {
            coord[i] = {1500. + i, 1300. - i};
        }

        std::vector<exseis::CoordElem> minmax(4);
        for (size_t offset = 0; offset < 300000; offset += 1 + offset * 10) {
            get_min_max(
                communicator, offset, coord.size(), coord.data(),
                minmax.data());
            REQUIRE(offset == minmax[0].num);
            REQUIRE(offset + 999 == minmax[1].num);
            REQUIRE(offset + 999 == minmax[2].num);
            REQUIRE(offset == minmax[3].num);

            REQUIRE(minmax[0].val == Approx(1500.));  // min x
            REQUIRE(minmax[1].val == Approx(2499.));  // max x
            REQUIRE(minmax[2].val == Approx(301.));   // min x
            REQUIRE(minmax[3].val == Approx(1300.));  // max x
        }
    }

    SECTION ("rand") {

        const auto test_min_max =
            [](bool y, bool min, const std::vector<Coord>& coord,
               const std::vector<exseis::CoordElem>& minmax) {
                exseis::Floating_point val =
                    exseis::Floating_point(min ? 1 : -1)
                    * std::numeric_limits<exseis::Floating_point>::infinity();
                size_t tn = 0;
                for (size_t i = 0; i < coord.size(); i++) {
                    double t = (y ? coord[i].y : coord[i].x);
                    if ((t < val && min) || (t > val && !min)) {
                        val = t;
                        tn  = i;
                    }
                }

                REQUIRE(
                    val == Approx(minmax[2U * size_t(y) + size_t(!min)].val));
                REQUIRE(tn == minmax[2U * size_t(y) + size_t(!min)].num);
            };

        srand(31337);
        std::vector<exseis::CoordElem> minmax(4);
        for (size_t l = 0; l < 20; l++) {
            size_t num = rand() % 1710880;
            std::vector<Coord> coord(num);
            for (size_t i = 0; i < num; i++) {
                coord[i] = {1.0 * rand(), 1.0 * rand()};
            }

            get_min_max(
                communicator, 0, coord.size(), coord.data(), minmax.data());
            test_min_max(false, false, coord, minmax);
            test_min_max(false, true, coord, minmax);
            test_min_max(true, false, coord, minmax);
            test_min_max(true, true, coord, minmax);
        }
    }
}
