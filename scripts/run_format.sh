#!/usr/bin/env bash

# Exit on first error
set -o errexit

# Assume current script is in exseisdat/scripts
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir=${script_dir}/..

# Get the format script
FORMAT_EXECUTABLE="${script_dir}/format.sh"

cd ${source_dir}

# All the directories containing source files
source_dirs="examples include src systest test util"

# All the extensions of source files
source_exts="cc c hh h"

# Number of processes to run in parallel
nprocs=20

for dir in $source_dirs
do
    for ext in $source_exts
    do
        # Run clang-format on all the files with the given extension in each
        # directory
        if [ ! -z "$(find $dir -iname "*.${ext}")" ]
        then
            find $dir -iname "*.${ext}" -print0 \
                | xargs -n 1 -0 -P "${nprocs}" ${FORMAT_EXECUTABLE}
        fi
    done
done
