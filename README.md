# Mini File System (C++ / Ubuntu)

A teaching-oriented **Mini File System** implemented in **C++17** for **Ubuntu/Linux**.

It demonstrates:

- Virtual disk backed by `disk/mini_fs.disk`
- Linux system calls: `open`, `read`, `write`, `lseek`, `ftruncate`, `close`, `fsync`
- Superblock
- Inode table and inode bitmap
- Block bitmap and block allocation
- Hierarchical directories
- Persistent files/folders
- File create/read/write/append/delete
- Directory create/list/change/remove
- `stat` for **both files and folders**
- `diskinfo`
- `tree`
- rename/copy/move
- C++ threads + `std::mutex`
- OOP-style class separation

> This is an educational simulated filesystem. It does **not** modify Linux ext4.

---

## 1. Requirements

Ubuntu/Linux:

```bash
sudo apt update
sudo apt install build-essential g++ make git -y
```

---

## 2. Compile + Run

From the project folder, you only need:

```bash
make && ./minifs
```

Or:

```bash
make run
```

On first launch, MiniFS automatically creates a **10 MB virtual disk**.

---

## 3. Main Commands

```text
help
format
create <file>
write <file> <text>
append <file> <text>
read <file>
nani <file>
delete <file>
rm <file>

mkdir <folder>
rmdir <folder>
ls [path]
cd <path>
pwd

stat <file-or-folder>
stats <file-or-folder>   # alias
diskinfo
tree [path]

rename <path> <new-name>
copy <source-file> <destination-file>
move <source> <destination-directory>

threadtest
clear
exit
```

---

## 4. `stat` / `stats` Works for Both File and Folder

### File stat

```text
create report.txt
write report.txt Operating System Project
stat report.txt
```

Example:

```text
========== MiniFS STAT ==========
Path          : /report.txt
Name          : report.txt
Type          : Regular File
Inode Number  : 1
Parent Inode  : 0
Size          : 24 bytes
Data Blocks   : 1
Block IDs     : 128
=================================
```

### Folder stat

```text
mkdir Documents
stat Documents
```

Example:

```text
========== MiniFS STAT ==========
Path          : /Documents
Name          : Documents
Type          : Directory
Inode Number  : 2
Parent Inode  : 0
Child Count   : 0
Data Blocks   : 0
=================================
```

Current folder:

```text
stat .
```

Root folder:

```text
stat /
```

---

## 5. Quick Demo

Inside MiniFS:

```text
mkdir University
cd University

mkdir CSE
cd CSE

create os.txt
write os.txt This is my Operating System Mini File System project.

read os.txt
stat os.txt
stat .

diskinfo
tree /

cd /
threadtest
tree /
```

---

## 6. Persistence Test

Create data:

```text
create saved.txt
write saved.txt This data should survive restart.
exit
```

Run again:

```bash
./minifs
```

Then:

```text
ls
read saved.txt
stat saved.txt
```

The data remains because it is stored in:

```text
disk/mini_fs.disk
```

---

## 7. Architecture

```text
              USER
               |
               v
             Shell
               |
               v
           FileSystem
        /      |       \
       v       v        v
 Directory  Inodes    Blocks
       \       |        /
        \      |       /
          VirtualDisk
               |
               v
       Linux System Calls
 open/read/write/lseek/ftruncate
```

Thread safety:

```text
Thread 1 ----\
Thread 2 ----- > std::mutex --> FileSystem
Thread 3 ----/
```

---

## 8. Virtual Disk Layout

```text
+--------------------------------+
| Superblock                     |
+--------------------------------+
| Inode Bitmap                   |
+--------------------------------+
| Block Bitmap                   |
+--------------------------------+
| Inode Table                    |
+--------------------------------+
| Reserved Metadata Area         |
| blocks 0 - 127                 |
+--------------------------------+
| Data Blocks                    |
| block 128 ...                  |
+--------------------------------+
```

Configuration:

```text
Disk Size       = 10 MB
Block Size      = 512 bytes
Total Blocks    = 20,480
Data Start      = Block 128
Maximum Inodes  = 256
Direct Blocks   = 8 per file
Maximum File    = 4096 bytes
```

The small maximum file size is intentional for this educational project.

---

## 9. Project Structure

```text
MiniFileSystem/
├── include/
│   ├── Constants.h
│   ├── SuperBlock.h
│   ├── Inode.h
│   ├── InodeManager.h
│   ├── BlockManager.h
│   ├── Directory.h
│   ├── VirtualDisk.h
│   ├── FileSystem.h
│   └── Shell.h
│
├── src/
│   ├── InodeManager.cpp
│   ├── BlockManager.cpp
│   ├── Directory.cpp
│   ├── VirtualDisk.cpp
│   ├── FileSystem.cpp
│   └── Shell.cpp
│
├── disk/
│   └── .gitkeep
│
├── main.cpp
├── Makefile
├── README.md
└── .gitignore
```

---

## 10. Git / GitHub

First commit:

```bash
git add .
git commit -m "Complete Mini File System implementation"
```

If remote is already configured:

```bash
git push
```

If not:

```bash
git branch -M main
git remote add origin git@github.com:YOUR_USERNAME/MiniFileSystem.git
git push -u origin main
```

For a feature branch:

```bash
git checkout -b feature/complete-minifs
git add .
git commit -m "Implement virtual disk inodes blocks directories and mutex"
git push -u origin feature/complete-minifs
```

---

## 11. Useful Linux-side Commands

Compile and run in one command:

```bash
make && ./minifs
```

Clean executable:

```bash
make clean
```

Delete only the runtime virtual disk and start fresh:

```bash
make reset
```

Then:

```bash
make && ./minifs
```

---

## 12. Viva Talking Points

Be able to explain:

1. **Virtual Disk** — a normal Linux file acts as a simulated disk.
2. **Block** — data is stored in fixed 512-byte units.
3. **Block Bitmap** — tracks free/used blocks.
4. **Inode** — stores metadata, type, size, parent and block addresses.
5. **Inode Bitmap** — tracks free/used inodes.
6. **Directory** — directory relationships use inode parent/name information.
7. **Superblock** — stores overall filesystem information.
8. **Persistence** — metadata and file data are stored in `mini_fs.disk`.
9. **System Calls** — Linux `open/read/write/lseek/ftruncate/close`.
10. **Mutex** — protects shared filesystem state from concurrent thread races.

---

## 13. One Command You Will Use Most

After entering the project folder, easiest one-command start:

```bash
bash install_and_run.sh
```

Or, if dependencies are already installed:

```bash
make && ./minifs
```

Then inside MiniFS:

```text
help
```

That shows every supported command.
