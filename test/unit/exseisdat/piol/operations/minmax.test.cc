#include "exseisdat/piol/operations/minmax.hh"

#include "exseisdat/test/catch2.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/piol/operations/sort.hh"
#include "exseisdat/utils/communicator/Communicator_mpi.hh"
#include "exseisdat/utils/signal_processing/AGC.hh"
#include "exseisdat/utils/signal_processing/mute.hh"
#include "exseisdat/utils/signal_processing/taper.hh"

using namespace exseis::piol;

struct Coord {
    /// The x coordinate
    exseis::utils::Floating_point x = 0;
    /// The y coordinate
    exseis::utils::Floating_point y = 0;
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
    ExSeisPIOL* piol,
    size_t offset,
    size_t lnt,
    const Coord* coord,
    CoordElem* minmax)
{
    auto xlam = [](const Coord& a) -> exseis::utils::Floating_point {
        return a.x;
    };
    auto ylam = [](const Coord& a) -> exseis::utils::Floating_point {
        return a.y;
    };
    get_min_max<Coord>(piol, offset, lnt, coord, xlam, ylam, minmax);
}

TEST_CASE("get_min_max", "[piol][get_min_max]")
{
    auto piol = ExSeis::make();

    SECTION ("simple") {
        std::vector<Coord> coord(1000);
        for (int i = 0; i < 1000; i++) {
            coord[i] = {1500. + i, 1300. - i};
        }

        std::vector<CoordElem> minmax(4);
        for (size_t offset = 0; offset < 300000; offset += 1 + offset * 10) {
            get_min_max(
                piol.get(), offset, coord.size(), coord.data(), minmax.data());
            piol->assert_ok();
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

        const auto test_min_max = [](bool y, bool min,
                                     const std::vector<Coord>& coord,
                                     const std::vector<CoordElem>& minmax) {
            exseis::utils::Floating_point val =
                exseis::utils::Floating_point(min ? 1 : -1)
                * std::numeric_limits<
                    exseis::utils::Floating_point>::infinity();
            size_t tn = 0;
            for (size_t i = 0; i < coord.size(); i++) {
                double t = (y ? coord[i].y : coord[i].x);
                if ((t < val && min) || (t > val && !min)) {
                    val = t;
                    tn  = i;
                }
            }

            REQUIRE(val == Approx(minmax[2U * size_t(y) + size_t(!min)].val));
            REQUIRE(tn == minmax[2U * size_t(y) + size_t(!min)].num);
        };

        srand(31337);
        std::vector<CoordElem> minmax(4);
        for (size_t l = 0; l < 20; l++) {
            size_t num = rand() % 1710880;
            std::vector<Coord> coord(num);
            for (size_t i = 0; i < num; i++) {
                coord[i] = {1.0 * rand(), 1.0 * rand()};
            }

            get_min_max(
                piol.get(), 0, coord.size(), coord.data(), minmax.data());
            piol->assert_ok();
            test_min_max(false, false, coord, minmax);
            test_min_max(false, true, coord, minmax);
            test_min_max(true, false, coord, minmax);
            test_min_max(true, true, coord, minmax);
        }
    }
}
