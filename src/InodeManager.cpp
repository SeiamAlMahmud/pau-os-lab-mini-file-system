#include "InodeManager.h"

#include <algorithm>
#include <cstring>
#include <ctime>

InodeManager::InodeManager(
    std::array<Inode, MiniFSConfig::MAX_INODES>& inodes,
    std::array<std::uint8_t, MiniFSConfig::MAX_INODES>& bitmap,
    SuperBlock& superBlock
) : inodes_(&inodes), bitmap_(&bitmap), superBlock_(&superBlock) {}

void InodeManager::initialize() {
    bitmap_->fill(0);

    for (auto& inode : *inodes_) {
        inode = Inode{};
        inode.id = -1;
    }

    superBlock_->freeInodes =
        static_cast<std::uint32_t>(MiniFSConfig::MAX_INODES);
}

int InodeManager::allocate(
    const std::string& name,
    bool isDirectory,
    int parent
) {
    for (std::size_t i = 0; i < MiniFSConfig::MAX_INODES; ++i) {
        if ((*bitmap_)[i] == 0) {
            (*bitmap_)[i] = 1;

            Inode inode {};
            inode.id = static_cast<int>(i);
            inode.used = 1;
            inode.isDirectory = isDirectory ? 1 : 0;
            inode.parent = parent;
            inode.size = 0;
            inode.blockCount = 0;

            std::fill(
                std::begin(inode.blocks),
                std::end(inode.blocks),
                -1
            );

            std::strncpy(
                inode.name,
                name.c_str(),
                MiniFSConfig::MAX_NAME - 1
            );
            inode.name[MiniFSConfig::MAX_NAME - 1] = '\0';

            inode.createdAt = static_cast<std::int64_t>(std::time(nullptr));
            inode.modifiedAt = inode.createdAt;

            (*inodes_)[i] = inode;

            if (superBlock_->freeInodes > 0) {
                --superBlock_->freeInodes;
            }

            return static_cast<int>(i);
        }
    }

    return -1;
}

void InodeManager::freeInode(int inodeNumber) {
    if (inodeNumber < 0 ||
        inodeNumber >= static_cast<int>(MiniFSConfig::MAX_INODES)) {
        return;
    }

    if ((*bitmap_)[inodeNumber] == 0) {
        return;
    }

    (*bitmap_)[inodeNumber] = 0;
    (*inodes_)[inodeNumber] = Inode{};
    (*inodes_)[inodeNumber].id = -1;
    ++superBlock_->freeInodes;
}

Inode* InodeManager::get(int inodeNumber) {
    if (inodeNumber < 0 ||
        inodeNumber >= static_cast<int>(MiniFSConfig::MAX_INODES) ||
        (*bitmap_)[inodeNumber] == 0) {
        return nullptr;
    }

    return &(*inodes_)[inodeNumber];
}

const Inode* InodeManager::get(int inodeNumber) const {
    if (inodeNumber < 0 ||
        inodeNumber >= static_cast<int>(MiniFSConfig::MAX_INODES) ||
        (*bitmap_)[inodeNumber] == 0) {
        return nullptr;
    }

    return &(*inodes_)[inodeNumber];
}
