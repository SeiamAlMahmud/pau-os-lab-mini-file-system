#include "BlockManager.h"

BlockManager::BlockManager(
    std::array<std::uint8_t, MiniFSConfig::TOTAL_BLOCKS>& bitmap,
    SuperBlock& superBlock
) : bitmap_(&bitmap), superBlock_(&superBlock) {}

void BlockManager::initialize() {
    bitmap_->fill(0);

    for (std::size_t i = 0; i < MiniFSConfig::DATA_START_BLOCK; ++i) {
        (*bitmap_)[i] = 1;
    }

    superBlock_->freeBlocks =
        static_cast<std::uint32_t>(
            MiniFSConfig::TOTAL_BLOCKS - MiniFSConfig::DATA_START_BLOCK
        );
}

int BlockManager::allocateBlock() {
    for (std::size_t i = MiniFSConfig::DATA_START_BLOCK;
         i < MiniFSConfig::TOTAL_BLOCKS;
         ++i) {
        if ((*bitmap_)[i] == 0) {
            (*bitmap_)[i] = 1;
            if (superBlock_->freeBlocks > 0) {
                --superBlock_->freeBlocks;
            }
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool BlockManager::allocateBlocks(std::size_t count, std::vector<int>& result) {
    result.clear();

    if (count > superBlock_->freeBlocks) {
        return false;
    }

    for (std::size_t i = 0; i < count; ++i) {
        const int block = allocateBlock();
        if (block < 0) {
            freeBlocks(result);
            result.clear();
            return false;
        }
        result.push_back(block);
    }

    return true;
}

void BlockManager::freeBlock(int blockNumber) {
    if (blockNumber < static_cast<int>(MiniFSConfig::DATA_START_BLOCK) ||
        blockNumber >= static_cast<int>(MiniFSConfig::TOTAL_BLOCKS)) {
        return;
    }

    if ((*bitmap_)[blockNumber] != 0) {
        (*bitmap_)[blockNumber] = 0;
        ++superBlock_->freeBlocks;
    }
}

void BlockManager::freeBlocks(const std::vector<int>& blocks) {
    for (const int block : blocks) {
        freeBlock(block);
    }
}

bool BlockManager::isUsed(int blockNumber) const {
    if (blockNumber < 0 ||
        blockNumber >= static_cast<int>(MiniFSConfig::TOTAL_BLOCKS)) {
        return false;
    }

    return (*bitmap_)[blockNumber] != 0;
}

std::size_t BlockManager::freeCount() const {
    return superBlock_->freeBlocks;
}
