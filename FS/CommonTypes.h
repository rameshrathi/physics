//
// Created by ramesh on 29/05/25.
//

#pragma once

#include <string>

namespace FS {
    // Start of FS

    using ino_t = uint64_t;
    using mode_t = uint32_t;
    using nlink_t = uint16_t;
    using uid_t = uint32_t;
    using off_t = uint64_t;
    using time_t = uint64_t;
    using blkcnt_t = uint64_t;
    using gid_t = uint32_t;
    using size_t = uint64_t;
    using dev_t = uint64_t;
    using blksize_t = uint32_t;
    using fsblkcnt_t = uint64_t;
    using fsfilcnt_t = uint64_t;

    using UChar = unsigned char;

    /* ================================== */

    struct ExAttrs {
        std::string name;
        std::string value;
    };

}   // FS
