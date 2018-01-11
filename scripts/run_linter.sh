#!/usr/bin/env bash

# Exit on first error
set -o errexit

# cd into exseisdat project directory. Assume this file is in exseisdat/scripts.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir=${script_dir}/..
cd ${source_dir}

# Add clang-tidy and clang-apply-replacements to $PATH
case $(uname) in
    Darwin)
        export PATH=$(brew --prefix llvm)/bin:$PATH
        ;;
esac

# All the directories containing source files
source_dirs="api examples include src systest test util"

build_dir=${PWD}/build

# Run clang-tidy
find ${source_dirs} -iname "*.cc" \
    | xargs -n 1 \
        clang-tidy -p ${build_dir}
