#include "VirtualDisk.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <iostream>

VirtualDisk::VirtualDisk(const std::string& path)
    : fd_(-1), path_(path) {}

VirtualDisk::~VirtualDisk() {
    closeDisk();
}

bool VirtualDisk::openOrCreate() {
    fd_ = ::open(path_.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd_ < 0) {
        std::cerr << "Disk open failed: " << std::strerror(errno) << "\n";
        return false;
    }

    struct stat st {};
    if (::fstat(fd_, &st) != 0) {
        std::cerr << "fstat failed: " << std::strerror(errno) << "\n";
        closeDisk();
        return false;
    }

    if (static_cast<std::size_t>(st.st_size) != MiniFSConfig::DISK_SIZE) {
        if (::ftruncate(fd_, static_cast<off_t>(MiniFSConfig::DISK_SIZE)) != 0) {
            std::cerr << "ftruncate failed: " << std::strerror(errno) << "\n";
            closeDisk();
            return false;
        }
    }

    return true;
}

bool VirtualDisk::readBytes(std::size_t offset, void* buffer, std::size_t size) const {
    if (fd_ < 0 || offset + size > MiniFSConfig::DISK_SIZE) {
        return false;
    }

    if (::lseek(fd_, static_cast<off_t>(offset), SEEK_SET) < 0) {
        return false;
    }

    std::size_t total = 0;
    auto* ptr = static_cast<char*>(buffer);

    while (total < size) {
        const ssize_t n = ::read(fd_, ptr + total, size - total);
        if (n <= 0) {
            return false;
        }
        total += static_cast<std::size_t>(n);
    }

    return true;
}

bool VirtualDisk::writeBytes(std::size_t offset, const void* buffer, std::size_t size) {
    if (fd_ < 0 || offset + size > MiniFSConfig::DISK_SIZE) {
        return false;
    }

    if (::lseek(fd_, static_cast<off_t>(offset), SEEK_SET) < 0) {
        return false;
    }

    std::size_t total = 0;
    const auto* ptr = static_cast<const char*>(buffer);

    while (total < size) {
        const ssize_t n = ::write(fd_, ptr + total, size - total);
        if (n <= 0) {
            return false;
        }
        total += static_cast<std::size_t>(n);
    }

    return true;
}

bool VirtualDisk::readBlock(std::size_t blockNumber, void* buffer) const {
    if (blockNumber >= MiniFSConfig::TOTAL_BLOCKS) {
        return false;
    }
    return readBytes(blockNumber * MiniFSConfig::BLOCK_SIZE,
                     buffer,
                     MiniFSConfig::BLOCK_SIZE);
}

bool VirtualDisk::writeBlock(std::size_t blockNumber, const void* buffer) {
    if (blockNumber >= MiniFSConfig::TOTAL_BLOCKS) {
        return false;
    }
    return writeBytes(blockNumber * MiniFSConfig::BLOCK_SIZE,
                      buffer,
                      MiniFSConfig::BLOCK_SIZE);
}

bool VirtualDisk::sync() {
    return fd_ >= 0 && ::fsync(fd_) == 0;
}

void VirtualDisk::closeDisk() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool VirtualDisk::isOpen() const {
    return fd_ >= 0;
}
