#!/ usr / bin / env bash

#
#Usage :./ compare_symbols.sh
#
#List all the symbols that differ between cwraptests and piolstub.
#missing : in piolstub and not in cwraptests
#different : in cwraptests and not in piolstub
#

#cd into this script directory
cd "$(dirname " $
{
    BASH_SOURCE[0]
}
")"


#Parse piol::*symbols from bin / cwraptests executable
#Pipeline:
#nm - get all symbols
#| grep PIOL - get symbols defined by PIOL
#| grep - v _gcov _asan - strip debugging symbols
#| grep ^[hex]{16 } ' - get only lines for symbols (i.e. addresses)
#| sed 's/^[address] [type]//' - strip address and symbol type info
#| gc ++ filt - demangle symbols
#| grep - v lambda - strip lambda function symbols
#| grep - v __func__ - strip function name symbols(from asserts)
#| grep - v vtable - strip vtable symbols
#| grep - v typeinfo - strip vtable symbols
  function
  PIOL_symbols()
{
    nm "$@" | grep '_ZN4PIOL' | grep - v '_gcov' | grep - v '_asan'
      | grep '^.\{16\} ' | sed 's/^.\{16\} [[:alpha:]] //' | gc++ filt - _
      | grep - v 'lambda(' | grep - v '::__func__' | grep - v '^vtable'
      | grep - v '^typeinfo'
}


#test_symbols piol_lib pattern
#Show the difference between cwraptests and piol_lib for the given pattern
function test_symbols()
{
    symbol_pattern = "$1"

      piol_tmp_file = $(mktemp) cwrap_tmp_file =
        $(mktemp)

#Get cwrap symbols
#Uniq needed for non - unique output from c++ filt, i.e.from multiple
#types of constructors(i.e.C1, C2, C3).
            PIOL_symbols..
          / bin / wraptests.so
        | grep "${symbol_pattern}" | sort | uniq > $
    {
        cwrap_tmp_file
    }

#Get ipiol symbols
    PIOL_symbols../ bin / piolstub.so | grep "${symbol_pattern}" | sort
      | uniq > ${piol_tmp_file}

          diff ${piol_tmp_file} ${cwrap_tmp_file}
      | sed 's/^</missing:  /'
      | sed 's/^>/different:/'

          rm
          - f ${cwrap_tmp_file} $
    {
        piol_tmp_file
    }
}


#Prm.*(for the param_utils::cpyPrm etc.free functions.
for
ns in "::ExSeis::"
      "::ReadInterface::"
      "::WriteInterface::"
      "::Trace_metadata::"
      "Prm.*("
      "::ReadModel::"
      "::Set::" do echo echo echo "=" echo "= ${ns}" echo "=" echo test_symbols
                                  "${ns}"
  | sed 's/^\([[:alnum:]]*:\).*[^,] piol::/\1 piol::/' done
