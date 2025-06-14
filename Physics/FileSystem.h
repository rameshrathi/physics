#pragma once

#include <RK/Types.h>
#include <FS/Block.h>
#include <FS/Inode.h>

namespace FS { // Start of FS

class FileSystem {
public:
    virtual ~FileSystem() = default;
    FileSystem() = default;

    // Mount/unmount
    virtual bool mount(const std::string& source) = 0;
    virtual void unmount() = 0;

    // File operations
    virtual bool createFile(const std::string& path) = 0;
    virtual bool deleteFile(const std::string& path) = 0;
    virtual bool readFile(const std::string& path, std::vector<uint8_t>& data) = 0;
    virtual bool writeFile(const std::string& path, const std::vector<uint8_t>& data) = 0;

    // Directory operations
    virtual bool createDirectory(const std::string& path) = 0;
    virtual bool deleteDirectory(const std::string& path) = 0;
    virtual bool listDirectory(const std::string& path, std::vector<FileInfo>& contents) = 0;

    // Metadata
    virtual bool stat(const std::string& path, FileInfo& info) = 0;

    // Utility
    virtual bool exists(const std::string& path) = 0;

    // Size
    virtual Size totalBlockCount() = 0;
    virtual Size freeBlockCount() = 0;
    virtual Size totalInodeCount() = 0;
    virtual Size freeInodeCount() = 0;
};

} // End of FS
