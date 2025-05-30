//
// Created by ramesh on 30/05/25.
//

#pragma once

#include <RK/Types.h>
#include <FS/CommonTypes.h>
#include <FS/Inode.h>

namespace FS
{

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
    UInt16 record_len;    // Record length
    UChar type;                   // File type
    Char name[];                  // Filename (variable length)
};

// Superblock structure
struct SuperBlock {
    UInt32 magic;               // Magic number
    UInt32 version;             // Filesystem version
    UInt32 block_size;          // Block size
    UInt32 total_blocks;        // Total blocks
    UInt32 free_blocks;         // Free blocks
    UInt32 total_inodes;        // Total inodes
    UInt32 free_inodes;         // Free inodes
    UInt32 first_data_block;    // First data block
    UInt32 inode_table_block;   // Inode table location
    UInt32 block_bitmap_block;  // Block bitmap location
    UInt32 inode_bitmap_block;  // Inode bitmap location
    time_t mount_time;            // Last mount time
    time_t write_time;            // Last write time
    UInt16 mount_count;         // Mount count
    UInt16 max_mount_count;     // Maximum mount count
    UInt16 state;               // Filesystem state
    UInt32 checksum;            // Superblock checksum
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
struct FileNode {
    Inode inode;                    // Inode data
    char *name;                     // Filename
    FileNode *parent;                   // Parent directory
    FileNode *children;                 // Child files (for directories)
    FileNode *next;                     // Next sibling

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
    UChar *bytes = static_cast<UChar *>(bitmap);
    bytes[bit / 8] &= ~(1 << (bit % 8));
}

// Hash table for fast filename lookup
#define HASH_TABLE_SIZE 1024

struct HashEntry {
    char *name;
    FileNode *file;
    HashEntry *next;
};

struct HashTable {
    HashEntry *buckets[HASH_TABLE_SIZE];
    Size semaphore;
};

// Calculate hash for filename
static inline unsigned int hash_filename(const char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash * 31) + *name++;
    }
    return hash % HASH_TABLE_SIZE;
}

} // Enf of FS