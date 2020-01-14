#pragma once

#include "resource_fs.h"

#if defined(__cplusplus)
extern "C" {
#endif


///
/// access api
///
enum RfsGenStatusCode {
    RFS_GEN_OK                 = 0,
    RFS_SOURCE_TOO_LARGE        = -100,
    RFS_TARGET_NOT_EXIST        = -101,
    RFS_INVALID_ID              = -102,
    RFS_INVALID_OPTION          = -103,
};

///
/// generate RFS .c source file
///@param path the directory or file to be embedded
///@param id identity of the FS, format: [a-z][a-z_0-9]*
///@param option e.g. gzip text files
///@param target_dir target directory 
int generate_rfs(const char *path, const char *id, int options, const char *target_dir);


#if defined(__cplusplus)
}
#endif