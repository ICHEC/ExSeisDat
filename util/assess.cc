#include "global.hh"
#include "file/filesegy.hh"
#include <iostream>
#include <glob.h>
#include <regex>
using namespace PIOL;
using namespace File;
int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few arguments\n";
        return -1;
    }
    Piol piol(new ExSeisPIOL);

    glob_t globs;
    std::cout << "Pattern: " << argv[1] << "\n";
    int err = glob(argv[1], GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err)
        return -1;

    std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);

    std::cout << "File Count: " << globs.gl_pathc << "\n";
    for (size_t i = 0; i < globs.gl_pathc; i++)
        if (std::regex_match(globs.gl_pathv[i], reg))
        {
            std::cout << "File: " << globs.gl_pathv[i] << "\n";
            std::unique_ptr<Interface> file = std::make_unique<SEGY>(piol, globs.gl_pathv[i]);
            if (piol->log->isErr())
                continue;
//    std::cout << "--\tText: " << file->readText() << "\n";
            std::cout << "-\tNs: " << file->readNs() << "\n";
            std::cout << "-\tNt: " << file->readNt() << "\n";
            std::cout << "-\tInc: " << file->readInc() << "\n";
        }
    globfree(&globs);
    return 0;
}
