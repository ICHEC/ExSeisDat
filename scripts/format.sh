#!/usr/bin/env bash

#
# Run clang format with a few modifications.
# In particular, we don't want pragma directives touched.
#

set -o errexit

# The clang-format executable.
: ${CLANG_FORMAT_EXECUTABLE:=$(which clang-format)}
export CLANG_FORMAT_EXECUTABLE

file="$1"
tmpfile="$(mktemp)"

trap "rm -f \"${tmpfile}\"" EXIT

# Run clang format over file with changes, write to tmpfile
cat "${file}" \
    | sed 's|#pragma omp|//@#pragma omp|' \
    | ${CLANG_FORMAT_EXECUTABLE} -style=file \
    | sed 's|//@#pragma omp|#pragma omp|' \
    > "${tmpfile}"

# Swap file for tmpfile
mv "${tmpfile}" "${file}"
