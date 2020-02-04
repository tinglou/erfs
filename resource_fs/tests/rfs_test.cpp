#include "gtest/gtest.h"
#include "resource_fs.h"

#include "rfs_rfsrc.h"


namespace {
const RfsRoot fs = rfs_rfsrc();


extern "C" int list_callback (const RfsRoot fs, const RfsHandle entry, enum RfsTravelType type, void* ctx) {
    int* indent = reinterpret_cast<int*>(ctx);
    int backup_indent = *indent;
    const uint8_t* name;
    uint32_t name_len;
    rfs_entryname(fs, entry, &name, &name_len);

    if (type == RFS_TRAVEL_DIR_ENTER) {
        // enter directory
        (*indent)++;

        for (int i = 0; i < backup_indent; i++) {
            std::cout << "  ";
        }
        std::cout << std::string((char*)name, (int)name_len)  << " [DIR]" << std::endl;
    } else if (type == RFS_TRAVEL_DIR_LEAVE) {
        // leave directory
        (*indent)--;
    } else {
        // file
        for (int i = 0; i < backup_indent; i++) {
            std::cout << "  ";
        }
        uint32_t flags;
        rfs_entryflags(entry, &flags);
        std::cout << std::string((char*)name, (int)name_len) << ((flags != 0) ? " [GZIPPED]" : "") << std::endl;
    }

    return 0;
}

TEST(RFS, travel) {
    int indent = 0;
    int result = rfs_travel(fs, list_callback, &indent);
    EXPECT_EQ(result, 0);
}

TEST(RFS, read_ok) {
    const uint8_t * buff;
    uint32_t size;
    int result = rfs_read(fs, (const uint8_t *)"/src/resource_fs.h", strlen("/src/resource_fs.h"), &buff, &size);
    EXPECT_EQ(result, RFS_OK);
}

TEST(RFS, read_fail) {
    const uint8_t * buff;
    uint32_t size;
    int result;
    
    result = rfs_read(fs, (const uint8_t *)"/hello.h", strlen("/hello.h"), &buff, &size);
    EXPECT_EQ(result, RFS_NOT_FOUND);

    result = rfs_read(fs, (const uint8_t *)"/", strlen("/"), &buff, &size);
    EXPECT_EQ(result, RFS_NOT_FILE);

    result = rfs_read(NULL, (const uint8_t *)"/", strlen("/"), &buff, &size);
    EXPECT_EQ(result, RFS_INVALID_INPUT);

    result = rfs_read(fs, (const uint8_t *)NULL, 0, &buff, &size);
    EXPECT_EQ(result, RFS_INVALID_INPUT);

    // treat "" as "/"
    result = rfs_read(fs, (const uint8_t *)"", 0, &buff, &size);
    EXPECT_EQ(result, RFS_NOT_FILE);
}


TEST(RFS, read_open_dir) {
    const uint8_t * buff;
    uint32_t size;
    RfsHandle handle;
    uint32_t flags;
    int result;

    result = rfs_open(fs, (const uint8_t *)"/", strlen("/"), &handle, &size);
    EXPECT_EQ(result, RFS_OK);    

    result = rfs_entryflags(handle, &flags);
    EXPECT_EQ(flags, RFS_DIRECTORY);    

}

TEST(RFS, read_open_file) {
    const uint8_t * buff;
    uint32_t size;
    RfsHandle handle;
    uint32_t flags;
    int result;

    result = rfs_open(fs, (const uint8_t *)"/src", strlen("/src"), &handle, &size);
    EXPECT_EQ(result, RFS_OK);    
    result = rfs_entryflags(handle, &flags);
    EXPECT_EQ(flags, RFS_DIRECTORY);  

    result = rfs_open(fs, (const uint8_t *)"/src/resource_fs.c", strlen("/src/resource_fs.c"), &handle, &size);
    EXPECT_EQ(result, RFS_OK);    
    result = rfs_entryflags(handle, &flags);
    EXPECT_EQ(flags, RFS_GZIPPED);    
}


} // namespace
