#!/usr/bin/env bash

# Exit on first error
set -o errexit

# Assume current script is in exseisdat/scripts
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir="$(cd "${script_dir}/.." && pwd)"

dockerize=false
install_deps=false

while [[ $# -ne 0 ]]
do
    case $1 in
    --dockerize)
        dockerize=true
        shift
        ;;

    --install-deps)
        install_deps=true
        shift
        ;;

    *)
        echo &>2 "Unknown option: $1"
        ;;
    esac
done

if [[ "${dockerize}" = "true" ]]
then
    echo "Dockerizing..."
    exec docker run -v "${source_dir}":/mnt -it ubuntu:20.04 \
        bash -o verbose -c "cd /mnt && ./scripts/run_format.sh --install-deps"
fi

if [[ "${install_deps}" = "true" ]]
then
    apt-get -y update
    apt-get -y install clang-format-9

    export CLANG_FORMAT_EXECUTABLE="$(which clang-format-9)"
fi

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
