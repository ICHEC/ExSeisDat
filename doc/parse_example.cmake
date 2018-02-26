# CMake -P script

#
# This is a script for parsing the examples into a separate doxygen commented
# file and an uncommented file.
#
# The example files are in the following format (using Markdown for markup):
# example.c:
#   /// @page my_example_c My Example: example.c
#   ///
#   /// My Example
#   /// ==========
#   ///
#   /// The example should start with the "@page" tag to create a new page
#   /// for the example!
#   ///
#   /// Some text here. Generally in Markdown. Then a description of the code,
#   /// followed by the actual code, e.g. the includes and main function
#   /// 
#
#   #include <whatever>
#
#   int main()
#   {
#
#       ///
#       /// and then we continue with our document.
#       ///
#
#       // You should still include useful comments in the code, because
#       // all the surrounding documentation will be stripped out in the
#       // "Complete Code" section, where the undocumented file is presented!
#       printf("Hello world!\n");
#
#       ///
#       /// Note that the text is indented with the code.
#       /// This is because clang-format will indent the comments to this level
#       /// during formatting.
#       ///
#
#       return 0;
#   }
#
#   ///
#   /// Finally, we print out the complete code listing with the documentation
#   /// stripped out.
#   ///
#   /// This script should output the documented version of the file somewhere
#   /// on Doxygen's INPUT path, and the undocumented version on Doxygen's
#   /// EXAMPLE_PATH path.
#   ///
#   /// Complete Code
#   /// -------------
#   ///
#   /// @include example.c
#
# This file will be turned into two files when using this script: one with the
# "/// " at the start of lines stripped and the code blocks surrounded with
# @code tags, so Doxygen can parse the whole thing as a Doxygen page,
# and one with all the lines starting with "///" stripped out,
# so Doxygen can reference/include the full, undocumented file for clarity.
#


#
# The input variables.
#
# They're initialized to NOTFOUND so the if(NOT ...) picks up on them if they're
# not initialized, and the error message displays NOTFOUND for the unset
# variables.
#

set(
    INPUT_FILENAME "NOTFOUND" CACHE FILEPATH
    "Path to the input file"
)

set(
    OUTPUT_DOC_FILENAME "NOTFOUND" CACHE FILEPATH
    "Path to the output file for the documented version of the code"
)

set(
    OUTPUT_CLEAN_FILENAME "NOTFOUND" CACHE FILEPATH
    "Path to the output file for the code with the docs stripped out."
)

# If any input variables are unset ...
if(NOT INPUT_FILENAME OR NOT OUTPUT_DOC_FILENAME OR NOT OUTPUT_CLEAN_FILENAME)

    # Output a usage and error message.
    message(
        FATAL_ERROR
"
  Usage: cmake -P parse.cmake \\
             -DINPUT_FILE=/path/to/input \\
             -DOUTPUT_DOC_FILENAME=/path/to/documented/output \\
             -DOUTPUT_CLEAN_FILENAME=/path/to/undocumented/output
  
  Arguments passed:
      INPUT_FILENAME: ${INPUT_FILENAME}
      OUTPUT_DOC_FILENAME:   ${OUTPUT_DOC_FILENAME}
      OUTPUT_CLEAN_FILENAME: ${OUTPUT_CLEAN_FILENAME}
"
    )
endif(NOT INPUT_FILENAME OR NOT OUTPUT_DOC_FILENAME OR NOT OUTPUT_CLEAN_FILENAME)


# Read the input file into the UNPROCESSED_FILE variable.
file(READ ${INPUT_FILENAME} UNPROCESSED_FILE)


#
# Process the file for the documented output into the DOC_FILE variable.
# Output it to the OUTPUT_DOC_FILENAME file.
#

# We want every code line to start with CODE and every doxygen line to
# start with ///

# Start by prefixing *every* line with "CODE"
# Added \n///\n at start and /// at end to ensure the file starts in with
# a doxygen block (and we match \n/// in this script for doxygen blocks.
string(
    REGEX REPLACE
    "\n" "\nCODE"
    PREFIXED_CODE
    "\n///\n${UNPROCESSED_FILE}///"
)

# Then replace lines starting with "CODE ///" with "///", because those
# were originally doxygen comments.
# This should five us all doxygen lines starting with "///" and all code
# lines starting with "CODE".
string(
    REGEX REPLACE
    "\nCODE *(/// ?|$)" "\n///"
    PREFIXED_DOXY
    "${PREFIXED_CODE}"
)

# Now we want to replace lines like
#   /// lorem ipsum
#   CODE ...
# with
#   /// lorem ipsum
#   /// @code{.c}
#   CODE // Code:
#   CODE ...
# to start a code block.
# the extra "// Code:" comment at the start of the line is to make sure
# doxygen doesn't strip the indentation.
string(
    REGEX REPLACE
    "\n///([^\n]*)\nCODE" "\n///\\1\n///@code{.c}\n// Code:\nCODE"
    ADDED_OPEN_FENCE
    "${PREFIXED_DOXY}"
)

# Then replace
#   CODE lorem(ipsum)
#   /// ...
# with
#   CODE lorem(ipsum)
#   /// @endcode
#   /// lorem ipsum
# to close the code block
string(
    REGEX REPLACE
    "\nCODE([^\n]*)\n///" "\nCODE\\1\n///@endcode\n///"
    ADDED_CLOSE_FENCE
    "${ADDED_OPEN_FENCE}"
)

# Replace CODE and /// with "" for the moment
string(REGEX REPLACE "\n///" "\n" FIXED_DOXY "${ADDED_CLOSE_FENCE}")

string(REGEX REPLACE "\nCODE" "\n" DOC_FILE "${FIXED_DOXY}")


# Output to disk (\n at end for UNIX compatibility)
file(WRITE ${OUTPUT_DOC_FILENAME} "${DOC_FILE}\n")


#
# Process the file for the undocumented output into the UNDOC_FILE variable.
# Output it to the OUTPUT_CLEAN_FILENAME file.
#

# Remove any lines starting with ///
#
# Remove text from lines starting with spaces and then ///, then merge
# any empty lines together.

# Replace /// lines with \n
string(
    REGEX REPLACE
    "\n *///[^\n]*" "\n"
    MERGE_DOC_COMMENTS
    "\n${UNPROCESSED_FILE}"
)

# Merge 3 or more \n into 2 \n
string(REGEX REPLACE "\n\n\n+" "\n\n" UNDOC_FILE "${MERGE_DOC_COMMENTS}")

# Output to disk (\n at end for UNIX compatibility)
file(WRITE ${OUTPUT_CLEAN_FILENAME} "${UNDOC_FILE}\n")
