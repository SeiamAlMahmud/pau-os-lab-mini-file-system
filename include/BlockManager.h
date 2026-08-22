#ifndef BLOCK_MANAGER_H
#define BLOCK_MANAGER_H

#include "Constants.h"
#include "SuperBlock.h"
#include <array>
#include <cstdint>
#include <vector>

class BlockManager {
private:
    std::array<std::uint8_t, MiniFSConfig::TOTAL_BLOCKS>* bitmap_;
    SuperBlock* superBlock_;

public:
    BlockManager(
        std::array<std::uint8_t, MiniFSConfig::TOTAL_BLOCKS>& bitmap,
        SuperBlock& superBlock
    );

    void initialize();
    int allocateBlock();
    bool allocateBlocks(std::size_t count, std::vector<int>& result);
    void freeBlock(int blockNumber);
    void freeBlocks(const std::vector<int>& blocks);

    bool isUsed(int blockNumber) const;
    std::size_t freeCount() const;
};

#endif
