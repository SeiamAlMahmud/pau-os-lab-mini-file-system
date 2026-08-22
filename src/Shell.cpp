#include "Shell.h"

#include <iostream>
#include <sstream>
#include <string>

Shell::Shell(FileSystem& fs)
    : fs_(fs) {}

void Shell::printHelp() const {
    std::cout << R"HELP(

================ MiniFS Commands ================

help
    Show this help.

format
    Reset/reformat the virtual filesystem.
    WARNING: removes all MiniFS files/folders.

create <file>
    Create an empty file.
    Example: create notes.txt

write <file> <text>
    Replace file content.
    Example: write notes.txt Hello MiniFS

append <file> <text>
    Add text to the end of a file.
    Example: append notes.txt More text

read <file>
    Read file content.
    Example: read notes.txt

delete <file>
rm <file>
    Delete a file.

mkdir <directory>
    Create a directory.
    Example: mkdir Documents

rmdir <directory>
    Remove an EMPTY directory.

ls [path]
    List files and folders.
    Example: ls
    Example: ls /Documents

cd <path>
    Change directory.
    Example: cd Documents
    Example: cd ..
    Example: cd /

pwd
    Show current virtual path.

stat <file-or-folder>
stats <file-or-folder>
    Show inode/stat information for a file OR folder.
    Example: stat notes.txt
    Example: stat Documents
    Example: stat .
    Example: stat /

diskinfo
    Show virtual disk, block and inode usage.

tree [path]
    Show directory tree.
    Example: tree
    Example: tree /

rename <path> <new-name>
    Rename a file or folder.
    Example: rename notes.txt final.txt

copy <source-file> <destination-file>
    Copy a file.
    Example: copy final.txt backup.txt

move <source> <destination-directory>
    Move a file/folder.
    Example: move backup.txt Documents

threadtest
    Run a demo using C++ threads + std::mutex.

clear
    Clear terminal.

exit
    Exit MiniFS.

=================================================

)HELP";
}

void Shell::run() {
    std::string input;

    std::cout << "\n=====================================\n";
    std::cout << "        MINI FILE SYSTEM\n";
    std::cout << "=====================================\n";
    std::cout << "C++17 | Linux syscalls | Inodes | Blocks | Threads/Mutex\n";
    std::cout << "Type 'help' to see commands.\n\n";

    while (true) {
        std::cout << "MiniFS:" << fs_.promptPath() << "> ";

        if (!std::getline(std::cin, input)) {
            std::cout << "\n";
            break;
        }

        if (input.empty()) {
            continue;
        }

        std::stringstream ss(input);
        std::string command;
        ss >> command;

        if (command == "help") {
            printHelp();
        }
        else if (command == "format") {
            std::string answer;
            std::cout << "This will erase MiniFS data. Type YES to continue: ";
            std::getline(std::cin, answer);

            if (answer == "YES") {
                fs_.format();
            } else {
                std::cout << "Format cancelled.\n";
            }
        }
        else if (command == "create") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: create <file>\n";
            } else {
                fs_.createFile(path);
            }
        }
        else if (command == "write" || command == "append") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: "
                          << command
                          << " <file> <text>\n";
                continue;
            }

            std::string text;
            std::getline(ss, text);

            if (!text.empty() && text.front() == ' ') {
                text.erase(0, 1);
            }

            if (text.empty()) {
                std::cout << "Enter text: ";
                std::getline(std::cin, text);
            }

            fs_.writeFile(
                path,
                text,
                command == "append"
            );
        }
        else if (command == "read") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: read <file>\n";
            } else {
                fs_.readFile(path);
            }
        }
        else if (command == "delete" || command == "rm") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: delete <file>\n";
            } else {
                fs_.deleteFile(path);
            }
        }
        else if (command == "mkdir") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: mkdir <directory>\n";
            } else {
                fs_.makeDirectory(path);
            }
        }
        else if (command == "rmdir") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: rmdir <directory>\n";
            } else {
                fs_.removeDirectory(path);
            }
        }
        else if (command == "ls") {
            std::string path;
            ss >> path;
            fs_.listDirectory(path);
        }
        else if (command == "cd") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: cd <directory>\n";
            } else {
                fs_.changeDirectory(path);
            }
        }
        else if (command == "pwd") {
            fs_.printWorkingDirectory();
        }
        else if (command == "stat" || command == "stats") {
            std::string path;
            ss >> path;

            if (path.empty()) {
                std::cout << "Usage: stat <file-or-folder>\n";
            } else {
                fs_.statPath(path);
            }
        }
        else if (command == "diskinfo") {
            fs_.diskInfo();
        }
        else if (command == "tree") {
            std::string path;
            ss >> path;
            fs_.tree(path);
        }
        else if (command == "rename") {
            std::string oldPath;
            std::string newName;
            ss >> oldPath >> newName;

            if (oldPath.empty() || newName.empty()) {
                std::cout << "Usage: rename <path> <new-name>\n";
            } else {
                fs_.renamePath(oldPath, newName);
            }
        }
        else if (command == "copy") {
            std::string source;
            std::string destination;
            ss >> source >> destination;

            if (source.empty() || destination.empty()) {
                std::cout << "Usage: copy <source-file> <destination-file>\n";
            } else {
                fs_.copyFile(source, destination);
            }
        }
        else if (command == "move") {
            std::string source;
            std::string destination;
            ss >> source >> destination;

            if (source.empty() || destination.empty()) {
                std::cout << "Usage: move <source> <destination-directory>\n";
            } else {
                fs_.movePath(source, destination);
            }
        }
        else if (command == "threadtest") {
            fs_.threadTest();
        }
        else if (command == "clear") {
            std::cout << "\033[2J\033[H";
        }
        else if (command == "exit") {
            std::cout << "Closing Mini File System...\n";
            break;
        }
        else {
            std::cout << "Unknown command: " << command
                      << "\nType 'help' to see available commands.\n";
        }
    }
}
