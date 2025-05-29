//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <FS/CommonTypes.h>

#include <vector>

namespace FS {
    // Struct for inode
    struct Inode {
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
} // FS