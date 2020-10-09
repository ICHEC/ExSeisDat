#ifndef EXSEISDAT_TEST_CATCH_REPORTER_MPI_HH
#define EXSEISDAT_TEST_CATCH_REPORTER_MPI_HH

#include "exseisdat/test/catch2.hh"

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <mpi.h>

namespace exseis {
namespace test {

class Catch_reporter_mpi : public Catch::IStreamingReporter {
    int m_rank     = 0;
    int m_num_rank = 0;

    std::stringstream m_console_reporter_stream{};
    std::ostream& m_stream = std::cout;
    Catch::ConsoleReporter m_console_reporter;

    using clock = std::chrono::high_resolution_clock;
    clock::time_point m_test_case_start;

    void print_with_prefix(
        const char* start, const char* end, const char* prefix)
    {
        while (start != end) {
            const char* pos = std::find(start, end, '\n');
            m_stream << prefix;
            if (pos == end) {
                m_stream.write(start, pos - start + 1);
                start = pos;
            }
            else {
                m_stream.write(start, pos - start + 1);
                start = pos + 1;
            }
        }
    }


    enum Collectivities { COLLECTIVE, NON_COLLECTIVE, SINGLE, NONE };

    template<Collectivities>
    struct print_tag {
    };

    void print(print_tag<COLLECTIVE> /*tag*/, const std::string& str)
    {
        int local_str_length = str.size();
        int str_length_max   = 0;
        MPI_Allreduce(
            &local_str_length, &str_length_max, 1, MPI_INT, MPI_MAX,
            MPI_COMM_WORLD);

        // No non-zero strings. Exit early.
        if (str_length_max == 0) return;

        std::vector<char> local_str(str_length_max);
        std::copy(str.begin(), str.end(), local_str.begin());

        std::vector<char> all_str;
        if (m_rank == 0) all_str.resize(m_num_rank * str_length_max);

        MPI_Gather(
            local_str.data(), local_str.size(), MPI_CHAR, all_str.data(),
            str_length_max, MPI_CHAR, 0, MPI_COMM_WORLD);

        if (m_rank == 0) {
            for (int i = 0; i < m_num_rank; i++) {
                const char* str = all_str.data() + i * str_length_max;
                const char* end = std::find(str, str + str_length_max, '\0');

                auto rank_str = std::string{"Rank "} + std::to_string(i) + ": ";
                print_with_prefix(str, end, rank_str.c_str());
            }
        }
    }

    void print(print_tag<NON_COLLECTIVE> /*tag*/, const std::string& str)
    {
        if (!str.empty()) {
            std::string rank_str = "Rank " + std::to_string(m_rank) + ": ";
            print_with_prefix(
                str.c_str(), str.c_str() + str.size() + 1, rank_str.c_str());
        }
    }

    void print(print_tag<SINGLE> /*tag*/, const std::string& str)
    {
        if (!str.empty() && m_rank == 0) {
            m_stream << str;
        }
    }

    void print(print_tag<NONE> /*tag*/, const std::string& /*str*/) {}


    template<
        Collectivities Collectivity,
        typename Return,
        typename Class,
        typename... Args,
        typename = std::enable_if_t<!std::is_same<Return, void>::value>>
    Return delegate(
        print_tag<Collectivity> /*tag*/,
        Return (Class::*f)(Args...),
        Args&&... args)
    {
        Return value    = (m_console_reporter.*f)(std::forward<Args>(args)...);
        std::string str = m_console_reporter_stream.str();
        m_console_reporter_stream.str("");
        print(print_tag<Collectivity>{}, str);

        return value;
    }

    template<Collectivities Collectivity, typename Class, typename... Args>
    void delegate(
        print_tag<Collectivity> /*tag*/,
        void (Class::*f)(Args...),
        Args&&... args)
    {
        (m_console_reporter.*f)(std::forward<Args>(args)...);
        std::string str = m_console_reporter_stream.str();
        m_console_reporter_stream.str("");
        print(print_tag<Collectivity>{}, str);
    }

  public:
    Catch_reporter_mpi(const Catch::ReporterConfig& config) :
        m_stream(config.stream()),
        m_console_reporter(Catch::ReporterConfig(
            config.fullConfig(), m_console_reporter_stream))
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &m_num_rank);
    }

    // NOLINTNEXTLINE
    static std::string getDescription()
    {
        return "An MPI-aware implementation of the ConsoleReporter.";
    }

    void noMatchingTestCases(std::string const& spec) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::noMatchingTestCases,
            spec);
    }

    void assertionStarting(
        Catch::AssertionInfo const& /*_assertionInfo*/) override
    {
        // delegate(print_tag<NON_COLLECTIVE>{},
        //    &Catch::ConsoleReporter::assertionStarting, _assertionInfo);
    }

    bool assertionEnded(Catch::AssertionStats const& assertion_stats) override
    {
        auto value = delegate(
            print_tag<NON_COLLECTIVE>{},
            &Catch::ConsoleReporter::assertionEnded, assertion_stats);

        if (!assertion_stats.assertionResult.isOk()) {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        return value;
    }

    struct Section_stats {
        clock::duration total_elapsed = std::chrono::seconds(0);
        size_t number_of_calls        = 0;
    };

    std::map<std::vector<std::string>, Section_stats> m_stats_map;

    std::vector<std::string> m_current_section_key;
    std::vector<clock::time_point> m_current_section_times;

    int section_count = 0;
    void sectionStarting(Catch::SectionInfo const& section_info) override
    {
        delegate(
            print_tag<NONE>{}, &Catch::ConsoleReporter::sectionStarting,
            section_info);

        m_current_section_key.emplace_back(section_info.name);
        m_current_section_times.emplace_back(clock::now());

        // if (section_count == 0) {
        //     print(print_tag<SINGLE>{}, "<");
        //     m_stream << std::flush;
        // }
        ++section_count;
    }

    void sectionEnded(Catch::SectionStats const& section_stats) override
    {
        delegate(
            print_tag<NONE>{}, &Catch::ConsoleReporter::sectionEnded,
            section_stats);

        // Get stats for the current section
        auto& stats = m_stats_map[m_current_section_key];

        // Get the elapsed time of the current section
        auto elapsed = clock::now() - m_current_section_times.back();

        // Update the stats
        stats.total_elapsed += elapsed;
        stats.number_of_calls += 1;

        // Clean up the keys and the time tracking
        m_current_section_key.pop_back();
        m_current_section_times.pop_back();

        --section_count;
        // if (section_count == 0) {
        //     print(print_tag<SINGLE>{}, ">");
        //     m_stream << std::flush;
        // }
        // delegate(print_tag<NONE>{},
        //    &Catch::ConsoleReporter::sectionEnded, _sectionStats);
    }

    void benchmarkStarting(Catch::BenchmarkInfo const& info) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::benchmarkStarting,
            info);
    }

    void benchmarkEnded(Catch::BenchmarkStats<> const& stats) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::benchmarkEnded,
            stats);
    }

    void testCaseStarting(Catch::TestCaseInfo const& test_case_info) override
    {
        delegate(
            print_tag<NONE>{}, &Catch::ConsoleReporter::testCaseStarting,
            test_case_info);
        if (m_rank == 0) {
            m_stream << "[" << test_case_info.name << "]" << std::endl;
        }

        m_test_case_start = clock::now();

        // Clear the section stats
        m_stats_map.clear();
        assert(m_current_section_key.empty());
        assert(m_current_section_times.empty());
    }

    void testCaseEnded(Catch::TestCaseStats const& test_case_stats) override
    {
        std::chrono::duration<double> seconds =
            clock::now() - m_test_case_start;

        delegate(
            print_tag<NONE>{}, &Catch::ConsoleReporter::testCaseEnded,
            test_case_stats);

        if (m_rank == 0) {
            // Print the section stats
            for (const auto& stats_kv : m_stats_map) {
                const auto& key   = stats_kv.first;
                const auto& stats = stats_kv.second;

                // The width of indentation and the section name
                constexpr size_t section_size = 38;
                // The width of the timing and call stats
                constexpr size_t stats_size = 42;

                size_t section_size_remaining = section_size;

                // Print indent
                constexpr size_t max_indents = 6;
                const size_t num_indents =
                    std::min<size_t>(2 * key.size(), max_indents);
                std::string indent(num_indents, ' ');
                m_stream << indent;
                section_size_remaining -= num_indents;

                // Print section name, truncated to max_section_name_width
                // characters
                const auto& section_name = key.back();
                const size_t max_section_name_width =
                    section_size_remaining - 2;
                auto truncated_section_name = section_name.substr(
                    0, std::min<size_t>(
                           max_section_name_width, section_name.size()));
                if (truncated_section_name.size() < section_name.size()) {
                    assert(
                        truncated_section_name.size()
                        == max_section_name_width);
                    truncated_section_name[max_section_name_width - 3] = '.';
                    truncated_section_name[max_section_name_width - 2] = '.';
                    truncated_section_name[max_section_name_width - 1] = '.';
                }
                else {
                    assert(
                        truncated_section_name.size()
                        <= max_section_name_width);
                    const size_t padding_size =
                        max_section_name_width - truncated_section_name.size();
                    std::string padding(padding_size, '.');
                    truncated_section_name += padding;
                }
                m_stream << truncated_section_name << ": ";

                // Print the time and call stats
                const std::chrono::duration<double> seconds =
                    stats.total_elapsed;

                std::array<char, stats_size + 1> stats_buffer;
                std::snprintf(
                    stats_buffer.data(), stats_buffer.size(),
                    "%8.2e s, %8.2e s/call, %5d calls\n", seconds.count(),
                    (seconds.count() / stats.number_of_calls),
                    static_cast<int>(stats.number_of_calls));
                stats_buffer.back() = '\0';
                m_stream << stats_buffer.data();
            }

            m_stream << "[" << test_case_stats.testInfo.name << "] - done "
                     << seconds.count() << " s" << std::endl;
        }
    }

    void testGroupStarting(Catch::GroupInfo const& group_info) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::testGroupStarting,
            group_info);
    }

    void testGroupEnded(Catch::TestGroupStats const& test_group_stats) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::testGroupEnded,
            test_group_stats);
    }

    void testRunEnded(Catch::TestRunStats const& test_run_stats) override
    {
        delegate(
            print_tag<COLLECTIVE>{}, &Catch::ConsoleReporter::testRunEnded,
            test_run_stats);
    }

    void testRunStarting(Catch::TestRunInfo const& test_run_info) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::testRunStarting,
            test_run_info);
    }

    Catch::ReporterPreferences getPreferences() const override
    {
        return m_console_reporter.getPreferences();
    }

    void skipTest(Catch::TestCaseInfo const& test_case_info) override
    {
        delegate(
            print_tag<SINGLE>{}, &Catch::ConsoleReporter::skipTest,
            test_case_info);
    }
};

}  // namespace test
}  // namespace exseis

#endif  // EXSEISDAT_TEST_CATCH_REPORTER_MPI_HH
