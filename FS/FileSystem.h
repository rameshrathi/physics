#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace FS { // Start of FS

using ino_t = uint64_t;
using mode_t = uint32_t;
using nlink_t = uint16_t;

using UChar = unsigned char;

struct ExAttrs {
    std::string name;
    std::string value;
};

struct INode {
    ino_t ino;      // Inode No
    mode_t mode;    // File type and permissions
    uid_t uid;      // Owner user Id
    gid_t gid;      // Owner Group Id
    nlink_t nlink;
    off_t size;
    time_t atime;
    time_t mtime;
    time_t ctime;
    blkcnt_t blocks;

    // External attributes
    std::vector<ExAttrs> ex_attrs;

    // Block pointers (for data storage)
    uint32_t direct_blocks[12];   // Direct block pointers
    uint32_t indirect_block;      // Single indirect
    uint32_t double_indirect;     // Doubly indirect
    uint32_t triple_indirect;     // Triple indirect
};

struct FileInfo {
    std::string name;
    bool is_directory;
    std::uint64_t size;
    std::uint64_t created_time;
    std::uint64_t modified_time;
};

// Directory entry structure
struct Dirent {
    ino_t ino;                    // Inode number
    off_t offset;                 // Offset in dir
    unsigned short record_len;        // Record length
    UChar type;           // File type
    char name[];                  // Filename (variable length)
};

// Superblock structure
struct SuperBlock {
    uint32_t magic;               // Magic number
    uint32_t version;             // Filesystem version
    uint32_t block_size;          // Block size
    uint32_t total_blocks;        // Total blocks
    uint32_t free_blocks;         // Free blocks
    uint32_t total_inodes;        // Total inodes
    uint32_t free_inodes;         // Free inodes
    uint32_t first_data_block;    // First data block
    uint32_t inode_table_block;   // Inode table location
    uint32_t block_bitmap_block;  // Block bitmap location
    uint32_t inode_bitmap_block;  // Inode bitmap location
    time_t mount_time;            // Last mount time
    time_t write_time;            // Last write time
    uint16_t mount_count;         // Mount count
    uint16_t max_mount_count;     // Maximum mount count
    uint16_t state;               // Filesystem state
    uint32_t checksum;            // Superblock checksum
};

// Block group descriptor (for large filesystems)
struct BlockGroupDescriptor {
    uint32_t block_bitmap;        // Block bitmap block
    uint32_t inode_bitmap;        // Inode bitmap block
    uint32_t inode_table;         // Inode table block
    uint16_t free_blocks_count;   // Free blocks in group
    uint16_t free_inodes_count;   // Free inodes in group
    uint16_t used_dirs_count;     // Used directories count
    uint16_t flags;               // Group flags
    uint32_t reserved[3];         // Reserved for future use
};

// Journal entry for crash recovery
struct JournalEntry {
    uint32_t magic;               // Journal magic
    uint32_t sequence;            // Sequence number
    uint32_t type;                // Transaction type
    uint32_t checksum;            // Entry checksum
    time_t timestamp;             // Transaction timestamp
    uint32_t blocks_count;        // Number of blocks in transaction
    uint32_t block_list[];        // List of blocks (variable length)
};

// In-memory file structure (enhanced)
struct File {
    struct INode inode;           // Inode data
    char *name;                   // Filename
    struct myfs_file *parent;     // Parent directory
    struct myfs_file *children;   // Child files (for directories)
    struct myfs_file *next;       // Next sibling

    // Caching and performance
    void *cache;                  // File data cache
    size_t cache_size;            // Cache size
    int cache_dirty;              // Cache dirty flag

    // Locking
    pthread_mutex_t lock;         // File lock
    int lock_count;               // Lock ref count
};

// B-tree node for efficient file lookup
struct BtreeNode {
    int is_leaf;                  // Is this a leaf node?
    int key_count;                // Number of keys
    ino_t keys[255];              // Keys (inode numbers)
    struct BtreeNode *children[256]; // Child pointers
    struct myfs_file *files[255]; // File pointers (leaf nodes only)
};

// Bitmap operations for block/inode allocation
static inline int test_bit(void *bitmap, int bit) {
    UChar *bytes = static_cast<UChar *>(bitmap);
    return (bytes[bit / 8] >> (bit % 8)) & 1;
}

static inline void set_bit(void *bitmap, int bit) {
    UChar *bytes = (UChar *)bitmap;
    bytes[bit / 8] |= (1 << (bit % 8));
}

static inline void clear_bit(void *bitmap, int bit) {
    UChar *bytes = (UChar *)bitmap;
    bytes[bit / 8] &= ~(1 << (bit % 8));
}

    // Hash table for fast filename lookup
#define HASH_TABLE_SIZE 1024

struct HashEntry {
    char *name;
    struct myfs_file *file;
    struct myfs_hash_entry *next;
};

struct HashTable {
    struct myfs_hash_entry *buckets[HASH_TABLE_SIZE];
    pthread_mutex_t lock;
};

// Calculate hash for filename
static inline unsigned int hash_filename(const char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + *name++;
    }
    return hash % HASH_TABLE_SIZE;
}

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
};

} // End of FS