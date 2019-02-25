#include "exseisdat/utils/signal_processing/Taper_function.hh"
#include "exseisdat/utils/signal_processing/mute.hh"
#include "exseisdat/utils/typedefs.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cmath>
#include <numeric>
#include <string>
#include <vector>


using namespace exseis::utils;


/// Creates a uniform signal with fixed value at any [i] location = 1
/// and a signal length of 250
///
std::vector<Trace_value> uniform_signal()
{

    std::vector<Trace_value> signal(250);

    for (size_t i = 0; i < signal.size(); i++) {
        signal[i] = 1;
    }

    return signal;
}


/// Creates a non-uniform signal, the cos function and applies it to the
/// signal[i]
///
std::vector<Trace_value> non_uniform_signal()
{

    std::vector<Trace_value> signal(250);

    for (size_t i = 0; i < signal.size(); i++) {
        signal[i] = std::cos(i);
    }
    return signal;
}


/// The following test loop through signal types,taper functions and
/// mute and taper lengths
///
TEST(Mute, Mute)
{
    for (auto signal_function : {uniform_signal, non_uniform_signal}) {
        for (auto taper_function :
             {linear_taper, cosine_taper, cosine_square_taper}) {
            for (size_t mute_size_at_begin : {0U, 10U}) {
                for (size_t taper_size_at_begin : {0U, 30U}) {
                    for (size_t taper_size_at_end : {0U, 30U}) {
                        for (size_t mute_size_at_end : {0U, 10U}) {

                            auto signal          = signal_function();
                            auto original_signal = signal_function();

                            mute(
                                signal.size(), signal.data(), taper_function,
                                mute_size_at_begin, taper_size_at_begin,
                                taper_size_at_end, mute_size_at_end);

                            for (size_t i = 0; i < signal.size(); i++) {
                                if (i < mute_size_at_begin) {
                                    EXPECT_FLOAT_EQ(signal[i], 0);
                                    continue;
                                }

                                if (i >= mute_size_at_begin
                                    && i < mute_size_at_begin
                                               + taper_size_at_begin) {
                                    EXPECT_FLOAT_EQ(
                                        signal[i],
                                        original_signal[i]
                                            * taper_function(
                                                i - mute_size_at_begin,
                                                taper_size_at_begin));

                                    continue;
                                }

                                if (i >= mute_size_at_begin
                                             + taper_size_at_begin
                                    && i < signal.size() - taper_size_at_end
                                               - mute_size_at_end) {
                                    EXPECT_FLOAT_EQ(
                                        signal[i], original_signal[i]);

                                    continue;
                                }

                                if (i >= signal.size() - mute_size_at_end
                                             - taper_size_at_end
                                    && i < signal.size() - mute_size_at_end) {
                                    EXPECT_FLOAT_EQ(
                                        signal[i],
                                        original_signal[i]
                                            * taper_function(
                                                signal.size() - (i + 1)
                                                    - mute_size_at_end,
                                                taper_size_at_end));

                                    continue;
                                }

                                if (i >= signal.size() - mute_size_at_end
                                    && i < signal.size()) {
                                    EXPECT_FLOAT_EQ(signal[i], 0);

                                    continue;
                                }
                            }
                        }  // for mute_size_at_end
                    }      // for taper_size_at_end
                }          // for taper_size_at_begin
            }              // for mute_size_at_begin
        }                  // for taper_function
    }                      // for signal_function
}
