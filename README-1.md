# Mini File System — Complete Beginner Setup Guide

এই README এমনভাবে লেখা হয়েছে যেন তুমি **একদম শুরু থেকে Ubuntu/Linux-এ C++ Mini File System project বানাতে শিখছো**।

এই guide follow করলে তুমি শিখবে:

- Ubuntu project environment setup
- C++ compiler install
- Git install/configure
- GitHub SSH connection
- Project folder structure
- Basic C++ console application
- Makefile
- Git commit/push/pull
- Team branch workflow
- Mini File System project roadmap

---

# 1. Ubuntu Terminal খুলো

Keyboard:

```text
Ctrl + Alt + T
```

Terminal open হওয়ার পরে লিখো:

```bash
pwd
```

সাধারণত output হবে:

```text
/home/yourusername
```

---

# 2. Ubuntu Update

Terminal-এ:

```bash
sudo apt update
```

তারপর:

```bash
sudo apt upgrade -y
```

> Password চাইলে Ubuntu password লিখবে। Password type করার সময় screen-এ কিছু দেখা যাবে না — এটা normal।

---

# 3. C++ Compiler Install

আমরা project-টি **C++17** ব্যবহার করে করব।

Install:

```bash
sudo apt install build-essential g++ make -y
```

Compiler check:

```bash
g++ --version
```

Make check:

```bash
make --version
```

দুটো command কাজ করলে C++ environment ready।

---

# 4. Git Install

```bash
sudo apt install git -y
```

Check:

```bash
git --version
```

---

# 5. Git Name এবং Email Configure

নিজের নাম বসাবে:

```bash
git config --global user.name "Your Name"
```

নিজের GitHub email:

```bash
git config --global user.email "your-email@gmail.com"
```

Check:

```bash
git config --global --list
```

Output-এর মধ্যে এরকম থাকবে:

```text
user.name=Your Name
user.email=your-email@gmail.com
```

---

# 6. Existing SSH Key আছে কি না দেখো

```bash
ls -la ~/.ssh
```

যদি এরকম files থাকে:

```text
id_ed25519
id_ed25519.pub
```

তাহলে আগে থেকেই SSH key থাকতে পারে।

যদি না থাকে, নতুন key generate করো।

---

# 7. SSH Key Generate

```bash
ssh-keygen -t ed25519 -C "your-email@gmail.com"
```

তারপর দেখাবে:

```text
Enter file in which to save the key
```

শুধু:

```text
Enter
```

চাপো।

তারপর passphrase চাইতে পারে।

Learning/project environment হলে চাইলে empty রাখতে পারো:

```text
Enter
Enter
```

শেষে files হবে:

```text
~/.ssh/id_ed25519
~/.ssh/id_ed25519.pub
```

## Important

```text
id_ed25519.pub
```

হলো **Public Key**।

আর:

```text
id_ed25519
```

হলো **Private Key**।

**Private key কাউকে দেবে না।**

---

# 8. SSH Agent Start

```bash
eval "$(ssh-agent -s)"
```

তারপর:

```bash
ssh-add ~/.ssh/id_ed25519
```

---

# 9. Public SSH Key দেখো

```bash
cat ~/.ssh/id_ed25519.pub
```

একটা বড় line আসবে:

```text
ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAA........ your-email@gmail.com
```

পুরো line copy করো।

---

# 10. GitHub-এ SSH Key Add

GitHub-এ login করে যাও:

```text
Profile Picture
↓
Settings
↓
SSH and GPG keys
↓
New SSH key
```

Title:

```text
Ubuntu Laptop
```

Key Type:

```text
Authentication Key
```

Key field-এ copied public key paste করো।

তারপর:

```text
Add SSH key
```

---

# 11. GitHub SSH Connection Test

Ubuntu Terminal:

```bash
ssh -T git@github.com
```

প্রথমবার জিজ্ঞেস করতে পারে:

```text
Are you sure you want to continue connecting?
```

লিখবে:

```text
yes
```

Successful হলে এমন message আসবে:

```text
Hi YOUR_USERNAME! You've successfully authenticated...
```

এটার মানে:

```text
Ubuntu ↔ GitHub SSH connection successful
```

---

# 12. Projects Folder তৈরি

Home directory-তে যাও:

```bash
cd ~
```

Projects folder বানাও:

```bash
mkdir -p projects
```

Projects folder-এ যাও:

```bash
cd projects
```

Check:

```bash
pwd
```

Output:

```text
/home/yourusername/projects
```

---

# 13. Mini File System Project Folder তৈরি

```bash
mkdir MiniFileSystem
```

Project folder-এ যাও:

```bash
cd MiniFileSystem
```

Check:

```bash
pwd
```

Output হবে এরকম:

```text
/home/yourusername/projects/MiniFileSystem
```

এটাই তোমার **Project Root Folder**।

---

# 14. Git Initialize

Project root folder-এর ভিতরে:

```bash
git init
```

Main branch set করো:

```bash
git branch -M main
```

Check:

```bash
git status
```

---

# 15. Project Folders তৈরি

```bash
mkdir -p include src disk
```

Initial structure:

```text
MiniFileSystem/
├── include/
├── src/
└── disk/
```

---

# 16. Main Files তৈরি

```bash
touch main.cpp
```

```bash
touch Makefile
```

```bash
touch README.md
```

```bash
touch .gitignore
```

Disk folder GitHub-এ রাখার জন্য:

```bash
touch disk/.gitkeep
```

---

# 17. Header Files তৈরি

```bash
touch include/VirtualDisk.h
```

```bash
touch include/Inode.h
```

```bash
touch include/BlockManager.h
```

```bash
touch include/Directory.h
```

```bash
touch include/FileSystem.h
```

```bash
touch include/Shell.h
```

---

# 18. Source Files তৈরি

```bash
touch src/VirtualDisk.cpp
```

```bash
touch src/BlockManager.cpp
```

```bash
touch src/Directory.cpp
```

```bash
touch src/FileSystem.cpp
```

```bash
touch src/Shell.cpp
```

চাইলে:

```bash
touch src/Inode.cpp
```

---

# 19. Project Structure Check

`tree` install:

```bash
sudo apt install tree -y
```

তারপর:

```bash
tree
```

Expected structure:

```text
MiniFileSystem
├── Makefile
├── README.md
├── disk
│   └── .gitkeep
├── include
│   ├── BlockManager.h
│   ├── Directory.h
│   ├── FileSystem.h
│   ├── Inode.h
│   ├── Shell.h
│   └── VirtualDisk.h
├── main.cpp
└── src
    ├── BlockManager.cpp
    ├── Directory.cpp
    ├── FileSystem.cpp
    ├── Inode.cpp
    ├── Shell.cpp
    └── VirtualDisk.cpp
```

---

# 20. `.gitignore` তৈরি

Open:

```bash
nano .gitignore
```

এর ভিতরে লিখো:

```text
# Executable
minifs

# Object files
*.o

# Virtual disk files
disk/*.disk
disk/*.img

# VS Code
.vscode/

# Temporary files
*.tmp
*.log
```

Save:

```text
Ctrl + O
Enter
Ctrl + X
```

---

# 21. README File-এর Basic Project Information

README-তে project সম্পর্কে এই information রাখা ভালো:

```markdown
# Mini File System

A Mini File System project developed using C++ for Ubuntu/Linux.

## Features

- Virtual Disk
- File Creation
- File Reading
- File Writing
- File Deletion
- Directory Management
- Inode Management
- Block Allocation
- Linux System Calls
- Thread and Mutex

## Language

C++17

## Platform

Ubuntu / Linux
```

---

# 22. First `main.cpp` Program

Open:

```bash
nano main.cpp
```

এর ভিতরে লিখো:

```cpp
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string command;

    cout << "=====================================\n";
    cout << "        MINI FILE SYSTEM\n";
    cout << "=====================================\n";
    cout << "Type 'help' to see available commands.\n\n";

    while (true)
    {
        cout << "MiniFS:/> ";

        getline(cin, command);

        if (command == "help")
        {
            cout << "\nAvailable Commands:\n";
            cout << "---------------------------\n";
            cout << "help       Show commands\n";
            cout << "create     Create a file\n";
            cout << "read       Read a file\n";
            cout << "write      Write to a file\n";
            cout << "delete     Delete a file\n";
            cout << "mkdir      Create directory\n";
            cout << "ls         List directory\n";
            cout << "exit       Exit MiniFS\n\n";
        }
        else if (command == "exit")
        {
            cout << "Closing Mini File System...\n";
            break;
        }
        else if (command.empty())
        {
            continue;
        }
        else
        {
            cout << "Unknown command: " << command << "\n";
            cout << "Type 'help' for available commands.\n";
        }
    }

    return 0;
}
```

Save:

```text
Ctrl + O
Enter
Ctrl + X
```

---

# 23. First Compile

Project folder-এ আছো কিনা check:

```bash
pwd
```

তারপর compile:

```bash
g++ -std=c++17 main.cpp -o minifs
```

যদি কোনো error না আসে, compile successful।

Check:

```bash
ls
```

এখন:

```text
minifs
```

নামে executable দেখতে পাবে।

---

# 24. Program Run

```bash
./minifs
```

Expected output:

```text
=====================================
        MINI FILE SYSTEM
=====================================
Type 'help' to see available commands.

MiniFS:/>
```

তারপর লিখো:

```text
help
```

Program commands দেখাবে।

Exit:

```text
exit
```

---

# 25. Makefile কেন দরকার?

প্রতিবার বড় `g++` command না লিখে শুধু:

```bash
make
```

দিয়ে project compile করার জন্য Makefile ব্যবহার করব।

---

# 26. Makefile তৈরি

Open:

```bash
nano Makefile
```

এর ভিতরে লিখো:

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

TARGET = minifs

SOURCES = main.cpp \
          src/VirtualDisk.cpp \
          src/BlockManager.cpp \
          src/Directory.cpp \
          src/FileSystem.cpp \
          src/Shell.cpp

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
```

> `$(CXX)` এবং `rm` line-এর আগে **TAB** থাকবে।

Save:

```text
Ctrl + O
Enter
Ctrl + X
```

---

# 27. Makefile Test

আগের executable remove:

```bash
rm -f minifs
```

Compile:

```bash
make
```

Run:

```bash
./minifs
```

Clean:

```bash
make clean
```

আবার compile:

```bash
make
```

---

# 28. Git Status Check

```bash
git status
```

সব file Git staging area-তে add:

```bash
git add .
```

আবার check:

```bash
git status
```

---

# 29. First Git Commit

```bash
git commit -m "Initial Mini File System project setup"
```

Commit log:

```bash
git log --oneline
```

---

# 30. GitHub Repository তৈরি

GitHub-এ:

```text
+
↓
New repository
```

Repository name:

```text
MiniFileSystem
```

Description:

```text
Mini File System project using C++ and Linux system calls
```

Public বা Private select করো।

এই options tick করবে না:

```text
Add README
Add .gitignore
Add License
```

কারণ local project-এ এগুলো already আছে।

তারপর:

```text
Create repository
```

---

# 31. GitHub SSH Repository URL Copy

Repository page-এ:

```text
Code
↓
SSH
```

এরকম URL পাবে:

```text
git@github.com:YOUR_USERNAME/MiniFileSystem.git
```

---

# 32. Local Project-এর সাথে GitHub Connect

Project directory:

```bash
cd ~/projects/MiniFileSystem
```

Remote add:

```bash
git remote add origin git@github.com:YOUR_USERNAME/MiniFileSystem.git
```

Example:

```bash
git remote add origin git@github.com:somrat123/MiniFileSystem.git
```

Check:

```bash
git remote -v
```

---

# 33. First GitHub Push

Main branch নিশ্চিত করো:

```bash
git branch -M main
```

Push:

```bash
git push -u origin main
```

Successful হলে GitHub repository refresh করলে files দেখতে পাবে।

---

# 34. এরপর Code Change করলে GitHub Push

প্রথমে:

```bash
git status
```

তারপর:

```bash
git add .
```

Commit:

```bash
git commit -m "Update project"
```

Push:

```bash
git push
```

Workflow:

```text
Code Change
     ↓
git status
     ↓
git add .
     ↓
git commit -m "message"
     ↓
git push
```

---

# 35. GitHub থেকে Latest Code নেওয়া

Team member নতুন code push করলে:

```bash
git pull
```

অথবা:

```bash
git pull origin main
```

মনে রাখবে:

```text
git push = Computer → GitHub

git pull = GitHub → Computer
```

---

# 36. Team Project Branch Workflow

ধরো তুমি Virtual Disk feature নিয়ে কাজ করবে।

New branch:

```bash
git checkout -b feature/virtual-disk
```

Branches check:

```bash
git branch
```

Expected:

```text
* feature/virtual-disk
  main
```

Code change করার পরে:

```bash
git add .
```

Commit:

```bash
git commit -m "Implement virtual disk"
```

Branch push:

```bash
git push -u origin feature/virtual-disk
```

তারপর GitHub-এ Pull Request করে `main` branch-এ merge করা যাবে।

---

# 37. Project File Responsibilities

## `main.cpp`

Program start হবে।

```text
main()
↓
Shell Start
```

---

## `include/VirtualDisk.h`

`VirtualDisk` class declaration থাকবে।

Example:

```cpp
class VirtualDisk
{
public:
    bool createDisk();
    bool writeBlock();
    bool readBlock();
};
```

---

## `src/VirtualDisk.cpp`

Virtual disk-এর actual implementation থাকবে।

এখানে Linux system calls ব্যবহার হবে:

```text
open()
read()
write()
lseek()
close()
ftruncate()
```

---

## `include/Inode.h`

File/directory metadata থাকবে।

Example:

```cpp
struct Inode
{
    int inodeNumber;
    int size;
    bool used;
    bool isDirectory;
};
```

---

## `include/BlockManager.h`

কোন block free এবং কোন block used সেটা manage করবে।

```text
Free Block
Used Block
```

---

## `include/Directory.h`

Directory entries manage করবে।

Example:

```text
hello.txt → inode 5
Documents → inode 8
```

---

## `include/FileSystem.h`

Main filesystem controller।

এখানে functions থাকবে:

```cpp
createFile()
readFile()
writeFile()
deleteFile()
makeDirectory()
```

---

## `include/Shell.h`

User commands handle করবে।

Example:

```text
create hello.txt
read hello.txt
write hello.txt
ls
cd Documents
```

---

# 38. Final Project Architecture

```text
                   USER
                     │
                     ▼
                  Shell
                     │
                     ▼
                FileSystem
              /     |      \
             /      |       \
            ▼       ▼        ▼
       Directory   Inode    Block
        Manager   Manager  Manager
             \      |       /
              \     |      /
               VirtualDisk
                    │
                    ▼
          Linux System Calls
       open/read/write/lseek
```

Thread/Mutex:

```text
Thread 1 ─┐
Thread 2 ─┼── Mutex ── FileSystem
Thread 3 ─┘
```

---

# 39. এখন পর্যন্ত Project Progress

```text
[✅] Ubuntu Setup
[✅] C++ Compiler
[✅] Make
[✅] Git
[✅] GitHub SSH
[✅] Project Folder
[✅] Project Structure
[✅] Basic Console Application
[✅] Makefile
[✅] Local Git Repository
[✅] GitHub Repository
[✅] SSH Push

[ ] Virtual Disk
[ ] Linux System Calls
[ ] Superblock
[ ] Block Manager
[ ] Inode
[ ] Directory
[ ] Create File
[ ] Write File
[ ] Read File
[ ] Delete File
[ ] mkdir
[ ] ls
[ ] cd
[ ] pwd
[ ] stat
[ ] diskinfo
[ ] tree
[ ] Thread
[ ] Mutex
[ ] Testing
```

---

# 40. Full Mini File System Roadmap

আমরা এই order-এ project complete করব:

```text
STEP 1
Project Setup + Git + GitHub + Console
✅

STEP 2
Virtual Disk
↓

STEP 3
Linux System Calls
open()
read()
write()
lseek()
close()
ftruncate()
↓

STEP 4
Superblock
↓

STEP 5
Block Manager
↓

STEP 6
Inode
↓

STEP 7
File Create
↓

STEP 8
File Write
↓

STEP 9
File Read
↓

STEP 10
File Delete
↓

STEP 11
Directory System
↓

STEP 12
mkdir / cd / ls / pwd
↓

STEP 13
stat / diskinfo / tree
↓

STEP 14
Thread + Mutex
↓

STEP 15
Testing + Bug Fixing
↓

STEP 16
Final Presentation
```

---

# 41. Final Commands Cheat Sheet

## Project Folder

```bash
cd ~/projects/MiniFileSystem
```

## Compile

```bash
make
```

## Run

```bash
./minifs
```

## Clean

```bash
make clean
```

## Git Status

```bash
git status
```

## Add Changes

```bash
git add .
```

## Commit

```bash
git commit -m "Your commit message"
```

## Push

```bash
git push
```

## Pull

```bash
git pull
```

## Branch Check

```bash
git branch
```

## New Branch

```bash
git checkout -b feature/your-feature-name
```

---

# Next Step

এরপর আমাদের আসল Mini File System coding শুরু হবে:

```text
VirtualDisk.h
      ↓
VirtualDisk.cpp
      ↓
10 MB virtual disk
      ↓
512-byte blocks
      ↓
Linux system calls
      ↓
open()
write()
read()
lseek()
ftruncate()
close()
```

এই অংশ complete হওয়ার পরে আমরা **Superblock → Block Manager → Inode → File Operations → Directory → Thread/Mutex** একে একে implement করব।
