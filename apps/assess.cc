////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details This utility searches for files matching a wildcard, filters out
///          the SEGY matches and provides details about what is in the files.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"

#include <glob.h>
#include <iostream>
#include <regex>

using namespace exseis::piol;

/*! Main function for assess.
 *  @param[in] argc The number of arguments. Should be at least 2.
 *  @param[in] argv The cstring array. The second array should be a globbing
 *                  pattern.
 *  @todo Switch globbing with regex
 *  @return Return 0 on success, -1 on failure
 */
int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Too few arguments\n";
        return -1;
    }
    auto piol = ExSeis::make();

    glob_t globs;
    std::cout << "Pattern: " << argv[1] << "\n";
    int err = glob(argv[1], GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err != 0) {
        return -1;
    }

    std::regex reg(
        ".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize
                        | std::regex::extended);

    std::cout << "File Count: " << globs.gl_pathc << "\n";
    for (size_t i = 0; i < globs.gl_pathc; i++) {
        if (std::regex_match(globs.gl_pathv[i], reg)) {
            std::cout << "File: " << globs.gl_pathv[i] << "\n";

            Input_file_segy file(piol, globs.gl_pathv[i]);
            piol->assert_ok();
            std::cout << "-\tNs: " << file.read_samples_per_trace() << "\n";
            std::cout << "-\tNt: " << file.read_number_of_traces() << "\n";
            std::cout << "-\tInc: " << file.read_sample_interval() << "\n";
            std::cerr << "-\tText: " << file.read_text() << "\n";
        }
    }

    globfree(&globs);
    return 0;
}
