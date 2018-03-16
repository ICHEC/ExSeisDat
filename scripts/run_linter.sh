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
source_dirs="examples include src systest test util"

build_dir=${PWD}/build

# Workaround a false-positive in googletest / googlemock
gmock_problem_file=$(find ${build_dir} -name gmock-spec-builders.h)
cp ${gmock_problem_file} ${gmock_problem_file}.bak
sed '1272 s#\( // NOLINT\)*$# // NOLINT#' "${gmock_problem_file}.bak" > "${gmock_problem_file}"

# Run clang-tidy
find ${source_dirs} -iname "*.cc" \
    -exec echo 'Linting (clang-tidy):' {} \; \
    -exec \
        clang-tidy  {} \
        -p ${build_dir} \;
