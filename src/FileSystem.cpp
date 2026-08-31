#include "FileSystem.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace {
    std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> parts;
        std::stringstream ss(path);
        std::string item;

        while (std::getline(ss, item, '/')) {
            if (!item.empty()) {
                parts.push_back(item);
            }
        }

        return parts;
    }
}

FileSystem::FileSystem()
    : disk_(MiniFSConfig::DISK_PATH),
      blockManager_(blockBitmap_, superBlock_),
      inodeManager_(inodes_, inodeBitmap_, superBlock_),
      directory_(inodes_),
      currentDirectory_(0) {}

bool FileSystem::initialize() {
    std::lock_guard<std::mutex> lock(fsMutex_);

    if (!disk_.openOrCreate()) {
        return false;
    }

    if (!loadMetadata()) {
        std::cout << "No valid MiniFS found. Formatting virtual disk...\n";

        // We already hold the mutex, so perform formatting inline.
        superBlock_ = SuperBlock{};
        std::memcpy(superBlock_.magic, MiniFSConfig::MAGIC, std::strlen(MiniFSConfig::MAGIC));
        superBlock_.version = 1;
        superBlock_.blockSize =
            static_cast<std::uint32_t>(MiniFSConfig::BLOCK_SIZE);
        superBlock_.totalBlocks =
            static_cast<std::uint32_t>(MiniFSConfig::TOTAL_BLOCKS);
        superBlock_.dataStartBlock =
            static_cast<std::uint32_t>(MiniFSConfig::DATA_START_BLOCK);
        superBlock_.maxInodes =
            static_cast<std::uint32_t>(MiniFSConfig::MAX_INODES);
        superBlock_.rootInode = 0;

        blockManager_.initialize();
        inodeManager_.initialize();

        const int root = inodeManager_.allocate("/", true, -1);
        if (root != 0) {
            std::cerr << "Failed to create root inode.\n";
            return false;
        }

        currentDirectory_ = root;
        return saveMetadata();
    }

    currentDirectory_ = superBlock_.rootInode;
    return true;
}

bool FileSystem::format() {
    std::lock_guard<std::mutex> lock(fsMutex_);

    if (!disk_.isOpen() && !disk_.openOrCreate()) {
        return false;
    }

    superBlock_ = SuperBlock{};
    std::memcpy(superBlock_.magic, MiniFSConfig::MAGIC, std::strlen(MiniFSConfig::MAGIC));

    superBlock_.version = 1;
    superBlock_.blockSize =
        static_cast<std::uint32_t>(MiniFSConfig::BLOCK_SIZE);
    superBlock_.totalBlocks =
        static_cast<std::uint32_t>(MiniFSConfig::TOTAL_BLOCKS);
    superBlock_.dataStartBlock =
        static_cast<std::uint32_t>(MiniFSConfig::DATA_START_BLOCK);
    superBlock_.maxInodes =
        static_cast<std::uint32_t>(MiniFSConfig::MAX_INODES);
    superBlock_.rootInode = 0;

    blockManager_.initialize();
    inodeManager_.initialize();

    const int root = inodeManager_.allocate("/", true, -1);
    if (root != 0) {
        std::cerr << "Failed to allocate root inode.\n";
        return false;
    }

    currentDirectory_ = root;

    if (!saveMetadata()) {
        return false;
    }

    std::cout << "MiniFS formatted successfully.\n";
    return true;
}

bool FileSystem::loadMetadata() {
    if (!disk_.readBytes(
            MiniFSConfig::SUPERBLOCK_OFFSET,
            &superBlock_,
            sizeof(superBlock_))) {
        return false;
    }

    if (std::strncmp(
            superBlock_.magic,
            MiniFSConfig::MAGIC,
            std::strlen(MiniFSConfig::MAGIC)) != 0) {
        return false;
    }

    if (superBlock_.blockSize != MiniFSConfig::BLOCK_SIZE ||
        superBlock_.totalBlocks != MiniFSConfig::TOTAL_BLOCKS ||
        superBlock_.maxInodes != MiniFSConfig::MAX_INODES) {
        return false;
    }

    if (!disk_.readBytes(
            MiniFSConfig::INODE_BITMAP_OFFSET,
            inodeBitmap_.data(),
            inodeBitmap_.size())) {
        return false;
    }

    if (!disk_.readBytes(
            MiniFSConfig::BLOCK_BITMAP_OFFSET,
            blockBitmap_.data(),
            blockBitmap_.size())) {
        return false;
    }

    if (!disk_.readBytes(
            MiniFSConfig::INODE_TABLE_OFFSET,
            inodes_.data(),
            sizeof(Inode) * inodes_.size())) {
        return false;
    }

    return inodeManager_.get(superBlock_.rootInode) != nullptr;
}

bool FileSystem::saveMetadata() {
    if (!disk_.writeBytes(
            MiniFSConfig::SUPERBLOCK_OFFSET,
            &superBlock_,
            sizeof(superBlock_))) {
        return false;
    }

    if (!disk_.writeBytes(
            MiniFSConfig::INODE_BITMAP_OFFSET,
            inodeBitmap_.data(),
            inodeBitmap_.size())) {
        return false;
    }

    if (!disk_.writeBytes(
            MiniFSConfig::BLOCK_BITMAP_OFFSET,
            blockBitmap_.data(),
            blockBitmap_.size())) {
        return false;
    }

    if (!disk_.writeBytes(
            MiniFSConfig::INODE_TABLE_OFFSET,
            inodes_.data(),
            sizeof(Inode) * inodes_.size())) {
        return false;
    }

    return disk_.sync();
}

int FileSystem::resolvePathUnlocked(
    const std::string& path,
    bool requireDirectory
) const {
    if (path.empty() || path == ".") {
        const Inode* current = inodeManager_.get(currentDirectory_);
        if (!current) {
            return -1;
        }

        if (requireDirectory && !current->isDirectory) {
            return -1;
        }

        return currentDirectory_;
    }

    int current = path.front() == '/'
        ? superBlock_.rootInode
        : currentDirectory_;

    if (path == "/") {
        return superBlock_.rootInode;
    }

    for (const auto& part : splitPath(path)) {
        if (part == ".") {
            continue;
        }

        if (part == "..") {
            const Inode* inode = inodeManager_.get(current);
            if (!inode) {
                return -1;
            }

            if (inode->parent >= 0) {
                current = inode->parent;
            }
            continue;
        }

        const Inode* parent = inodeManager_.get(current);
        if (!parent || !parent->isDirectory) {
            return -1;
        }

        current = directory_.findChild(current, part);
        if (current < 0) {
            return -1;
        }
    }

    const Inode* result = inodeManager_.get(current);
    if (!result) {
        return -1;
    }

    if (requireDirectory && !result->isDirectory) {
        return -1;
    }

    return current;
}

bool FileSystem::splitParentAndNameUnlocked(
    const std::string& path,
    int& parentInode,
    std::string& name
) const {
    if (path.empty() || path == "/" || path == "." || path == "..") {
        return false;
    }

    std::string cleaned = path;
    while (cleaned.size() > 1 && cleaned.back() == '/') {
        cleaned.pop_back();
    }

    const auto pos = cleaned.find_last_of('/');

    if (pos == std::string::npos) {
        parentInode = currentDirectory_;
        name = cleaned;
    } else {
        const std::string parentPath =
            pos == 0 ? "/" : cleaned.substr(0, pos);

        name = cleaned.substr(pos + 1);
        parentInode = resolvePathUnlocked(parentPath, true);
    }

    if (parentInode < 0 ||
        name.empty() ||
        name == "." ||
        name == ".." ||
        name.size() >= MiniFSConfig::MAX_NAME) {
        return false;
    }

    return true;
}

std::vector<int> FileSystem::currentBlocksUnlocked(
    const Inode& inode
) const {
    std::vector<int> result;

    for (int i = 0; i < inode.blockCount; ++i) {
        if (inode.blocks[i] >= 0) {
            result.push_back(inode.blocks[i]);
        }
    }

    return result;
}

bool FileSystem::createFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    int parent = -1;
    std::string name;

    if (!splitParentAndNameUnlocked(path, parent, name)) {
        std::cout << "Invalid file path.\n";
        return false;
    }

    if (directory_.findChild(parent, name) >= 0) {
        std::cout << "A file or directory with that name already exists.\n";
        return false;
    }

    const int inode = inodeManager_.allocate(name, false, parent);
    if (inode < 0) {
        std::cout << "No free inode available.\n";
        return false;
    }

    if (!saveMetadata()) {
        inodeManager_.freeInode(inode);
        return false;
    }

    std::cout << "File created: " << path << "\n";
    return true;
}

bool FileSystem::writeFileUnlocked(
    int inodeNumber,
    const std::string& content,
    bool append
) {
    Inode* inode = inodeManager_.get(inodeNumber);
    if (!inode || inode->isDirectory) {
        return false;
    }

    std::string finalContent;

    if (append) {
        finalContent = readFileUnlocked(inodeNumber);
        finalContent += content;
    } else {
        finalContent = content;
    }

    const std::size_t maxSize =
        MiniFSConfig::DIRECT_BLOCKS * MiniFSConfig::BLOCK_SIZE;

    if (finalContent.size() > maxSize) {
        std::cout << "File too large. Maximum file size is "
                  << maxSize << " bytes in this MiniFS.\n";
        return false;
    }

    const std::size_t requiredBlocks =
        finalContent.empty()
            ? 0
            : (finalContent.size() + MiniFSConfig::BLOCK_SIZE - 1)
                / MiniFSConfig::BLOCK_SIZE;

    const std::vector<int> oldBlocks = currentBlocksUnlocked(*inode);

    std::vector<int> newBlocks;
    if (!blockManager_.allocateBlocks(requiredBlocks, newBlocks)) {
        std::cout << "Not enough free blocks.\n";
        return false;
    }

    std::array<char, MiniFSConfig::BLOCK_SIZE> buffer {};

    for (std::size_t i = 0; i < newBlocks.size(); ++i) {
        buffer.fill('\0');

        const std::size_t start = i * MiniFSConfig::BLOCK_SIZE;
        const std::size_t remaining = finalContent.size() - start;
        const std::size_t chunk =
            std::min(remaining, MiniFSConfig::BLOCK_SIZE);

        if (chunk > 0) {
            std::memcpy(
                buffer.data(),
                finalContent.data() + start,
                chunk
            );
        }

        if (!disk_.writeBlock(
                static_cast<std::size_t>(newBlocks[i]),
                buffer.data())) {
            blockManager_.freeBlocks(newBlocks);
            return false;
        }
    }

    blockManager_.freeBlocks(oldBlocks);

    std::fill(
        std::begin(inode->blocks),
        std::end(inode->blocks),
        -1
    );

    inode->blockCount = static_cast<int>(newBlocks.size());

    for (std::size_t i = 0; i < newBlocks.size(); ++i) {
        inode->blocks[i] = newBlocks[i];
    }

    inode->size = finalContent.size();
    inode->modifiedAt = static_cast<std::int64_t>(std::time(nullptr));

    return saveMetadata();
}

bool FileSystem::writeFile(
    const std::string& path,
    const std::string& content,
    bool append
) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, false);
    Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inode->isDirectory) {
        std::cout << "File not found.\n";
        return false;
    }

    if (!writeFileUnlocked(inodeNumber, content, append)) {
        return false;
    }

    std::cout << (append ? "Appended " : "Wrote ")
              << content.size()
              << " byte(s) to "
              << path << "\n";

    return true;
}

std::string FileSystem::readFileUnlocked(int inodeNumber) const {
    const Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inode->isDirectory) {
        return {};
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(inode->size));

    std::array<char, MiniFSConfig::BLOCK_SIZE> buffer {};

    std::size_t remaining = static_cast<std::size_t>(inode->size);

    for (int i = 0; i < inode->blockCount && remaining > 0; ++i) {
        if (inode->blocks[i] < 0) {
            break;
        }

        if (!disk_.readBlock(
                static_cast<std::size_t>(inode->blocks[i]),
                buffer.data())) {
            return {};
        }

        const std::size_t chunk =
            std::min(remaining, MiniFSConfig::BLOCK_SIZE);

        result.append(buffer.data(), chunk);
        remaining -= chunk;
    }

    return result;
}

bool FileSystem::readFile(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, false);
    const Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inode->isDirectory) {
        std::cout << "File not found.\n";
        return false;
    }

    std::cout << readFileUnlocked(inodeNumber) << "\n";
    return true;
}

std::string FileSystem::getFileContent(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);
    const int inodeNumber = resolvePathUnlocked(path, false);
    return readFileUnlocked(inodeNumber);
}

bool FileSystem::fileExists(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);
    const int inodeNumber = resolvePathUnlocked(path, false);
    const Inode* inode = inodeManager_.get(inodeNumber);
    return inode != nullptr && !inode->isDirectory;
}

bool FileSystem::removeFileUnlocked(int inodeNumber) {
    Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inode->isDirectory) {
        return false;
    }

    blockManager_.freeBlocks(currentBlocksUnlocked(*inode));
    inodeManager_.freeInode(inodeNumber);

    return saveMetadata();
}

bool FileSystem::deleteFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, false);
    const Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inode->isDirectory) {
        std::cout << "File not found.\n";
        return false;
    }

    if (!removeFileUnlocked(inodeNumber)) {
        return false;
    }

    std::cout << "File deleted: " << path << "\n";
    return true;
}

bool FileSystem::makeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    int parent = -1;
    std::string name;

    if (!splitParentAndNameUnlocked(path, parent, name)) {
        std::cout << "Invalid directory path.\n";
        return false;
    }

    if (directory_.findChild(parent, name) >= 0) {
        std::cout << "A file or directory with that name already exists.\n";
        return false;
    }

    const int inode = inodeManager_.allocate(name, true, parent);
    if (inode < 0) {
        std::cout << "No free inode available.\n";
        return false;
    }

    if (!saveMetadata()) {
        inodeManager_.freeInode(inode);
        return false;
    }

    std::cout << "Directory created: " << path << "\n";
    return true;
}

bool FileSystem::removeDirectoryUnlocked(int inodeNumber) {
    Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || !inode->isDirectory ||
        inodeNumber == superBlock_.rootInode) {
        return false;
    }

    if (!directory_.isEmpty(inodeNumber)) {
        return false;
    }

    inodeManager_.freeInode(inodeNumber);
    return saveMetadata();
}

bool FileSystem::removeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, true);

    if (inodeNumber < 0) {
        std::cout << "Directory not found.\n";
        return false;
    }

    if (inodeNumber == superBlock_.rootInode) {
        std::cout << "Root directory cannot be removed.\n";
        return false;
    }

    if (!directory_.isEmpty(inodeNumber)) {
        std::cout << "Directory is not empty.\n";
        return false;
    }

    if (currentDirectory_ == inodeNumber) {
        currentDirectory_ = inodes_[inodeNumber].parent;
    }

    if (!removeDirectoryUnlocked(inodeNumber)) {
        return false;
    }

    std::cout << "Directory removed: " << path << "\n";
    return true;
}

bool FileSystem::changeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, true);

    if (inodeNumber < 0) {
        std::cout << "Directory not found.\n";
        return false;
    }

    currentDirectory_ = inodeNumber;
    return true;
}

void FileSystem::listDirectory(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber =
        path.empty()
            ? currentDirectory_
            : resolvePathUnlocked(path, true);

    if (inodeNumber < 0) {
        std::cout << "Directory not found.\n";
        return;
    }

    const auto children = directory_.children(inodeNumber);

    std::cout << std::left
              << std::setw(8) << "TYPE"
              << std::setw(28) << "NAME"
              << std::setw(12) << "SIZE"
              << "INODE\n";

    std::cout << std::string(58, '-') << "\n";

    for (const int childId : children) {
        const Inode* child = inodeManager_.get(childId);
        if (!child) {
            continue;
        }

        std::cout << std::left
                  << std::setw(8)
                  << (child->isDirectory ? "DIR" : "FILE")
                  << std::setw(28)
                  << child->name
                  << std::setw(12)
                  << (child->isDirectory
                      ? directory_.childCount(childId)
                      : static_cast<std::size_t>(child->size))
                  << child->id
                  << "\n";
    }

    if (children.empty()) {
        std::cout << "(empty)\n";
    }
}

std::string FileSystem::pathOfUnlocked(int inodeNumber) const {
    if (inodeNumber == superBlock_.rootInode) {
        return "/";
    }

    std::vector<std::string> parts;
    int current = inodeNumber;

    while (current >= 0 &&
           current != superBlock_.rootInode) {
        const Inode* inode = inodeManager_.get(current);
        if (!inode) {
            break;
        }

        parts.push_back(inode->name);
        current = inode->parent;
    }

    std::reverse(parts.begin(), parts.end());

    std::string result;

    for (const auto& part : parts) {
        result += "/" + part;
    }

    return result.empty() ? "/" : result;
}

void FileSystem::printWorkingDirectory() const {
    std::lock_guard<std::mutex> lock(fsMutex_);
    std::cout << pathOfUnlocked(currentDirectory_) << "\n";
}

std::string FileSystem::timeToString(std::int64_t value) {
    if (value <= 0) {
        return "-";
    }

    const std::time_t raw = static_cast<std::time_t>(value);
    std::tm tm {};

    localtime_r(&raw, &tm);

    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

void FileSystem::statPath(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber = resolvePathUnlocked(path, false);
    const Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode) {
        std::cout << "Path not found.\n";
        return;
    }

    std::cout << "\n========== MiniFS STAT ==========\n";
    std::cout << "Path          : "
              << pathOfUnlocked(inodeNumber) << "\n";
    std::cout << "Name          : "
              << (inodeNumber == superBlock_.rootInode
                    ? "/"
                    : inode->name)
              << "\n";
    std::cout << "Type          : "
              << (inode->isDirectory ? "Directory" : "Regular File")
              << "\n";
    std::cout << "Inode Number  : "
              << inode->id << "\n";
    std::cout << "Parent Inode  : "
              << inode->parent << "\n";
    std::cout << "Created       : "
              << timeToString(inode->createdAt) << "\n";
    std::cout << "Modified      : "
              << timeToString(inode->modifiedAt) << "\n";

    if (inode->isDirectory) {
        std::cout << "Child Count   : "
                  << directory_.childCount(inodeNumber)
                  << "\n";
        std::cout << "Size          : Directory metadata\n";
        std::cout << "Data Blocks   : 0\n";
    } else {
        std::cout << "Size          : "
                  << inode->size << " bytes\n";
        std::cout << "Data Blocks   : "
                  << inode->blockCount << "\n";

        std::cout << "Block IDs     : ";
        if (inode->blockCount == 0) {
            std::cout << "(none)";
        } else {
            for (int i = 0; i < inode->blockCount; ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << inode->blocks[i];
            }
        }
        std::cout << "\n";
    }

    std::cout << "=================================\n\n";
}

void FileSystem::diskInfo() const {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const std::size_t usedBlocks =
        MiniFSConfig::TOTAL_BLOCKS - superBlock_.freeBlocks;

    const std::size_t usedInodes =
        MiniFSConfig::MAX_INODES - superBlock_.freeInodes;

    std::cout << "\n========== DISK INFO ==========\n";
    std::cout << "Disk Size       : "
              << MiniFSConfig::DISK_SIZE / (1024 * 1024)
              << " MB\n";
    std::cout << "Block Size      : "
              << MiniFSConfig::BLOCK_SIZE
              << " bytes\n";
    std::cout << "Total Blocks    : "
              << MiniFSConfig::TOTAL_BLOCKS
              << "\n";
    std::cout << "Metadata Blocks : "
              << MiniFSConfig::DATA_START_BLOCK
              << "\n";
    std::cout << "Used Blocks     : "
              << usedBlocks
              << "\n";
    std::cout << "Free Blocks     : "
              << superBlock_.freeBlocks
              << "\n";
    std::cout << "Total Inodes    : "
              << MiniFSConfig::MAX_INODES
              << "\n";
    std::cout << "Used Inodes     : "
              << usedInodes
              << "\n";
    std::cout << "Free Inodes     : "
              << superBlock_.freeInodes
              << "\n";
    std::cout << "Current Path    : "
              << pathOfUnlocked(currentDirectory_)
              << "\n";
    std::cout << "===============================\n\n";
}

void FileSystem::printTreeUnlocked(
    int inodeNumber,
    const std::string& prefix,
    bool isLast
) const {
    const Inode* inode = inodeManager_.get(inodeNumber);
    if (!inode) {
        return;
    }

    if (inodeNumber == superBlock_.rootInode) {
        std::cout << "/\n";
    } else {
        std::cout << prefix
                  << (isLast ? "└── " : "├── ")
                  << inode->name
                  << (inode->isDirectory ? "/" : "")
                  << "\n";
    }

    if (!inode->isDirectory) {
        return;
    }

    auto children = directory_.children(inodeNumber);

    std::sort(
        children.begin(),
        children.end(),
        [this](int a, int b) {
            const Inode* left = inodeManager_.get(a);
            const Inode* right = inodeManager_.get(b);

            if (!left || !right) {
                return a < b;
            }

            if (left->isDirectory != right->isDirectory) {
                return left->isDirectory > right->isDirectory;
            }

            return std::string(left->name) < std::string(right->name);
        }
    );

    std::string nextPrefix = prefix;

    if (inodeNumber != superBlock_.rootInode) {
        nextPrefix += isLast ? "    " : "│   ";
    }

    for (std::size_t i = 0; i < children.size(); ++i) {
        printTreeUnlocked(
            children[i],
            nextPrefix,
            i + 1 == children.size()
        );
    }
}

void FileSystem::tree(const std::string& path) const {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int inodeNumber =
        path.empty()
            ? currentDirectory_
            : resolvePathUnlocked(path, false);

    if (inodeNumber < 0) {
        std::cout << "Path not found.\n";
        return;
    }

    printTreeUnlocked(inodeNumber, "", true);
}

bool FileSystem::renamePath(
    const std::string& oldPath,
    const std::string& newName
) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    if (newName.empty() ||
        newName == "." ||
        newName == ".." ||
        newName.find('/') != std::string::npos ||
        newName.size() >= MiniFSConfig::MAX_NAME) {
        std::cout << "Invalid new name.\n";
        return false;
    }

    const int inodeNumber = resolvePathUnlocked(oldPath, false);
    Inode* inode = inodeManager_.get(inodeNumber);

    if (!inode || inodeNumber == superBlock_.rootInode) {
        std::cout << "Path not found or root cannot be renamed.\n";
        return false;
    }

    if (directory_.findChild(inode->parent, newName) >= 0) {
        std::cout << "Name already exists in that directory.\n";
        return false;
    }

    std::strncpy(
        inode->name,
        newName.c_str(),
        MiniFSConfig::MAX_NAME - 1
    );
    inode->name[MiniFSConfig::MAX_NAME - 1] = '\0';
    inode->modifiedAt = static_cast<std::int64_t>(std::time(nullptr));

    if (!saveMetadata()) {
        return false;
    }

    std::cout << "Renamed successfully.\n";
    return true;
}

bool FileSystem::copyFile(
    const std::string& source,
    const std::string& destination
) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int sourceInode = resolvePathUnlocked(source, false);
    const Inode* src = inodeManager_.get(sourceInode);

    if (!src || src->isDirectory) {
        std::cout << "Source file not found.\n";
        return false;
    }

    int parent = -1;
    std::string name;

    if (!splitParentAndNameUnlocked(destination, parent, name)) {
        std::cout << "Invalid destination path.\n";
        return false;
    }

    if (directory_.findChild(parent, name) >= 0) {
        std::cout << "Destination already exists.\n";
        return false;
    }

    const std::string content = readFileUnlocked(sourceInode);

    const int newInode = inodeManager_.allocate(name, false, parent);
    if (newInode < 0) {
        std::cout << "No free inode available.\n";
        return false;
    }

    if (!writeFileUnlocked(newInode, content, false)) {
        inodeManager_.freeInode(newInode);
        saveMetadata();
        return false;
    }

    std::cout << "File copied successfully.\n";
    return true;
}

bool FileSystem::movePath(
    const std::string& source,
    const std::string& destinationDirectory
) {
    std::lock_guard<std::mutex> lock(fsMutex_);

    const int sourceInode = resolvePathUnlocked(source, false);
    Inode* src = inodeManager_.get(sourceInode);

    if (!src || sourceInode == superBlock_.rootInode) {
        std::cout << "Source path not found or root cannot be moved.\n";
        return false;
    }

    const int destination =
        resolvePathUnlocked(destinationDirectory, true);

    if (destination < 0) {
        std::cout << "Destination directory not found.\n";
        return false;
    }

    if (sourceInode == destination) {
        std::cout << "Cannot move a path into itself.\n";
        return false;
    }

    // Prevent moving a directory into one of its descendants.
    if (src->isDirectory) {
        int current = destination;
        while (current >= 0) {
            if (current == sourceInode) {
                std::cout << "Cannot move a directory inside its own subtree.\n";
                return false;
            }

            const Inode* inode = inodeManager_.get(current);
            if (!inode) {
                break;
            }
            current = inode->parent;
        }
    }

    if (directory_.findChild(destination, src->name) >= 0) {
        std::cout << "Destination already contains that name.\n";
        return false;
    }

    src->parent = destination;
    src->modifiedAt = static_cast<std::int64_t>(std::time(nullptr));

    if (!saveMetadata()) {
        return false;
    }

    std::cout << "Moved successfully.\n";
    return true;
}

void FileSystem::threadTest() {
    std::mutex outputMutex;

    {
        std::lock_guard<std::mutex> lock(fsMutex_);
        if (directory_.findChild(currentDirectory_, "thread_demo") < 0) {
            const int inode =
                inodeManager_.allocate(
                    "thread_demo",
                    true,
                    currentDirectory_
                );

            if (inode >= 0) {
                saveMetadata();
            }
        }
    }

    const int testDirectory = [&]() {
        std::lock_guard<std::mutex> lock(fsMutex_);
        return directory_.findChild(currentDirectory_, "thread_demo");
    }();

    if (testDirectory < 0) {
        std::cout << "Unable to create thread_demo directory.\n";
        return;
    }

    const std::string basePath =
        pathOfUnlocked(testDirectory);

    std::cout << "Starting concurrent filesystem test...\n";

    auto worker = [this, &outputMutex, basePath](int id) {
        const std::string file =
            basePath + "/thread_" + std::to_string(id) + ".txt";

        {
            std::lock_guard<std::mutex> out(outputMutex);
            std::cout << "[Thread " << id
                      << "] requesting filesystem access\n";
        }

        {
            std::lock_guard<std::mutex> lock(fsMutex_);

            int inodeNumber = resolvePathUnlocked(file, false);

            if (inodeNumber < 0) {
                int parent = -1;
                std::string name;

                if (splitParentAndNameUnlocked(file, parent, name)) {
                    inodeNumber =
                        inodeManager_.allocate(name, false, parent);
                }
            }

            if (inodeNumber >= 0) {
                const std::string text =
                    "Data written safely by thread "
                    + std::to_string(id);

                writeFileUnlocked(inodeNumber, text, false);
            }
        }

        {
            std::lock_guard<std::mutex> out(outputMutex);
            std::cout << "[Thread " << id
                      << "] completed safely\n";
        }
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Thread test complete. Check: tree "
              << basePath << "\n";
}

std::string FileSystem::promptPath() const {
    std::lock_guard<std::mutex> lock(fsMutex_);
    return pathOfUnlocked(currentDirectory_);
}
