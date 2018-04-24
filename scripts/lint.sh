#!/usr/bin/env bash

if [[ $# -ne 2 ]]
then
    echo "usage: $(basename "$0") BUILD_DIRECTORY TARGET_FILE"

    exit 1
fi

build_dir="$1"
target_file="$2"


# Add clang-tidy and clang-apply-replacements to $PATH
case $(uname) in
    # On macOS, expect clang-tidy to be installed with HomeBrew
    # in the llvm package. This isn't added to the PATH by default because
    # of name clashes, but we're not worried about that here.
    Darwin)
        export PATH=$(brew --prefix llvm)/bin:$PATH
        ;;
esac


# Assume this file is in exseisdat/scripts.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir="$( cd "${script_dir}/.." && pwd)"

echo "Linting (clang-tidy) ${target_file}"
# cd into exseisdat project directory. 
# This ensures clang-tidy finds the .clang-tidy file.
pushd ${source_dir} >/dev/null
clang-tidy "${target_file}" \
    -p "${build_dir}" \
    --header-filter="${source_dir}" \
    --quiet
popd > /dev/null
