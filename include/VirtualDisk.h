#ifndef VIRTUAL_DISK_H
#define VIRTUAL_DISK_H

#include "Constants.h"
#include <cstddef>
#include <mutex>
#include <string>

class VirtualDisk {
private:
    int fd_;
    std::string path_;
    mutable std::mutex ioMutex_;

public:
    explicit VirtualDisk(const std::string& path = MiniFSConfig::DISK_PATH);
    ~VirtualDisk();

    bool openOrCreate();
    bool readBytes(std::size_t offset, void* buffer, std::size_t size) const;
    bool writeBytes(std::size_t offset, const void* buffer, std::size_t size);

    bool readBlock(std::size_t blockNumber, void* buffer) const;
    bool writeBlock(std::size_t blockNumber, const void* buffer);

    bool sync();
    void closeDisk();
    bool isOpen() const;
};

#endif
