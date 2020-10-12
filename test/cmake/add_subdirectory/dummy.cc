#include <iostream>

#include <exseis/utils/encoding/character_encoding.hh>

int main()
{
    std::cout << "Hello, world!\n";

    bool is_printable = exseis::is_printable_ascii('a');
    std::cout << "Printable: " << is_printable << '\n';
}
