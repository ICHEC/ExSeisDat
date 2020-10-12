#include "exseis/test/catch2.hh"

#include "exseis/utils/signal_processing/Taper_function.hh"
#include "exseis/utils/signal_processing/mute.hh"
#include "exseis/utils/types/typedefs.hh"

#include <cmath>
#include <numeric>
#include <string>
#include <vector>


/// Creates a uniform signal with fixed value at any [i] location = 1
/// and a signal length of 250
///
std::vector<exseis::Trace_value> uniform_signal()
{

    std::vector<exseis::Trace_value> signal(250);

    for (size_t i = 0; i < signal.size(); i++) {
        signal[i] = 1;
    }

    return signal;
}


/// Creates a non-uniform signal, the cos function and applies it to the
/// signal[i]
///
std::vector<exseis::Trace_value> non_uniform_signal()
{

    std::vector<exseis::Trace_value> signal(250);

    for (size_t i = 0; i < signal.size(); i++) {
        signal[i] = std::cos(i);
    }
    return signal;
}


/// The following test loops through signal types, taper functions and
/// mute and taper lengths
///
TEST_CASE("Mute", "[signal_processing][mute]")
{
    auto signal_function = GENERATE(
        as<std::function<decltype(uniform_signal)>>(), uniform_signal,
        non_uniform_signal);
    auto mute_size_at_begin  = GENERATE(0U, 10U);
    auto taper_size_at_begin = GENERATE(0U, 30U);
    auto taper_size_at_end   = GENERATE(0U, 30U);
    auto mute_size_at_end    = GENERATE(0U, 10U);

    for (auto taper_function : {exseis::linear_taper, exseis::cosine_taper,
                                exseis::cosine_square_taper}) {

        auto signal          = signal_function();
        auto original_signal = signal_function();

        exseis::mute(
            signal.size(), signal.data(), taper_function, mute_size_at_begin,
            taper_size_at_begin, mute_size_at_end, taper_size_at_end);

        {
            INFO("The signal should be muted up to mute_size_at_begin.");
            for (size_t i = 0; i < mute_size_at_begin; i++) {
                REQUIRE(signal[i] == Approx(0));
            }
        }

        {
            INFO(
                "The signal should be tapered from mute_size_at_begin to mute_size_at_begin + taper_size_at_begin.");
            for (size_t i = mute_size_at_begin;
                 i < mute_size_at_begin + taper_size_at_begin; i++) {
                auto tapered_signal =
                    original_signal[i]
                    * taper_function(
                        i - mute_size_at_begin, taper_size_at_begin);

                REQUIRE(signal[i] == Approx(tapered_signal));
            }
        }

        {
            INFO("The signal should be unchanged in the central region.");
            for (size_t i = mute_size_at_begin + taper_size_at_begin;
                 i < signal.size() - taper_size_at_end - mute_size_at_end;
                 i++) {
                REQUIRE(signal[i] == Approx(original_signal[i]));
            }
        }

        {
            INFO(
                "The signal should be tapered from (taper_size_at_end + mute_size_at_end) from the end.");
            for (size_t i =
                     signal.size() - mute_size_at_end - taper_size_at_end;
                 i < signal.size() - mute_size_at_end; i++) {
                auto tapered_signal =
                    original_signal[i]
                    * taper_function(
                        signal.size() - (i + 1) - mute_size_at_end,
                        taper_size_at_end);

                REQUIRE(signal[i] == Approx(tapered_signal));
            }
        }

        {
            INFO(
                "The signal should be muted from mute_size_at_end from the end until the end.");
            for (size_t i = signal.size() - mute_size_at_end; i < signal.size();
                 i++) {
                REQUIRE(signal[i] == Approx(0));
            }
        }
    }  // for taper_function
}
