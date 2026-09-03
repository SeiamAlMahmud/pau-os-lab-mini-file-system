#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

const int TOTAL_BLOCKS = 20;

struct Inode {
    int id;
    string name;
    string data;
    int block;
};

struct Directory {
    string name;
    Directory* parent;
    vector<Inode> files;
    vector<Directory*> folders;

    Directory(string n, Directory* p = nullptr) {
        name = n;
        parent = p;
    }
};

class MiniFileSystem {
private:
    Directory* root;
    Directory* current;
    bool used[TOTAL_BLOCKS] = {};
    int nextInode = 1;

    Inode* findFile(string name) {
        for (auto &f : current->files)
            if (f.name == name) return &f;
        return nullptr;
    }

    Directory* findFolder(string name) {
        for (auto d : current->folders)
            if (d->name == name) return d;
        return nullptr;
    }

    int freeBlock() {
        for (int i = 0; i < TOTAL_BLOCKS; i++)
            if (!used[i]) return i;
        return -1;
    }

public:
    MiniFileSystem() {
        root = new Directory("root");
        current = root;
    }

    void createFile(string name) {
        if (name.empty()) {
            cout << "Enter a file name.\n";
            return;
        }
        if (findFile(name)) {
            cout << "File already exists.\n";
            return;
        }

        int block = freeBlock();
        if (block == -1) {
            cout << "Disk is full.\n";
            return;
        }

        used[block] = true;
        current->files.push_back({nextInode++, name, "", block});
        cout << "File created.\n";
    }

    void writeFile(string name) {
        Inode* f = findFile(name);
        if (!f) {
            cout << "File not found.\n";
            return;
        }

        cout << "Enter text: ";
        string text;
        getline(cin, text);
        f->data = text;
        cout << "File written successfully.\n";
    }

    void readFile(string name) {
        Inode* f = findFile(name);
        if (!f) {
            cout << "File not found.\n";
            return;
        }
        cout << f->data << "\n";
    }

    void deleteFile(string name) {
        for (auto it = current->files.begin(); it != current->files.end(); ++it) {
            if (it->name == name) {
                used[it->block] = false;
                current->files.erase(it);
                cout << "File deleted.\n";
                return;
            }
        }
        cout << "File not found.\n";
    }

    void makeDirectory(string name) {
        if (name.empty() || findFolder(name)) {
            cout << "Invalid or existing directory name.\n";
            return;
        }
        current->folders.push_back(new Directory(name, current));
        cout << "Directory created.\n";
    }

    void changeDirectory(string name) {
        if (name == "..") {
            if (current->parent) current = current->parent;
            return;
        }

        Directory* d = findFolder(name);
        if (d) current = d;
        else cout << "Directory not found.\n";
    }

    void list() {
        cout << "Directories:\n";
        for (auto d : current->folders)
            cout << "  [DIR] " << d->name << "\n";

        cout << "Files:\n";
        for (auto &f : current->files)
            cout << "  " << f.name << "\n";
    }

    void pwd() {
        if (current == root) {
            cout << "/\n";
            return;
        }

        vector<string> path;
        Directory* d = current;
        while (d != root) {
            path.push_back(d->name);
            d = d->parent;
        }

        for (auto it = path.rbegin(); it != path.rend(); ++it)
            cout << "/" << *it;
        cout << "\n";
    }

    void statFile(string name) {
        Inode* f = findFile(name);
        if (!f) {
            cout << "File not found.\n";
            return;
        }

        cout << "File Name : " << f->name << "\n";
        cout << "Inode     : " << f->id << "\n";
        cout << "Size      : " << f->data.size() << " bytes\n";
        cout << "Block     : " << f->block << "\n";
    }

    void help() {
        cout << "\nCommands:\n"
             << " create <file>  - Create a file\n"
             << " write <file>   - Write to a file\n"
             << " read <file>    - Read a file\n"
             << " delete <file>  - Delete a file\n"
             << " mkdir <dir>    - Create directory\n"
             << " cd <dir>       - Enter directory\n"
             << " cd ..          - Go to parent directory\n"
             << " ls             - List files/directories\n"
             << " pwd            - Show current path\n"
             << " stat <file>    - Show inode information\n"
             << " help           - Show commands\n"
             << " exit           - Exit program\n\n";
    }

    void run() {
        cout << "=== Mini File System ===\n";
        cout << "Type 'help' to see commands.\n\n";

        string line, command, name;

        while (true) {
            cout << "miniFS> ";
            getline(cin, line);

            stringstream ss(line);
            ss >> command >> name;

            if (command == "create") createFile(name);
            else if (command == "write") writeFile(name);
            else if (command == "read") readFile(name);
            else if (command == "delete" || command == "rm") deleteFile(name);
            else if (command == "mkdir") makeDirectory(name);
            else if (command == "cd") changeDirectory(name);
            else if (command == "ls") list();
            else if (command == "pwd") pwd();
            else if (command == "stat") statFile(name);
            else if (command == "help") help();
            else if (command == "exit") break;
            else if (!command.empty()) cout << "Unknown command. Type 'help'.\n";
        }

        cout << "Mini File System closed.\n";
    }
};

int main() {
    MiniFileSystem fs;
    fs.run();
    return 0;
}
