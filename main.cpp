#include "FileSystem.h"
#include "Shell.h"
#include "WebServer.h"

#include <iostream>

int main() {
    FileSystem fs;

    if (!fs.initialize()) {
        std::cerr << "Failed to initialize MiniFS.\n";
        return 1;
    }

    WebServer webServer(fs);
    webServer.start(8080);
    std::cout << "Web GUI is running at http://localhost:8080\n";

    Shell shell(fs);
    shell.run();

    webServer.stop();

    return 0;
}
