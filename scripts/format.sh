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

# Make sure tmpfile is cleaned up on exit
trap "rm -f \"${tmpfile}\"" EXIT

# Run clang format over file with changes, write to tmpfile
cat "${file}" \
    | sed 's|#pragma omp|//@#pragma omp|' \
    | ${CLANG_FORMAT_EXECUTABLE} -style=file \
    | sed 's|//@#pragma omp|#pragma omp|' \
    > "${tmpfile}"

if [ ! -s "${tmpfile}" ]
then
    echo "Error! Formatted file is empty!"
    exit 1
fi


# Swap file for tmpfile, but only if there's a change
set +o errexit
if cmp --quiet "${tmpfile}" "${file}"
then
    set -o errexit
    echo "Formatting ${file}: No Change..."
    rm "${tmpfile}"
else
    set -o errexit
    echo "Formatting ${file}: Updating..."
    cat "${tmpfile}" > "${file}"
fi
