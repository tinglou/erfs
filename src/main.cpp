#include "rfs_generator.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    int result = 0;
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <src_dir> <id> <dest_file>" << std::endl;
        return 1;
    }
    result = generate_rfs(argv[1], argv[2], 0, argv[3]);
    return result;
}