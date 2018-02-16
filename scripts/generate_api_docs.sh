#!/usr/bin/env bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source_dir="${script_dir}/.."
doc_dir="${source_dir}/doc"

cd "${source_dir}"
doxygen "${doc_dir}"/Doxyfile
