#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <cstdint>

namespace MiniFSConfig {
    constexpr std::size_t BLOCK_SIZE = 512;
    constexpr std::size_t DISK_SIZE = 10 * 1024 * 1024; // 10 MB
    constexpr std::size_t TOTAL_BLOCKS = DISK_SIZE / BLOCK_SIZE;
    constexpr std::size_t DATA_START_BLOCK = 128;

    constexpr std::size_t MAX_INODES = 256;
    constexpr std::size_t DIRECT_BLOCKS = 8;
    constexpr std::size_t MAX_NAME = 64;

    constexpr std::size_t SUPERBLOCK_OFFSET = 0;
    constexpr std::size_t INODE_BITMAP_OFFSET = BLOCK_SIZE;
    constexpr std::size_t BLOCK_BITMAP_OFFSET = BLOCK_SIZE * 2;
    constexpr std::size_t INODE_TABLE_OFFSET =
        BLOCK_BITMAP_OFFSET + TOTAL_BLOCKS;

    constexpr const char* DISK_PATH = "disk/mini_fs.disk";
    constexpr const char* MAGIC = "MINIFS1";
}

#endif
