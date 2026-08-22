#ifndef INODE_MANAGER_H
#define INODE_MANAGER_H

#include "Constants.h"
#include "Inode.h"
#include "SuperBlock.h"

#include <array>
#include <cstdint>
#include <string>

class InodeManager {
private:
    std::array<Inode, MiniFSConfig::MAX_INODES>* inodes_;
    std::array<std::uint8_t, MiniFSConfig::MAX_INODES>* bitmap_;
    SuperBlock* superBlock_;

public:
    InodeManager(
        std::array<Inode, MiniFSConfig::MAX_INODES>& inodes,
        std::array<std::uint8_t, MiniFSConfig::MAX_INODES>& bitmap,
        SuperBlock& superBlock
    );

    void initialize();
    int allocate(const std::string& name, bool isDirectory, int parent);
    void freeInode(int inodeNumber);

    Inode* get(int inodeNumber);
    const Inode* get(int inodeNumber) const;
};

#endif
