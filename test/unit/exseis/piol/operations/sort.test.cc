#include "exseis/piol/operations/sort.hh"

#include "exseis/test/catch2.hh"

#include "exseis/piol/operations/minmax.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"
#include "exseis/utils/signal_processing/AGC.hh"
#include "exseis/utils/signal_processing/mute.hh"
#include "exseis/utils/signal_processing/taper.hh"


TEST_CASE("sort", "[piol][operations][sort]")
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    std::map<exseis::Trace_metadata_key, exseis::Trace_metadata_info>
        metadata_info{
            {exseis::Trace_metadata_key::source_x, {exseis::Type::Double}},
            {exseis::Trace_metadata_key::source_y, {exseis::Type::Double}},
            {exseis::Trace_metadata_key::receiver_x, {exseis::Type::Double}},
            {exseis::Trace_metadata_key::receiver_y, {exseis::Type::Double}},
            {exseis::Trace_metadata_key::gtn, {exseis::Type::UInt64}}};

    SECTION ("SrtRcvBackwards") {
        exseis::Trace_metadata trace_metadata(metadata_info, 200);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::source_x,
                1000.0
                    - (trace_metadata.size() * communicator.get_rank() + i)
                          / 20);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::source_y,
                1000.0
                    - (trace_metadata.size() * communicator.get_rank() + i)
                          % 20);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_x,
                1000.0
                    - (trace_metadata.size() * communicator.get_rank() + i)
                          / 10);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_y,
                1000.0
                    - (trace_metadata.size() * communicator.get_rank() + i)
                          % 10);
            trace_metadata.set_index(
                i, exseis::Trace_metadata_key::gtn,
                trace_metadata.size() * communicator.get_rank() + i);
        }
        auto list = exseis::sort(
            communicator, exseis::Sort_type::SrcRcv, trace_metadata);
        size_t global_list_size = communicator.sum(list.size());
        for (size_t i = 0; i < list.size(); i++) {
            REQUIRE(
                global_list_size
                    - (trace_metadata.size() * communicator.get_rank() + i) - 1
                == list[i]);
        }
    }

    SECTION ("SrcRcvForwards") {
        exseis::Trace_metadata trace_metadata(metadata_info, 200);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::source_x,
                1000.0
                    + (trace_metadata.size() * communicator.get_rank() + i)
                          / 20);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::source_y,
                1000.0
                    + (trace_metadata.size() * communicator.get_rank() + i)
                          % 20);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_x,
                1000.0
                    + (trace_metadata.size() * communicator.get_rank() + i)
                          / 10);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_y,
                1000.0
                    + (trace_metadata.size() * communicator.get_rank() + i)
                          % 10);
            trace_metadata.set_index(
                i, exseis::Trace_metadata_key::gtn,
                trace_metadata.size() * communicator.get_rank() + i);
        }
        auto list = exseis::sort(
            communicator, exseis::Sort_type::SrcRcv, trace_metadata);

        for (size_t i = 0; i < list.size(); i++) {
            REQUIRE(
                trace_metadata.size() * communicator.get_rank() + i == list[i]);
        }
    }

    SECTION ("SrcRcvRand") {
        exseis::Trace_metadata trace_metadata(metadata_info, 10);
        for (size_t i = 0; i < trace_metadata.size(); i++) {
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::source_y, 1000.0);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_x, 1000.0);
            trace_metadata.set_floating_point(
                i, exseis::Trace_metadata_key::receiver_y, 1000.0);
            trace_metadata.set_index(
                i, exseis::Trace_metadata_key::gtn,
                10 * communicator.get_rank() + i);
        }
        trace_metadata.set_floating_point(
            0, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 5.0);
        trace_metadata.set_floating_point(
            1, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 3.0);
        trace_metadata.set_floating_point(
            2, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 1.0);
        trace_metadata.set_floating_point(
            3, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 4.0);
        trace_metadata.set_floating_point(
            4, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 2.0);
        trace_metadata.set_floating_point(
            5, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 9.0);
        trace_metadata.set_floating_point(
            6, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 6.0);
        trace_metadata.set_floating_point(
            7, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 8.0);
        trace_metadata.set_floating_point(
            8, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 7.0);
        trace_metadata.set_floating_point(
            9, exseis::Trace_metadata_key::source_x,
            10.0 * communicator.get_rank() + 0.0);

        auto list = exseis::sort(
            communicator, exseis::Sort_type::SrcRcv, trace_metadata);

        REQUIRE(10 * communicator.get_rank() + 5U == list[0]);
        REQUIRE(10 * communicator.get_rank() + 3U == list[1]);
        REQUIRE(10 * communicator.get_rank() + 1U == list[2]);
        REQUIRE(10 * communicator.get_rank() + 4U == list[3]);
        REQUIRE(10 * communicator.get_rank() + 2U == list[4]);
        REQUIRE(10 * communicator.get_rank() + 9U == list[5]);
        REQUIRE(10 * communicator.get_rank() + 6U == list[6]);
        REQUIRE(10 * communicator.get_rank() + 8U == list[7]);
        REQUIRE(10 * communicator.get_rank() + 7U == list[8]);
        REQUIRE(10 * communicator.get_rank() + 0U == list[9]);
    }
}
