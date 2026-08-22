#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Constants.h"
#include "Inode.h"

#include <array>
#include <string>
#include <vector>

class Directory {
private:
    const std::array<Inode, MiniFSConfig::MAX_INODES>* inodes_;

public:
    explicit Directory(
        const std::array<Inode, MiniFSConfig::MAX_INODES>& inodes
    );

    int findChild(int parentInode, const std::string& name) const;
    std::vector<int> children(int parentInode) const;
    std::size_t childCount(int parentInode) const;
    bool isEmpty(int directoryInode) const;
};

#endif
