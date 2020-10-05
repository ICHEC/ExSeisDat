
ExSeisDat Examples    {#examples}
==================

Below are a number of tutorials on how to use ExSeisDat.

@internal
    @note The order these subpages are added is the order they'll appear in the
          side bar!
@endinternal

- @subpage examples_example1_cc
- @subpage examples_exampleCreateFile_cc


@internal

An Example Example
------------------

The examples in this directory are written in the following form:

~~~
/// Example: My Example    {#examples_my_example_cc}
/// ===================
///
/// The `{#ref}` command is used to generate a page in doxygen. In this case,
/// it's referenced by `example_my_example_cc` with the title "Example: My
/// Example".
///
/// For a file in the examples folder named `my_example.cc`, we'll give it
/// the reference `examples_my_example_cc`. The title, then, is what will appear
/// as the html title, and for any links referencing it, along with being the
/// main heading of the generated page.
/// Unfortunately, we can't use slashes or dots in the reference names,
/// because that confuses doxygen. So we'll simply replace those with `_`.
///
/// The reference should then be included in the list of subpages listed in
/// examples/README.md for it to fall under the "ExSeisDat Examples" subtree of
/// the table of contents. Otherwise it will appear as a standalone page.
///
/// In general, the file will start with some preamble about the program,
/// followed by includes etc.
///

#include <iostream>

///
/// maybe a few functions
///

// Don't forget to include some useful comments in the actual program!
// These non-doxygen-style comments will be left in the "clean" version
// of the code included at the end of the tutorial page.
void f(int i) {
    std::cout << "Hello " << i << "!" << std::endl;
}

///
/// and a `main` function, since we're writing full programs here as examples
///

int main() {

///
/// I prefer to include an extra "///" at the top and bottom, because it
/// more clearly delineates comments and code.
///
/// Some more info should be put here about how we're setting up calling the
/// function
///

    // of course, also comment the code we're documenting
    f(42);

///
/// and the return value
///

    return 0;
}

///
/// This file will then be processed into two files using
/// `doc/parse_examples.cmake`:
/// - path/to/build/doc/examples/documented/my_example.cc.md
/// - path/to/build/doc/examples/clean/my_example.cc
///
/// The "documented" version will have the "///" stripped out and "~~~" added
/// around the code sections.
/// the result will be a page that can be parsed by doxygen into a html page
/// as a tutorial.
///
/// The "clean" version will have all the "///" lines stripped out.
/// The result is the full code without extra documentation that can be included
/// at the end of the tutorial as the "full example" for clarity.
///
/// The "documented" folder is added to the doxygen `INPUTS` path, and the
/// "clean" folder is added to the `EXAMPLE_PATH` path.
/// The documented folder is processed by doxygen, and the clean folder can
/// be referred to in `@include` statements.
/// This is done in the "Complete Code" section at the end.
///
///
/// Complete Code
/// -------------
///
/// @include my_example.cc
~~~

Proper syntax highlighting should make the comments and code a bit clearer than
presented here.
Unfortunately, doxygen seems to strip the c-style doxygen comments from
c-highlighted comment blocks, so it couldn't be highlighted here.

@endinternal
