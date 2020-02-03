#pragma once

#if !defined(__RESOURCE_FS_H__)
#define __RESOURCE_FS_H__

#if defined(__RFS_IMPL__)
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif

#pragma pack(1)

/// a directory or file
typedef struct  {
    uint32_t name_offset;
    uint32_t name_size;
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t flags;
} RfsEntry;

typedef const RfsEntry* RfsHandle;

/// the whole resource filesystem
typedef struct {
    // all entries including directories and files
    uint32_t entry_count;
    RfsEntry *entries;

    // buffer to hold all names and contents
    uint32_t data_size;
    uint8_t  *data;
} RfsFileSystem;

typedef const RfsFileSystem * RfsRoot;
#pragma pack()

#if defined(__cplusplus)
}
#endif

#else // defined(RFS_IMPL)
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef const void* RfsRoot;
typedef const void* RfsHandle;
#endif // defined(RFS_IMPL)

#if defined(__cplusplus)
extern "C" {
#endif

///
/// RFSEntry flags
///
enum RfsEntryFlags {
    RFS_DIRECTORY       = 1,
    RFS_GZIPPED         = 2, 
};

///
/// return code of access api
///
enum RfsStatusCode {
    RFS_OK                      = 0,
    RFS_INVALID_INPUT           = -1,
    RFS_NOT_FOUND               = -2,
    RFS_NOT_FILE                = -3,
    RFS_NOT_DIRECTORY           = -4,
    RFS_OUTOF_BOUND             = -5,
};

/// read a regular file
///@param fs the file system
///@param path the file name to read
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_read(const RfsRoot fs, const uint8_t *path, const uint8_t **out, uint32_t *size);

/// open a FS entry
///@param fs the file system
///@param path the file name to read
///@param out handle
///@param size file size or entries in the directory
///@return 0 for success; other for notfound
int rfs_open(const RfsRoot fs, const uint8_t *path, RfsHandle *out, uint32_t *size);

/// get flags of an entry (directry or file)
///@param entry entry (directry or file)
///@return flags
uint32_t rfs_entryflags(const RfsHandle entry);

/// get name of an entry (directry or file)
///@param fs the file system
///@param entry entry (directry or file)
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_entryname(const RfsRoot fs, const RfsHandle entry, const uint8_t **out, uint32_t *size);

/// read a regular file
///@param fs the file system
///@param entry the file name
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_readfile(const RfsRoot fs, const RfsHandle entry, const uint8_t **out, uint32_t *size);

/// get flags of an entry (directry or file)
///@param fs the file system
///@param dir directory to read
///@param index entry index
///@param out out entry
///@return 0 for success; other for notfound
int rfs_readdir(const RfsRoot fs, const RfsHandle dir, uint32_t index, RfsHandle *out);


enum RfsTravelType {
    RFS_TRAVEL_DIR_ENTER,
    RFS_TRAVEL_DIR_LEAVE,
    RFS_TRAVEL_FILE,
};

typedef int (*rfs_visit) (const RfsRoot fs, const RfsHandle entry, enum RfsTravelType type, void* ctx);

/// travel the resource filesystem
///@param fs the file system
///@param func callback function
///@param ctx context
///@return 0 for success; other for notfound
int rfs_travel(const RfsRoot fs, rfs_visit func, void* ctx);

#if defined(__cplusplus)
}
#endif

#endif // !defined(__RESOURCE_FS_H__)