#include "gzip_file.h"
#include "zlib.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

#define CHUNK 16384

namespace fs = std::filesystem;

static const std::set<std::string> gzip_blacklist = {
    // compressed file
    "zip",
    "gz",
    "tgz",
    "rar",

    // media audio
    "flac",
    "mp3",
    "ogg",
    
    // media video
    "avi",
    
    // media image
    "jpg",
    "jpeg",
    "heic",
    "png",
    "gif"
};

///
/// @return 0:success, -1:failed to open file to read; -2: failed to open file to write; -3: compress fail; -4: needn't compress
///
int gzip_file(const char* source_path, const char* dest_path) {
    int ret = 0;

    fs::path source(source_path);
    fs::path dest(dest_path);
    int source_size = fs::file_size(source);

    if (source_size < 1024) {
        ret = RFS_GZIP_COMPRESS_RATIO;
        return ret;
    }

    auto ext = source.extension().string();
    if (ext.length() > 0) {
        ext = ext.substr(1);
    }

    if (gzip_blacklist.find(ext) != gzip_blacklist.end()) {
        ret = RFS_GZIP_COMPRESS_RATIO;
        return ret;
    } 

    std::ifstream ifs (source, std::ios::binary);

    gzFile file = gzopen(dest_path, "wb9");
    if (file == NULL) {
        ret = RFS_GZIP_DEST_NOT_FOUND;
        return ret;
    }
    unsigned char buf[CHUNK];
    int len;

    for(;;) {
        ifs.read(reinterpret_cast<char*>(buf), CHUNK);
        len = ifs.gcount();
        if (len == 0) {
            break;
        }
        if (gzwrite(file, buf, (unsigned)len) != len) {
            ret = RFS_GZIP_COMPRESS_FAIL;
            break;
        }
    } 
    if (gzclose(file) != Z_OK) {
        ret = RFS_GZIP_COMPRESS_FAIL;
        return ret;
    }

    int dest_size = fs::file_size(dest);
    if (((float)dest_size) / ((float)source_size) > 0.8) {
        ret = RFS_GZIP_COMPRESS_RATIO;
    }

    return ret;
}

