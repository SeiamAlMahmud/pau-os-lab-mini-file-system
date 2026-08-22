#ifndef INODE_H
#define INODE_H

#include "Constants.h"
#include <cstdint>

struct Inode {
    std::int32_t id = -1;
    std::uint8_t used = 0;
    std::uint8_t isDirectory = 0;
    std::uint16_t reserved = 0;

    std::int32_t parent = -1;
    std::uint64_t size = 0;
    std::int32_t blockCount = 0;
    std::int32_t blocks[MiniFSConfig::DIRECT_BLOCKS]{};

    char name[MiniFSConfig::MAX_NAME]{};

    std::int64_t createdAt = 0;
    std::int64_t modifiedAt = 0;
};

#endif
