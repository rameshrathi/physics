# A typical Unix file system divides a partition into several regions
-------------------------------------------
[web.cs.wpi.edu](web.cs.wpi.edu)

#### -* For example:
Boot block (block 0): May contain bootloader code or boot record. It is usually the very first sector(s) of the partition
web.cs.wpi.edu
.
Superblock: Immediately after the boot block. It stores metadata about the file system: total number of blocks, block size, number of inodes, number of free blocks, pointer to free blocks/inodes, root directory inode number, a magic signature, etc
web.cs.wpi.edu
github.com
. The superblock is used to verify and mount the filesystem (e.g. checking a magic number like 0xF0F03410)
www3.nd.edu
github.com
.
Inode table: A contiguous array of inodes follows the superblock
web.cs.wpi.edu
. Each inode is a fixed-size struct containing file metadata (type, permissions, timestamps, size, link count) and pointers to data blocks
web.cs.wpi.edu
. Inodes do not contain file names or paths; those live in directories. As a rule of thumb, about 10% of the filesystem blocks may be reserved for inodes (as in ND SimpleFS, which uses 10% of blocks for inodes
www3.nd.edu
). For example, ND’s SimpleFS uses 4 KB blocks, with 128 inodes per block (32 bytes each)
www3.nd.edu
.
Data blocks: The rest of the space is divided into fixed-size data blocks (e.g. 4 KB each) that hold file and directory contents
web.cs.wpi.edu
. Files grow by allocating and writing to these data blocks. (Indirect blocks can be used for large files.) A free-block bitmap or list is maintained (in-memory or on-disk) to track unused blocks
www3.nd.edu
.
The on-disk layout looks conceptually like this:
+-----------+-----------+-----------+-----------+
| Boot Block| Superblock| Inode Table (array of inodes) |
+-----------+-----------+-----------+           +-----------+
| Data Blocks ... |
+-----------+
A superblock example from ND’s SimpleFS project shows fields: magic number, total blocks, inode-block count, and total inodes
www3.nd.edu
. Inodes typically contain a valid bit, size, a number of direct block pointers (e.g. 5), and an indirect pointer
www3.nd.edu
. For example, one inode struct might be:
struct Inode {
uint32_t valid;           // 1 if in use, 0 if free
uint32_t size;            // file size in bytes
uint32_t direct[5];       // direct block pointers (or fewer/more as desired)
uint32_t indirect;        // pointer to an indirect block (if needed)
};
Each Inode thus points to actual data blocks. Direct pointers hold block numbers, and an indirect block is a block full of additional pointers (e.g. 1024 pointers if block size is 4 KB and pointers are 4 bytes)
www3.nd.edu
. An inode also contains typical metadata (owner, permissions, timestamps), but those can be omitted or simplified in a minimal implementation
web.cs.wpi.edu
. Directory contents are themselves stored in data blocks pointed to by directory inodes. A directory entry maps a file name to an inode number. (This structure is essentially a small record with a file name and inode index.) By storing the boot record, superblock, inodes, and data blocks in known locations, the operating system can mount the file system by reading the superblock (verifying the magic number), then accessing inodes and data blocks by their calculated offsets
web.cs.wpi.edu
www3.nd.edu
.
Key Classes and Data Structures
We organize the implementation into classes that reflect these on-disk structures and operations. The main classes are:
Superblock Class
The Superblock class represents the file system metadata stored at the start of the disk. It typically includes fields like:
Magic number (filesystem signature).
Total number of blocks on the device.
Number of blocks reserved for the inode table.
Total number of inodes.
Counts of free blocks or free inodes (optional if using bitmaps).
(Optionally) pointers to root directory inode, free-block lists, etc.
For example:
struct Superblock {
static constexpr uint32_t MAGIC = 0xF0F03410;  // filesystem signature
uint32_t totalBlocks;   // total number of blocks on the device
uint32_t inodeBlocks;   // number of blocks reserved for inodes
uint32_t totalInodes;   // total number of inodes
uint32_t freeBlocks;    // count of free data blocks (optional)
// ... add more fields as needed
};
On format, we fill a Superblock with appropriate values (and write it to block 0). On mount, we read block 0 into a Superblock instance and check superblock.magic == Superblock::MAGIC. The superblock’s fields (total blocks, inode blocks, etc.) tell us how to interpret the rest of the disk
github.com
www3.nd.edu
. For example, SimpleFS uses a magic 0xF0F03410 at byte 0 of block 0
www3.nd.edu
, and fields for total blocks, inode blocks, and total inodes.
Inode Class
The Inode class models a file or directory inode. It stores metadata and block pointers. A simple C++ struct might be:
struct Inode {
bool valid;             // true if this inode is allocated
uint32_t size;          // file size in bytes
uint32_t direct[NUM_DIRECT];  // direct block pointers
uint32_t indirect;      // block number of indirect pointer block (or 0)
// (Optionally: owner, permissions, timestamps, etc.)
};
For example, if we use NUM_DIRECT = 5, each inode has 5 direct pointers and 1 indirect pointer, similar to ND’s SimpleFS
www3.nd.edu
. The valid flag indicates if the inode is in use. The size field tracks the file length. The direct array contains block numbers (e.g. block indices) where file data is stored. A 0 can represent a null pointer (no block allocated). The indirect field, if non-zero, is the block number of an indirect block that itself contains an array of more data block pointers
www3.nd.edu
. Inodes do not contain file names or paths. Those are managed by directories. The inode holds only metadata (type, permissions) and block pointers
web.cs.wpi.edu
. By storing inodes in a fixed area on disk, the file system can read and write them by simple arithmetic (e.g. inode N is at block (inodeStartBlock + N / inodesPerBlock), offset (N % inodesPerBlock) * sizeof(Inode)).
DirectoryEntry Class
A directory file contains a list of entries mapping names to inodes. We represent each entry with a simple struct:
struct DirEntry {
char name[MAX_NAME_LEN]; // file or directory name (e.g. 255 chars)
uint32_t inode;          // inode number for this entry
};
When a directory inode is read, its data blocks are treated as arrays of DirEntry. (In a real FS, directory entries may be packed/variable-length, but a simple fixed-size struct works for small demos.) The root directory is an inode (often inode 0 or 1) whose blocks start with an entry for "." (itself) and ".." (parent), plus other entries. Directories store the filesystem hierarchy. The file system must lookup paths by parsing directory entries: split /a/b/c, find "a" in the root dir, get its inode, open that directory and find "b", etc. Each DirEntry connects a name to an inode number. This matches the Unix convention that directories map file names to inodes
geeksforgeeks.org
.
BlockManager Class
The BlockManager (or similar) handles low-level block allocation and I/O. Its responsibilities include:
Initializing access to the underlying device (file or block device path).
Reading and writing blocks (fixed-size, e.g. 4096 bytes).
Managing free blocks: tracking which blocks are free or used (often via a bitmap or free list).
Allocating/freeing blocks: returning a free block number when needed, or releasing a block.
A sketch of its interface:
class BlockManager {
public:
BlockManager(const std::string &devicePath);
bool init();            // open device, read superblock, etc.
int allocBlock();       // allocate a free block, return block index or -1
void freeBlock(int blk);
void readBlock(int blk, char *buffer);
void writeBlock(int blk, const char *buffer);
// ...
private:
int fd;                // file descriptor for the device
std::vector<bool> freeMap; // in-memory free-block bitmap
Superblock superblock;
// ...
};
Internally, init() might open the device (using open() or fopen()), read the superblock (using read() or pread()), and then initialize the free-block map (for example, marking all blocks as free except superblock/inode blocks)
www3.nd.edu
www3.nd.edu
. The free-block bitmap can be stored in memory and rebuilt at mount time by scanning the inode table if no on-disk bitmap is kept
www3.nd.edu
www3.nd.edu
. To read a block: seek to blk * BLOCK_SIZE and read BLOCK_SIZE bytes into a buffer. To write: similarly, seek and write. For example, one can use the POSIX calls pread(fd, buffer, BLOCK_SIZE, offset) and pwrite(fd, buffer, BLOCK_SIZE, offset) for block I/O (they handle seeking internally). The StackOverflow discussion confirms using open(), read(), and write() (or the unbuffered pread/pwrite) to access a device
stackoverflow.com
:
“Use open(2), read(2), and write(2) to read from and write to the device”
stackoverflow.com
.
When writing in C++, you can also use std::fstream opened in binary mode, but be careful to disable buffering (e.g. ofstream.rdbuf()->pubsetbuf(nullptr, 0)) so that data is written immediately
stackoverflow.com
. In low-level file system code, it is common to use the POSIX calls directly.
FileSystem Class
The FileSystem class ties everything together and provides the high-level API: mounting, formatting, and file/directory operations. Internally it holds a Superblock, a list of Inode objects, a free-block map (often via BlockManager), and perhaps an in-memory open-file table. Example interface outline:
class FileSystem {
public:
FileSystem();
bool formatDisk(const std::string &devicePath, uint32_t totalBlocks);
bool mount(const std::string &devicePath);
int  createFile(const std::string &path);
bool deleteFile(const std::string &path);
int  readFile(int inum, char *buffer, size_t count, size_t offset);
int  writeFile(int inum, const char *buffer, size_t count, size_t offset);
int  makeDirectory(const std::string &path);
bool removeDirectory(const std::string &path);
// ... other operations ...
private:
Superblock superblock;
std::vector<Inode> inodes;
std::vector<bool> blockFreeMap;
BlockManager blockMgr;
std::unordered_map<int, std::string> openFiles; // if tracking fds
// helper functions (e.g. find free inode, path parsing, etc.)
};
formatDisk: Create a new filesystem image. Set aside inode blocks (e.g. 10% of total blocks) and clear the inode table. Initialize and write the superblock (with magic number and counts)
www3.nd.edu
.
mount: Open the device, read superblock, verify it, and prepare in-memory structures. For example, rebuild the free-block bitmap by scanning all inodes (to see which blocks are in use)
www3.nd.edu
. After mounting, the file system is ready for operations.
createFile/deleteFile: Allocate or free an inode and its blocks. For create, find a free inode (set valid = true, size = 0) and add a directory entry for it. For delete, remove it from its parent directory and free all its data blocks and the inode.
readFile/writeFile: Given an inode number, read or write data at a given offset. These methods traverse the direct (and indirect) pointers. For write, allocate new blocks if needed (updating inodes[inum].direct[] or the indirect block) and update the size. For read, copy from the existing data blocks into the buffer. (ND’s SimpleFS spec outlines this: “read” returns up to length bytes from an inode; “write” may allocate blocks and returns bytes written
www3.nd.edu
www3.nd.edu
.)
makeDirectory/removeDirectory: Similar to files, but create a directory inode and initialize its contents with “.” and “..” entries.
The code for these methods will call into BlockManager to read/write blocks and manipulate the inode and directory structures. For example, the mount operation might look like:
bool FileSystem::mount(const std::string &devicePath) {
blockMgr = BlockManager(devicePath);
if (!blockMgr.init()) return false;
// Read superblock (assume block 0)
Superblock sb;
blockMgr.readBlock(0, reinterpret_cast<char*>(&sb));
if (sb.MAGIC != Superblock::MAGIC) return false;
superblock = sb;
// Load all inodes and build free-block map
// (e.g. scan inode blocks for valid inodes and mark their data blocks used)
// ...
return true;
}
This aligns with ND’s spec: “read the superblock, build a free block bitmap, and prepare the filesystem for use”
www3.nd.edu
.
Data Storage on Block Devices (e.g. USB Drives)
A USB flash drive (or SD card, external HDD, etc.) is treated by the OS as a block device. The operating system represents it with a device file (like /dev/sdb or /dev/sdc1 for a partition). The disk is divided into sectors (usually 512 bytes) but filesystems operate on larger blocks (common sizes are 4096 bytes). When we implement our filesystem in C++, we can either use a regular file as a virtual disk image or access the real device file for the USB drive. Key points:
Partitions: Disks can be partitioned (using an MBR/GPT). Each partition is a standalone filesystem
web.cs.wpi.edu
. You can format a whole device (no partition table) or a partition. If using a partition, you would offset by its starting sector. For simplicity, one often uses the whole device (no partition table) as the filesystem, writing a superblock at block 0.
Device Files: On Linux, you can open a block device (e.g. "/dev/sdb") with open() just like a regular file
web.cs.wpi.edu
stackoverflow.com
. Your program must have appropriate permissions (usually root). On Windows, you might use special APIs (beyond this scope).
Sector vs Block: We read/write in blocks (e.g. 4 KB). Underneath, the OS will break this into sector reads/writes. You can also use a file on an existing filesystem as the “disk image” (e.g. disk.img). In that case, you open the file and use pread/pwrite to simulate block I/O.
Endianness: Most PCs are little-endian. When writing multibyte integers to disk, be consistent. On the same machine you may not need byte-swapping, but it is good practice to document endianness if interoperability is required.
To the file system code, a USB or SD card is just an addressable sequence of blocks. We use our BlockManager to abstract away the details of raw device I/O
stackoverflow.com
. For example, on Linux you can open a block device:
int fd = open("/dev/sdb", O_RDWR);
if (fd < 0) { /* handle error */ }
// Read block 5
const int BLOCK_SIZE = 4096;
char buf[BLOCK_SIZE];
if (pread(fd, buf, BLOCK_SIZE, 5*BLOCK_SIZE) != BLOCK_SIZE) {
// handle read error
}
This reads the 6th 4 KB block from the USB. Writing is similar with pwrite. After operations, close the device with close(fd).
Raw Block I/O in C++
Interacting with a block device or disk image in C++ is done via file I/O at the block level. The typical system calls are open, read/pread, write/pwrite, and close (all from <fcntl.h> and <unistd.h> on POSIX). As noted above, a StackOverflow answer advises using these calls for raw device access
stackoverflow.com
:
“Use open(2), read(2), and write(2) to read from and write to the device. … You can also use fopen(3) or C++ fstreams, but if you do, disable buffering.”
stackoverflow.com
Example C++ code for block-level I/O:
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const int BLOCK_SIZE = 4096;

int openDevice(const std::string &path) {
int fd = open(path.c_str(), O_RDWR);
if (fd < 0) { perror("open"); /* handle error */ }
return fd;
}

void readBlock(int fd, int blockNum, char *buffer) {
off_t offset = (off_t)blockNum * BLOCK_SIZE;
ssize_t n = pread(fd, buffer, BLOCK_SIZE, offset);
if (n < 0) { perror("pread"); /* handle error */ }
else if (n != BLOCK_SIZE) {
// handle partial read (e.g. end of file)
}
}

void writeBlock(int fd, int blockNum, const char *buffer) {
off_t offset = (off_t)blockNum * BLOCK_SIZE;
ssize_t n = pwrite(fd, buffer, BLOCK_SIZE, offset);
if (n < 0) { perror("pwrite"); /* handle error */ }
else if (n != BLOCK_SIZE) {
// handle partial write
}
}
This code opens a device (or image file) and reads/writes fixed-size blocks. Using pread/pwrite is convenient since it does not change the file’s current offset. Remember to close(fd) when done. If using C++ streams, you could do:
std::fstream disk;
disk.open(path, std::ios::in | std::ios::out | std::ios::binary);
disk.seekg(blockNum * BLOCK_SIZE);
disk.read(buffer, BLOCK_SIZE);
disk.seekp(blockNum * BLOCK_SIZE);
disk.write(buffer, BLOCK_SIZE);
But the C++ stream approach may be buffered internally; to ensure no caching, disable buffering or use disk.rdbuf()->pubsetbuf(nullptr,0). For simplicity, the POSIX calls are preferred.
File System Operations
With the basic classes in place, we implement file system operations. Key operations include mounting, file creation/deletion, file read/write, and directory management. We organize these as methods on the FileSystem class. Here are the core steps:
Mounting the File System
Mounting prepares the file system for use (makes it “active”). Steps:
Open the device. Use open(devicePath) via BlockManager or similar.
Read the superblock. Read block 0 and deserialize it into a Superblock object. Check the magic number to ensure it’s our FS
www3.nd.edu
.
Initialize in-memory structures. Based on the superblock, allocate memory for all inodes and the free-block bitmap. For example, if inodeBlocks = 10, load 10 blocks of inodes into memory or map them.
Build free-block map. If no on-disk free-block bitmap is stored, scan all inodes: for each valid inode, mark its direct and indirect blocks as used. Mark superblock and inode table blocks as used. Everything else is free. (ND’s SimpleFS rebuilds the free map on each mount by scanning inodes
www3.nd.edu
www3.nd.edu
.)
After this, the file system is mounted, and we can perform create/read/write/delete operations.
Example code snippet (conceptual):
bool FileSystem::mount(const std::string &devicePath) {
// Initialize block manager
if (!blockMgr.openDevice(devicePath)) return false;
// Read superblock from block 0
Superblock sb;
blockMgr.readBlock(0, reinterpret_cast<char*>(&sb));
if (sb.MAGIC != Superblock::MAGIC) {
std::cerr << "Invalid filesystem magic\n";
return false;
}
superblock = sb;
// Load inode table into memory
inodes.resize(sb.totalInodes);
for (uint32_t b = 0; b < sb.inodeBlocks; b++) {
char buffer[BLOCK_SIZE];
blockMgr.readBlock(1 + b, buffer); // inode blocks start at block 1
// Copy buffer into inodes[b * (BLOCK_SIZE / sizeof(Inode)) ...]
// ...
}
// Build free-block map by scanning inodes
blockFreeMap.assign(sb.totalBlocks, true);
// Mark superblock and inode table blocks as used:
blockFreeMap[0] = false;
for (uint32_t i = 1; i <= sb.inodeBlocks; i++) blockFreeMap[i] = false;
// For each valid inode, mark its blocks:
for (const Inode &ino : inodes) {
if (!ino.valid) continue;
for (int d = 0; d < NUM_DIRECT; d++) {
if (ino.direct[d] != 0) blockFreeMap[ino.direct[d]] = false;
}
if (ino.indirect != 0) {
// read indirect block, mark its pointers
char buf[BLOCK_SIZE];
blockMgr.readBlock(ino.indirect, buf);
uint32_t *ptrs = reinterpret_cast<uint32_t*>(buf);
int nptrs = BLOCK_SIZE / sizeof(uint32_t);
for (int i = 0; i < nptrs; i++) {
if (ptrs[i] != 0) blockFreeMap[ptrs[i]] = false;
}
blockFreeMap[ino.indirect] = false;
}
}
return true;
}
This follows the SimpleFS outline: “read the superblock, build a free block bitmap, and prepare the filesystem for use”
www3.nd.edu
.
Creating and Deleting Files
Creating a file (createFile): Given a path, we must allocate a new inode and add a directory entry. Steps:
Parse the path to separate parent directory and new file name.
Find the parent directory inode. Read the path components to locate the parent directory.
Find a free inode. Scan the in-memory inode array for valid == false. Mark it valid = true and size = 0. Initialize its direct pointers to 0.
Add a directory entry. In the parent directory’s data blocks, write a new DirEntry with the file’s name and the new inode number.
Update metadata. Write the parent directory block(s) back to disk, and write the new inode to disk.
ND’s spec: “Creates a new inode of zero length. On success, return the inumber.”
www3.nd.edu
. In our interface, we might return a file descriptor or inode number. Example (conceptual):
int FileSystem::createFile(const std::string &path) {
int parentInum = lookupParent(path);
std::string fname = basename(path);
int freeInum = findFreeInode();
if (freeInum < 0) return -1; // no free inode

    Inode &inode = inodes[freeInum];
    inode.valid = true;
    inode.size = 0;
    for(int i=0;i<NUM_DIRECT;i++) inode.direct[i] = 0;
    inode.indirect = 0;
    // Write inode to disk
    writeInodeToDisk(freeInum);

    // Create directory entry in parent
    DirEntry entry;
    strncpy(entry.name, fname.c_str(), MAX_NAME_LEN);
    entry.inode = freeInum;
    writeDirEntry(parentInum, entry);

    return freeInum;
}
Deleting a file (deleteFile): Given a path (or inode number), remove it:
Lookup the file’s inode and parent directory.
Remove directory entry. Delete the entry for this file from the parent directory’s blocks.
Free all its blocks. For each non-zero pointer in the inode (direct and indirect), mark those blocks free (blockFreeMap[block] = true).
Free indirect block. If the inode has an indirect block, also free that block.
Invalidate the inode. Set inode.valid = false. Optionally zero out inode on disk.
Update superblock free count (if tracked).
ND’s spec: “removes the inode indicated by the inumber. It should release all data and indirect blocks and return them to the free block map.”
www3.nd.edu
. That is exactly the algorithm above. For example:
bool FileSystem::deleteFile(int inum) {
if (inum < 0 || inum >= (int)inodes.size()) return false;
Inode &inode = inodes[inum];
if (!inode.valid) return false;
// Free data blocks
for (int i = 0; i < NUM_DIRECT; i++) {
if (inode.direct[i] != 0) {
blockFreeMap[inode.direct[i]] = true;
inode.direct[i] = 0;
}
}
if (inode.indirect != 0) {
// Free indirect block contents
char buf[BLOCK_SIZE];
blockMgr.readBlock(inode.indirect, buf);
uint32_t *ptrs = reinterpret_cast<uint32_t*>(buf);
int count = BLOCK_SIZE / sizeof(uint32_t);
for (int i = 0; i < count; i++) {
if (ptrs[i] != 0) {
blockFreeMap[ptrs[i]] = true;
}
}
blockFreeMap[inode.indirect] = true;
inode.indirect = 0;
}
inode.valid = false;
// Remove from parent directory (not shown)
// ...
return true;
}
This ensures all data blocks are reclaimed.
Reading and Writing Files
Reading (readFile): Given an inode (and an offset and length), we copy data from the file into a buffer:
Check inode validity. If not valid or offset beyond size, return 0 or error.
Calculate starting block. Determine which block corresponds to the given offset (blockIndex = offset / BLOCK_SIZE) and byte offset within that block.
Read blocks. For each block needed, use its direct pointer or via the indirect block to find the block number, then readBlock. Copy requested bytes into the user buffer.
Return bytes read. (May be less than requested if end of file is reached.)
Pseudo-code:
int FileSystem::readFile(int inum, char *buf, size_t count, size_t offset) {
Inode &inode = inodes[inum];
if (!inode.valid) return -1;
if (offset >= inode.size) return 0;
size_t bytesRead = 0;
while (count > 0 && offset < inode.size) {
int blknum = offset / BLOCK_SIZE;
int blockOffset = offset % BLOCK_SIZE;
int toRead = std::min((size_t)BLOCK_SIZE - blockOffset, count);
uint32_t blockIndex;
if (blknum < NUM_DIRECT) {
blockIndex = inode.direct[blknum];
} else {
// handle indirect block...
}
if (blockIndex == 0) break; // gap
char data[BLOCK_SIZE];
blockMgr.readBlock(blockIndex, data);
int actually = std::min(toRead, (int)(inode.size - offset));
memcpy(buf + bytesRead, data + blockOffset, actually);
bytesRead += actually;
offset += actually;
count -= actually;
}
return bytesRead;
}
ND’s read specification says: “copy length bytes from the data blocks of the inode into data, starting at offset. Return number of bytes read.”
www3.nd.edu
. Writing (writeFile): Similar to read, but we may allocate new blocks:
Check inode valid. If not valid, return error.
Calculate block and offset. Same as read.
If writing past current size, allocate blocks. For each block we need, if the direct pointer is 0, allocate a new block (allocBlock()) and update the inode. For writing beyond direct pointer range, use an indirect block (allocate if needed).
Write data to blocks. Use writeBlock for each block. Fill the gap with zeros if needed.
Update inode size. Increase inode.size if we wrote past the old end.
Return bytes written.
Pseudo-code:
int FileSystem::writeFile(int inum, const char *buf, size_t count, size_t offset) {
Inode &inode = inodes[inum];
if (!inode.valid) return -1;
size_t bytesWritten = 0;
while (count > 0) {
int blknum = offset / BLOCK_SIZE;
int blockOffset = offset % BLOCK_SIZE;
int toWrite = std::min((size_t)BLOCK_SIZE - blockOffset, count);
uint32_t blockIndex;
if (blknum < NUM_DIRECT) {
if (inode.direct[blknum] == 0) {
int newBlk = allocBlock();
if (newBlk < 0) break;
inode.direct[blknum] = newBlk;
}
blockIndex = inode.direct[blknum];
} else {
// allocate indirect block if needed...
}
char data[BLOCK_SIZE];
if (toWrite != BLOCK_SIZE) {
// partial block: read existing data (to preserve other bytes)
blockMgr.readBlock(blockIndex, data);
} else {
// full block write, no need to read
memset(data, 0, BLOCK_SIZE);
}
memcpy(data + blockOffset, buf + bytesWritten, toWrite);
blockMgr.writeBlock(blockIndex, data);
bytesWritten += toWrite;
offset += toWrite;
count -= toWrite;
if (offset > inode.size) inode.size = offset;
}
return bytesWritten;
}
ND’s write spec: “copy length bytes into data blocks, allocating any necessary blocks. Return number of bytes written.”
www3.nd.edu
. Notice that the actual number written may be less if disk is full. After writing, remember to write the updated inode (with new size and pointers) back to disk if the inode table is on disk.
Managing Directories
Directories are just special files containing DirEntry records. You can treat a directory inode like a file but with the invariant that each block holds directory entries.
Creating a directory: Similar to creating a file, but after allocating the inode, initialize it as a directory. Usually, you write two initial entries: "." pointing to itself, and ".." pointing to parent. The directory’s size is 2 entries (or block size if you allocate a block). You may maintain a type flag in the inode to mark it as directory.
Listing a directory: Read the directory’s data blocks and print the name fields of each DirEntry that has a non-zero inode.
Deleting a directory: Ensure it is empty (only . and .. exist), then free it like a file (release its inode and blocks), and remove its entry from the parent.
Directory entries can be linear (scan linearly until a free slot or matching name). When removing an entry, you can mark it free by setting inode = 0 in that DirEntry. These directory operations rely on the same block- and inode-management code as files, with the addition of parsing and writing name entries.
Utilities: Format and Mount
We should provide utility functions or programs to format a device with our filesystem and to mount (attach) it for use. Format Utility (mkfs): This initializes an empty file system on a block device or image. Steps:
Determine sizes. Decide total blocks (given by device size or passed as parameter) and number of inode blocks (e.g. 10% of total, rounding up)
www3.nd.edu
.
Clear all blocks. For safety, you can write zeros to the entire device (or at least overwrite superblock/inode areas).
Initialize superblock. Fill Superblock fields: set magic, totalBlocks, inodeBlocks, totalInodes, freeBlocks = totalBlocks - (1 + inodeBlocks), etc. Write this to block 0.
Initialize inode table. For each inode block (blocks 1..inodeBlocks), write a block of zeros or Inode structs with valid=false.
Free-block map. Optionally write a free-block bitmap (if you design one). If not, just remember that all data blocks beyond inode area are free.
Done. The device now contains an empty file system.
ND’s SimpleFS format requirement: “It should set aside ten percent of the blocks for inodes, clear the inode table, and write the superblock.”
www3.nd.edu
. Our code should follow that. A format example:
bool FileSystem::formatDisk(const std::string &devicePath, uint32_t totalBlocks) {
blockMgr.openDevice(devicePath);
Superblock sb;
sb.MAGIC = Superblock::MAGIC;
sb.totalBlocks = totalBlocks;
sb.inodeBlocks = std::max<uint32_t>(1, (totalBlocks + 9) / 10); // ~10%
sb.totalInodes = sb.inodeBlocks * (BLOCK_SIZE / sizeof(Inode));
sb.freeBlocks = totalBlocks - (1 + sb.inodeBlocks);
// Write superblock to block 0
blockMgr.writeBlock(0, reinterpret_cast<char*>(&sb));
// Clear inode blocks
Inode emptyInode;
memset(&emptyInode, 0, sizeof(emptyInode));
char buf[BLOCK_SIZE];
for (uint32_t b = 0; b < sb.inodeBlocks; b++) {
// fill block with empty inodes
Inode *inodeArr = reinterpret_cast<Inode*>(buf);
int inodesPerBlock = BLOCK_SIZE / sizeof(Inode);
for (int i = 0; i < inodesPerBlock; i++) inodeArr[i] = emptyInode;
blockMgr.writeBlock(1 + b, buf);
}
return true;
}
Mount Utility: Once formatted, we need a way to mount the FS (perhaps via a CLI or as part of the program startup). The mount utility would essentially perform FileSystem::mount, then enter an interactive mode or allow further commands (create, write, etc.). In a real OS, mounting attaches the FS to the global namespace. In our simple tool, “mounting” just means the program reads the superblock and is ready to perform operations on that device.
Using the System with a USB Drive
To use this file system on a USB drive, you can either create a disk image file or write directly to the USB device. For example, on Linux:
Create an image file (optional):
dd if=/dev/zero of=disk.img bs=4096 count=1000000
This makes a 4 GB file of zeros. You can run your format and file operations on disk.img.
Or use a USB device:
Suppose your USB appears as /dev/sdb. Warning: This will overwrite its contents.
sudo ./myfs_format /dev/sdb 400000
sudo ./myfs_mount /dev/sdb
The first command initializes the USB with our FS (assuming 400000 blocks). The second mounts and allows commands.
Accessing via OS: Since this is a custom FS not recognized by the OS, you generally cannot mount it in Linux (mount -t ourfs /dev/sdb /mnt) unless you write a FUSE or kernel driver. Instead, you interact via your program. For example:
sudo ./fshell /dev/sdb   # hypothetical shell for our FS
> format
> create /hello.txt
> write 0 "Hello, USB!"
> close 0
Here fshell is a custom command-line interface that uses your FileSystem class.
Alternatively, use a loopback:
You can mount the image file in Linux via a loop device if you implement a FUSE driver. This is advanced; for now, we assume usage through your code.
In summary, treating the USB as a block device means using open("/dev/sdb"), performing block I/O as above, and ensuring proper permissions. The principles are the same as with any disk: open, read/write at block offsets, close.
Testing: Unit and Integration Tests (GoogleTest)
Thorough testing is crucial. We recommend using a C++ testing framework like GoogleTest. GoogleTest makes it easy to write unit tests and integration tests that verify file operations work correctly
google.github.io
. For example, you might write tests for:
Inode and Superblock I/O: Verify that writing a superblock or inode to a simulated disk and reading it back yields the same values.
Block Allocation: Test that allocBlock() and freeBlock() manage the free-block map correctly.
File Operations: Test creating a file, writing data, reading it back, and deleting the file. E.g.:
#include <gtest/gtest.h>
#include "FileSystem.h"

TEST(FileSystemTest, CreateReadWrite) {
FileSystem fs;
fs.formatDisk("disk.img", 100);   // format 100-block disk
ASSERT_TRUE(fs.mount("disk.img"));

    // Create and write file
    int inum = fs.createFile("/foo.txt");
    ASSERT_NE(inum, -1);
    const char *msg = "Hello, world";
    EXPECT_EQ(fs.writeFile(inum, msg, 12, 0), 12);

    // Read it back
    char buf[20] = {0};
    EXPECT_EQ(fs.readFile(inum, buf, 12, 0), 12);
    EXPECT_STREQ(buf, "Hello, world");
}

TEST(FileSystemTest, OverwriteAndTruncate) {
FileSystem fs;
fs.formatDisk("disk.img", 100);
fs.mount("disk.img");

    int inum = fs.createFile("/data.bin");
    const std::string data = "ABCDEFGHIJ"; // 10 bytes
    fs.writeFile(inum, data.c_str(), data.size(), 0);

    // Overwrite part of it
    const std::string over = "XYZ";
    fs.writeFile(inum, over.c_str(), over.size(), 4);

    // Now file should be "ABCDXYZHIJ"
    char buf[20] = {0};
    fs.readFile(inum, buf, 10, 0);
    EXPECT_STREQ(buf, "ABCDXYZHIJ");
}
Directory Operations: Test creating directories, listing contents, and removing directories.
Mount/Persistence: Write a file, “unmount” (destroy the FileSystem object), then mount again and check the file still exists (simulating a reboot). This checks consistency and that metadata was flushed to disk.
Edge Cases: Test behavior when disk is full, or when trying to read beyond end of file, or deleting non-empty directories.
You can group tests in suites (TEST(FileSystemTest, ...)) for readability. GoogleTest supports assertions like ASSERT_TRUE, EXPECT_EQ, etc
google.github.io
. For integration testing (simulating crashes), a framework like CrashMonkey (from systems research) exists, but for a simple FS you might simulate power-loss by partially writing data and not updating metadata, then remounting to see if things stay consistent. At minimum, ensure that after normal operations, the filesystem is internally consistent and all invariants hold (no dangling blocks, correct sizes, etc.). GoogleTest is cross-platform and easy to set up. Its primer notes that “GoogleTest helps you write better C++ tests”
google.github.io
. You can run your tests automatically as part of a build, and it will report detailed failures.