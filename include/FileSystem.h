#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "BlockManager.h"
#include "Constants.h"
#include "Directory.h"
#include "Inode.h"
#include "InodeManager.h"
#include "SuperBlock.h"
#include "VirtualDisk.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

class FileSystem {
private:
    VirtualDisk disk_;
    SuperBlock superBlock_ {};

    std::array<std::uint8_t, MiniFSConfig::MAX_INODES> inodeBitmap_ {};
    std::array<std::uint8_t, MiniFSConfig::TOTAL_BLOCKS> blockBitmap_ {};
    std::array<Inode, MiniFSConfig::MAX_INODES> inodes_ {};

    BlockManager blockManager_;
    InodeManager inodeManager_;
    Directory directory_;

    int currentDirectory_;
    mutable std::mutex fsMutex_;

    bool loadMetadata();
    bool saveMetadata();

    int resolvePathUnlocked(
        const std::string& path,
        bool requireDirectory = false
    ) const;

    bool splitParentAndNameUnlocked(
        const std::string& path,
        int& parentInode,
        std::string& name
    ) const;

    std::string pathOfUnlocked(int inodeNumber) const;
    std::vector<int> currentBlocksUnlocked(const Inode& inode) const;

    bool writeFileUnlocked(
        int inodeNumber,
        const std::string& content,
        bool append
    );

    bool removeFileUnlocked(int inodeNumber);
    bool removeDirectoryUnlocked(int inodeNumber);

    std::string readFileUnlocked(int inodeNumber) const;

    void printTreeUnlocked(
        int inodeNumber,
        const std::string& prefix,
        bool isLast
    ) const;

    static std::string timeToString(std::int64_t value);

public:
    FileSystem();

    bool initialize();
    bool format();

    bool createFile(const std::string& path);
    bool writeFile(
        const std::string& path,
        const std::string& content,
        bool append = false
    );
    bool readFile(const std::string& path) const;
    std::string getFileContent(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    bool deleteFile(const std::string& path);

    bool makeDirectory(const std::string& path);
    bool removeDirectory(const std::string& path);

    bool changeDirectory(const std::string& path);
    void listDirectory(const std::string& path = "") const;
    void printWorkingDirectory() const;

    void statPath(const std::string& path) const;
    void diskInfo() const;
    void tree(const std::string& path = "") const;

    bool renamePath(
        const std::string& oldPath,
        const std::string& newName
    );

    bool copyFile(
        const std::string& source,
        const std::string& destination
    );

    bool movePath(
        const std::string& source,
        const std::string& destinationDirectory
    );

    void threadTest();

    std::string promptPath() const;
};

#endif
