#ifndef SHELL_H
#define SHELL_H

#include "FileSystem.h"

class Shell {
private:
    FileSystem& fs_;

    void printHelp() const;

public:
    explicit Shell(FileSystem& fs);
    void run();
};

#endif
