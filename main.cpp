#include "FileSystem.h"
#include "Shell.h"

#include <iostream>

int main() {
    FileSystem fs;

    if (!fs.initialize()) {
        std::cerr << "Failed to initialize MiniFS.\n";
        return 1;
    }

    Shell shell(fs);
    shell.run();

    return 0;
}
