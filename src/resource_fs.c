#include "resource_fs.h"

#define CHECK_NULL(V)       if(V == 0) {return RFS_INVALID_INPUT;}

/// read a regular file
///@param fs the file system
///@param path the file name to read
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_read(const RfsFileSystem *fs, const uint8_t *path, const uint8_t **out, uint32_t *size) {
    RfsHandle handle;
    int result = rfs_open (fs, path, &handle, size);
    if (result != 0) {
        return result;
    }
    if ((handle->flags & RFS_DIRECTORY) != 0) {
        return RFS_NOT_FILE;
    }
    *out = fs->data + handle->data_offset;
    // *size = handle->data_size;
    return RFS_OK;
}


static int strcmp_withlength (const uint8_t *s1, int l1, const uint8_t *s2, int l2) {
    int minlen;
    int cmp;
    minlen = (l1 > l2)? l1 : l2;
    for(int i = 0; i < minlen; i++){
        if(*s1 < *s2) {
            return -1;
        } else if(*s1 > *s2){
            return 1;
        }
    }
    if(l1 < l2) {
        return -1;
    } else if (l1 > l2) {
        return 1;
    }
    return 0;
}

static int rfs_find(const RfsFileSystem *fs, const RfsHandle handle, const uint8_t *name, int len, RfsHandle *out) {
    RfsHandle A = fs->entries + handle->data_offset;
    int L = 0;
    int R = (handle->data_size - 1);
    int m;

    RfsHandle mentry;
    uint8_t *mname;
    int mlen;
    int cmp;
    while (L <= R) {
        m = (L + R) / 2;
        mentry = A + m;
        mname = fs->data + mentry->name_offset;
        mlen = mentry->name_size;
        cmp = strcmp_withlength(mname, mlen, name, len);
        if (cmp < 0) {
            L = m + 1;
        } else if (cmp > 0) {
            R = m - 1;
        } else {
            *out = mentry;
            return RFS_OK;
        }
    }

    return RFS_NOT_FOUND;
}


/// open a FS entry
/// don't support "/../" or "/./"
///@param fs the file system
///@param path the file name to read
///@param out handle
///@param size file size or entries in the directory
///@return 0 for success; other for notfound
int rfs_open(const RfsFileSystem *fs, const uint8_t *path, RfsHandle *out, uint32_t *size) {
    CHECK_NULL(fs);
    CHECK_NULL(path);
    CHECK_NULL(out);
    CHECK_NULL(size);
    // if first character is '/', ignore
    const uint8_t *pos = path;
    if (*pos == '/') {
        pos++;
    }

    // open root dir
    RfsHandle dir = fs->entries;
    if(*pos == 0){
        *out = dir;
        *size = dir->data_size;
        return RFS_OK;
    }

    int result;
    RfsHandle entry;
    const uint8_t *start = pos;
    int len;
    while (*pos != 0) {
        while(*pos != '/' && *pos != 0) pos++;
        len = pos - start;

        result = rfs_find(fs, dir, start, len, &entry);
        if (result != RFS_OK) {
            return result;
        }
        if (*pos == 0) {
            // path end
            break;
        }
        if ((entry->flags & RFS_DIRECTORY) == 0) {
            // path isn't end but reach a regular file
            return RFS_NOT_FOUND;
        }
        pos++;
        start = pos;
    }

    *out = entry;
    *size = entry->data_size;
    return RFS_OK;
}

/// get flags of an entry (directry or file)
///@param entry entry (directry or file)
///@return flags
uint32_t rfs_entryflags(const RfsHandle handle) {
    CHECK_NULL(handle);
    return handle->flags;
}

/// get name of an entry (directry or file)
///@param fs the file system
///@param handle entry (directry or file)
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_entryname(const RfsFileSystem *fs, const RfsHandle handle, const uint8_t **out, uint32_t *size) {
    CHECK_NULL(fs);
    CHECK_NULL(handle);
    CHECK_NULL(out);
    CHECK_NULL(size);
    *out = fs->data + handle->name_offset;
    *size = handle->name_size;
    return RFS_OK;
}

/// read a regular file
///@param fs the file system
///@param handle the file name
///@param out pointer to the content
///@param size file size
///@return 0 for success; other for notfound
int rfs_readfile(const RfsFileSystem *fs, const RfsHandle handle, const uint8_t **out, uint32_t *size) {
    CHECK_NULL(fs);
    CHECK_NULL(handle);
    CHECK_NULL(out);
    CHECK_NULL(size);
    if ((handle->flags & RFS_DIRECTORY) != 0) {
        return RFS_NOT_FILE;
    }
    *out = fs->data + handle->data_offset;
    *size = handle->data_size;
    return RFS_OK;
}

/// get flags of an entry (directry or file)
///@param fs the file system
///@param handle directory to read
///@param index entry index
///@param out out entry
///@return 0 for success; other for notfound
int rfs_readdir(const RfsFileSystem *fs, const RfsHandle handle, uint32_t index, RfsHandle *out){
    CHECK_NULL(fs);
    CHECK_NULL(handle);
    CHECK_NULL(out);
    if ((handle->flags & RFS_DIRECTORY) == 0) {
        return RFS_NOT_DIRECTORY;
    }
    if (index >= handle->data_size) {
        return RFS_OUTOF_BOUND;
    }
    *out = fs->entries + handle->data_offset;
    return RFS_OK;
}


int rfs_travel_itr(const RfsFileSystem *fs, RfsHandle handle, rfs_visit func, void* ctx) {
    int result = 0;

    if ((handle->flags & RFS_DIRECTORY) != 0) {
        result = (*func)(fs, handle, RFS_TRAVEL_DIR_ENTER, ctx);
        if (result) {
            return result;
        }

        // directory entries
        int size = handle->data_size;
        for (int i = 0; i < size; i++) {
            result = rfs_travel_itr(fs, fs->entries + (handle->data_offset + i), func, ctx);
            if (result) {
                return result;
            }
        }

        result = (*func)(fs, handle, RFS_TRAVEL_DIR_LEAVE, ctx);
        if (result) {
            return result;
        }
    } else {
        result = (*func)(fs, handle, RFS_TRAVEL_FILE, ctx);
        if (result) {
            return result;
        }
    }
    return result;
}

/// travel the resource filesystem
///@param fs the file system
///@param func callback function
///@param ctx context
///@return 0 for success; other for notfound
int rfs_travel(const RfsFileSystem *fs, rfs_visit func, void* ctx) {
    CHECK_NULL(fs);
    CHECK_NULL(func);

    RfsHandle dir = fs->entries;
    CHECK_NULL(dir);

    return rfs_travel_itr(fs, dir, func, ctx);
}
