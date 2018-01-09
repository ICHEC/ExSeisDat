#!/usr/bin/env bash

# Exit on first error
set -o errexit

# The clang-format executable.
: ${CLANG_FORMAT_EXECUTABLE:=$(which clang-format)}

# Assume current script is in exseisdat/scripts
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir=${script_dir}/..

cd ${source_dir}

# All the directories containing source files
source_dirs="api examples include src systest test util"

# All the extensions of source files
source_exts="cc c hh h"

for dir in $source_dirs
do
    for ext in $source_exts
    do
        # Run clang-format on all the files with the given extension in each
        # directory
        find $dir -iname "*.${ext}" \
            -exec printf "\nFormatting {}\n" \; \
            -exec ${CLANG_FORMAT_EXECUTABLE} -style=file -i {} \;
    done
done
