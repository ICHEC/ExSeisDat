#include "exseisdat/piol/operations/sort.hh"

#include "exseisdat/test/catch2.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/utils/communicator/Communicator_mpi.hh"
#include "exseisdat/utils/signal_processing/AGC.hh"
#include "exseisdat/utils/signal_processing/mute.hh"
#include "exseisdat/utils/signal_processing/taper.hh"

using namespace exseis::piol;

TEST_CASE("sort", "[piol][operations][sort]")
{
    auto piol = ExSeis::make();

    SECTION ("SrtRcvBackwards") {
        Trace_metadata trace_metadata(200);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::x_src,
                1000.0 - (trace_metadata.size() * piol->get_rank() + i) / 20);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_src,
                1000.0 - (trace_metadata.size() * piol->get_rank() + i) % 20);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::x_rcv,
                1000.0 - (trace_metadata.size() * piol->get_rank() + i) / 10);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_rcv,
                1000.0 - (trace_metadata.size() * piol->get_rank() + i) % 10);
            trace_metadata.set_index(
                i, Trace_metadata_key::gtn,
                trace_metadata.size() * piol->get_rank() + i);
        }
        auto list = sort(piol.get(), Sort_type::SrcRcv, trace_metadata);
        size_t global_list_size = piol->comm->sum(list.size());
        for (size_t i = 0; i < list.size(); i++) {
            REQUIRE(
                global_list_size
                    - (trace_metadata.size() * piol->get_rank() + i) - 1
                == list[i]);
        }
    }

    SECTION ("SrcRcvForwards") {
        Trace_metadata trace_metadata(200);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::x_src,
                1000.0 + (trace_metadata.size() * piol->get_rank() + i) / 20);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_src,
                1000.0 + (trace_metadata.size() * piol->get_rank() + i) % 20);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::x_rcv,
                1000.0 + (trace_metadata.size() * piol->get_rank() + i) / 10);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_rcv,
                1000.0 + (trace_metadata.size() * piol->get_rank() + i) % 10);
            trace_metadata.set_index(
                i, Trace_metadata_key::gtn,
                trace_metadata.size() * piol->get_rank() + i);
        }
        auto list = sort(piol.get(), Sort_type::SrcRcv, trace_metadata);

        for (size_t i = 0; i < list.size(); i++) {
            REQUIRE(trace_metadata.size() * piol->get_rank() + i == list[i]);
        }
    }

    SECTION ("SrcRcvRand") {
        Trace_metadata trace_metadata(10);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_src, 1000.0);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::x_rcv, 1000.0);
            trace_metadata.set_floating_point(
                i, Trace_metadata_key::y_rcv, 1000.0);
            trace_metadata.set_index(
                i, Trace_metadata_key::gtn, 10 * piol->get_rank() + i);
        }
        trace_metadata.set_floating_point(
            0, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 5.0);
        trace_metadata.set_floating_point(
            1, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 3.0);
        trace_metadata.set_floating_point(
            2, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 1.0);
        trace_metadata.set_floating_point(
            3, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 4.0);
        trace_metadata.set_floating_point(
            4, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 2.0);
        trace_metadata.set_floating_point(
            5, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 9.0);
        trace_metadata.set_floating_point(
            6, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 6.0);
        trace_metadata.set_floating_point(
            7, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 8.0);
        trace_metadata.set_floating_point(
            8, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 7.0);
        trace_metadata.set_floating_point(
            9, Trace_metadata_key::x_src, 10.0 * piol->get_rank() + 0.0);

        auto list = sort(piol.get(), Sort_type::SrcRcv, trace_metadata);

        REQUIRE(10 * piol->get_rank() + 5U == list[0]);
        REQUIRE(10 * piol->get_rank() + 3U == list[1]);
        REQUIRE(10 * piol->get_rank() + 1U == list[2]);
        REQUIRE(10 * piol->get_rank() + 4U == list[3]);
        REQUIRE(10 * piol->get_rank() + 2U == list[4]);
        REQUIRE(10 * piol->get_rank() + 9U == list[5]);
        REQUIRE(10 * piol->get_rank() + 6U == list[6]);
        REQUIRE(10 * piol->get_rank() + 8U == list[7]);
        REQUIRE(10 * piol->get_rank() + 7U == list[8]);
        REQUIRE(10 * piol->get_rank() + 0U == list[9]);
    }
}
