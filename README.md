# Mini File System

A small C++ OS Lab project that demonstrates basic file-system concepts.

## Main OS Concepts
- File allocation
- Inodes
- Directories

## Features
- Create files
- Write files
- Read files
- Delete files
- Create directories
- Change directories
- List files/directories
- Show current path
- `stat <file>` shows inode, size and allocated block

## Build and Run

### Linux
```bash
make
./mini_fs
```

Or:
```bash
g++ -std=c++17 main.cpp -o mini_fs
./mini_fs
```

## Commands
```text
create file.txt
write file.txt
read file.txt
stat file.txt
mkdir docs
cd docs
cd ..
ls
pwd
delete file.txt
help
exit
```

## Note
This is an educational in-memory simulation. It uses 20 simple allocation blocks.
Data is not persisted after the program exits.
