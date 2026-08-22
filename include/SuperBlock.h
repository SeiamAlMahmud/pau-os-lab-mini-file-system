#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <cstdint>

struct SuperBlock {
    char magic[8]{};
    std::uint32_t version = 1;
    std::uint32_t blockSize = 0;
    std::uint32_t totalBlocks = 0;
    std::uint32_t dataStartBlock = 0;
    std::uint32_t maxInodes = 0;

    std::uint32_t freeBlocks = 0;
    std::uint32_t freeInodes = 0;
    std::int32_t rootInode = 0;
};

#endif
