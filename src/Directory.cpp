#include "Directory.h"

Directory::Directory(
    const std::array<Inode, MiniFSConfig::MAX_INODES>& inodes
) : inodes_(&inodes) {}

int Directory::findChild(
    int parentInode,
    const std::string& name
) const {
    for (const auto& inode : *inodes_) {
        if (inode.used &&
            inode.parent == parentInode &&
            name == inode.name) {
            return inode.id;
        }
    }

    return -1;
}

std::vector<int> Directory::children(int parentInode) const {
    std::vector<int> result;

    for (const auto& inode : *inodes_) {
        if (inode.used && inode.parent == parentInode) {
            result.push_back(inode.id);
        }
    }

    return result;
}

std::size_t Directory::childCount(int parentInode) const {
    return children(parentInode).size();
}

bool Directory::isEmpty(int directoryInode) const {
    return childCount(directoryInode) == 0;
}
