#!/usr/bin/env bash


# Exit on first error
set -o errexit


# Get the build directory from the command line
if [[ $# == 1 ]]
then
    # Get absolute path of build_dir from first argument.
    build_dir="$(cd "$1" && pwd)"
    echo
    echo "BUILD_DIRECTORY: ${build_dir}"
    echo
else
    echo
    echo "Error: wrong number of arguments"
    echo
    echo "usage: $(basename $0) BUILD_DIRECTORY"
    echo

    exit 1
fi


script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir="$( cd "${script_dir}/.." && pwd)"

# All the directories containing source files
source_dirs="examples include src test util"

# Workaround a false-positive in googletest / googlemock
#
# Add "// NOLINT" to the end of the offending line.
# Also try to delete any " // NOLINT" that's already there, so we don't
# end up with " // NOLINT // NOLINT // ..." from running this script multiple
# times.
gmock_problem_file=$(find ${build_dir} -name gmock-spec-builders.h)

# We use cp and sed because the inline options for sed are different on
# macOS and Linux.
cp ${gmock_problem_file} ${gmock_problem_file}.bak
sed '1272 s#\( // NOLINT\)*$# // NOLINT#' "${gmock_problem_file}.bak" \
    > "${gmock_problem_file}"
rm -f "${gmock_problem_file}.bak"

# Run lint.sh on every source file in ExSeisDat.
#
# Find all the .cc files in the project and run clang-tidy on them.
# Need to cd into source_dir because source_dirs are defined relative to that.
#
nprocs=20
cd "${source_dir}"
find ${source_dirs} \( \
    -iname "*.cc" -o -iname "*.c" \
\) -print0 \
    | xargs -0 -n 1 -P "${nprocs}" "${script_dir}/lint.sh" "${build_dir}"
