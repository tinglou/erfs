#pragma once

#if defined(__cplusplus)
extern "C" {
#endif


///
///
enum RfsGzipStatusCode {
    RFS_GZIP_OK                 = 0,
    RFS_GZIP_SRC_NOT_FOUND      = -1,
    RFS_GZIP_DEST_NOT_FOUND     = -2,
    RFS_GZIP_COMPRESS_FAIL      = -3,
    RFS_GZIP_COMPRESS_RATIO     = -4,
};

///
/// @return 0:success, -1:failed to open file to read; -2: failed to open file to write; -3: compress fail; -4: needn't compress
///
int gzip_file(const char* source_path, const char* dest_path);


#if defined(__cplusplus)
}
#endif