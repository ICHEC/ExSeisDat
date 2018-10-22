#!/usr/bin/env bash

# Set number of processes to use
nprocs=1

# Exit on first error
set -o errexit


# Get the build directory from the command line
if [[ $# -ge 1 ]]
then
    # Get absolute path of build_dir from first argument.
    build_dir="$(cd "$1" && pwd)"
    shift
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

# Run lint.sh on every source file in ExSeisDat.
#
# Find all the .cc files in the project and run clang-tidy on them.
# Need to cd into source_dir because source_dirs are defined relative to that.
#
cd "${source_dir}"
find ${source_dirs} \( \
    -iname "*.cc" -o -iname "*.c" \
\) -print0 \
    | xargs -0 -n 1 -P "${nprocs}" -I TARGET_FILE "${script_dir}/lint.sh" "${build_dir}" TARGET_FILE "$@"
