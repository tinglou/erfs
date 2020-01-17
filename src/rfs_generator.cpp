#include "rfs_generator.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#define RFS_MAX_SIZE        (1024 * 1024 * 100)

namespace fs = std::filesystem;

struct CodegenContext {
    int ordinal;
    int offset;
    int escape;
};

class RfsGenEntry {
private:
    std::string name_;
    fs::path    path_;
    uint32_t    flags_;
    int         size_;
    int         ordinal_;
    int         name_offset_;
    int         data_offset_;
public:
    const auto& name() {return name_;}
    auto& name(const std::string& name){
        this->name_ = name;
        return *this;
    }

    const auto& path() {return path_;}
    auto& path(const fs::path& path){
        this->path_ = path;
        return *this;
    }

    auto flags(){return flags_;}
    auto& flags(uint32_t flags){this->flags_ = flags; return *this;}

    auto& size(int size) {this->size_ = size; return *this;}
    const auto size(){return size_;};

    auto ordinal() {return ordinal_;};
    auto& ordinal(int ordinal) {this->ordinal_ = ordinal; return *this;}

    const auto name_offset(){return name_offset_;};
    auto& name_offset(int name_offset) {this->name_offset_ = name_offset; return *this;}
    
    const auto data_offset(){return data_offset_;};
    auto& data_offset(int data_offset) {this->data_offset_ = data_offset; return *this;}

    virtual void debug(int indent = 0) {
        for (int i = 0; i < indent; i++) {
            std::cout << "    ";
        }
        std::cout << name() << ", size: " << size() << std::endl;
    }
    virtual bool is_directory() = 0;
};

class RfsGenDirectory:public RfsGenEntry{
private:
    std::vector<std::shared_ptr<RfsGenEntry> > entries_;
public:
    auto& entries(){return entries_;};

    virtual void debug(int indent = 0) override{
        RfsGenEntry::debug(indent);

        for (auto& en : entries()) {
            en ->debug(indent + 1);
        }
    }

    virtual bool is_directory() override {return true;};
};

class RfsGenFile:public RfsGenEntry{
public:
    virtual bool is_directory() override {return false;};
};

static int build_tree(std::shared_ptr<RfsGenDirectory>& dir);
static int calculate_offset (std::shared_ptr<RfsGenDirectory>& dir, CodegenContext &ctx);

static int generate_all (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir, const std::string& id, const CodegenContext &ctx);
static int generate_dir (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir);
static int generate_data (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir);

///
/// generate RFS .c source file
///@param path the directory or file to be embedded
///@param id identity of the FS, format: [a-z][a-z_0-9]*
///@param option e.g. gzip text files
///@param target_dir target directory 
int generate_rfs(const char *path, const char *id, int options, const char *target_dir) {
    int result = 0;

    //
    // pahse 0: check input parameters
    //
    fs::path source(path);
    if (!fs::exists(source) || !(fs::is_directory(source) || fs::is_regular_file(source))) {
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        return RFS_NOT_FOUND;
    }
    
    fs::path target(target_dir);
    if (!fs::exists(target) || !fs::is_directory(target)) {
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        return RFS_TARGET_NOT_EXIST;
    }
    
    std::string name = std::string("rfs_") + std::string(id) + std::string(".c");
    fs::path rfsfile = target / name;


    //
    // phase 1: generate the directory tree and check the total disk usage of the source directory
    //
    auto root = std::make_shared<RfsGenDirectory>();
    std::cout << "processing: " << source << std::endl;

    root->path(source).name("/").size(1);
    if (!(fs::is_directory(source))) {
        auto s = fs::file_size(source);
        if (s > RFS_MAX_SIZE) {
            // std::cout << __FILE__ << ":" << __LINE__ << std::endl;
            return RFS_SOURCE_TOO_LARGE;
        }
        auto file = std::make_shared<RfsGenFile>();
        file->path(source).name(source.filename());
        file->size(s + file->name().length());

        root->path(source.parent_path());
        root->entries().push_back(file);
        root->size(root->size() + file->size());
    } else {
        result = build_tree(root);
        if(result < 0) {
            std::cout << __FILE__ << ":" << __LINE__ << std::endl;

            std::cout << "TOO LARGE" << std::endl;
        }
    }

    // calculate offset
    CodegenContext ctx = {0, 0};

    root->ordinal(ctx.ordinal++);
    root->name_offset(ctx.offset);
    ctx.offset += root->name().length();
    calculate_offset(root, ctx);

    //
    // phase 2: generate the RFS source file
    //
    // root->debug(0);
    std::ofstream ofs(rfsfile);
    generate_all(ofs, root, id, ctx);


    return 0;
}


///
/// preprocess the directory tree
///
static int build_tree(std::shared_ptr<RfsGenDirectory>& dir) {
    int result = 0;
    for(auto& p: fs::directory_iterator(dir->path())) {
        auto path = p.path();

        // std::cout << p.path() << std::endl;
        if (fs::is_directory(path)) {
            auto subdir = std::make_shared<RfsGenDirectory>();
            subdir->name(path.filename()).path(path);
            subdir->size(subdir->name().length());

            result = build_tree(subdir);
            if(result < 0) {
                std::cout << __FILE__ << ":" << __LINE__ << std::endl;
                return result;
            }
            dir->entries().push_back(subdir);
            dir->size(dir->size() + subdir->size());
        } else {
            auto s = fs::file_size(path);
            if (s > RFS_MAX_SIZE) {
                std::cout << __FILE__ << ":" << __LINE__ << std::endl;
                return RFS_SOURCE_TOO_LARGE;
            }

            auto file = std::make_shared<RfsGenFile>();
            file->name(path.filename()).path(path);
            file->size(file->name().length() + s);
            // std::cout << "processing: " << path << " ,size: " << file->size() << std::endl;

            dir->entries().push_back(file);
            dir->size(dir->size() + file->size());
        }
        if (dir->size() > RFS_MAX_SIZE) {
            std::cout << __FILE__ << ":" << __LINE__ << std::endl;
            return RFS_SOURCE_TOO_LARGE;
        }
    }

    // sort the entries
    std::sort(dir->entries().begin(), dir->entries().end(), [](const auto& left, const auto& right){
        return left->name() < (right->name());
    });
    
    // std::cout << "processing: " << dir->path() << " ,size: " << dir->size() << std::endl;
    return result;
}

static int calculate_offset (std::shared_ptr<RfsGenDirectory>& dir, CodegenContext &ctx){
    // ordinal in dir tree
    for (auto en : dir->entries()) {
        en->ordinal(ctx.ordinal++);
    }

    // name and data offset
    for (auto en : dir->entries()) {
        en->name_offset(ctx.offset);
        ctx.offset += en->name().length();

        if (en->is_directory()) {
            auto subdir = std::dynamic_pointer_cast<RfsGenDirectory> (en);
            calculate_offset (subdir, ctx);
        } else {
            en->data_offset(ctx.offset);
            ctx.offset += en->size() - en->name().length();
        }
    }

    return 0;
}

static int generate_all (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir, const std::string& id, const CodegenContext &ctx) {
    os  << "/**" << std::endl
        << " PLEASE DO NOT EDIT THIS FILE, because it's generated by rfs_generator automatically." << std::endl
        << " rfs_generator is used to embed all content of a directory into a .c sourcde file." << std::endl
        << "*/" << std::endl
        << std::endl
        << "#include \"resource_fs.h\"" << std::endl
        << std::endl

        << "static const RfsFileSystem rfs_" << id << "_;" << std::endl
        << "const RfsFileSystem* rfs_" << id << "(){" << std::endl
        << "  return &rfs_" << id << "_;" << std::endl
        << "}" << std::endl
        << std::endl
        
        << "static const RfsFileSystem rfs_" << id << "_ = {" << std::endl
        << "  // entry_count" << std::endl
        << "  .entry_count = " << ctx.ordinal << ',' << std::endl
        ;
    // dir tree
    generate_dir(os, dir);
    os  << "," << std::endl;

    os  << "  // data_size" << std::endl
        << "  .data_size = " << ctx.offset << ',' << std::endl
        ;

    // data heap
    generate_data(os, dir);
    os  << std::endl;

    os  << "};" << std::endl;
    return 0;
}

//
//  generate directory tree
//
static int generate_dir_entry (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir) {
    os  << "    // [" << dir->ordinal() << "]: "  << dir->path() << std::endl
        << "    {"
        // name
        <<  dir->name_offset() << ", " << dir->name().length() ;
    // directory entries
    if (dir->entries().size() > 0) {
        os << ", " << dir->entries()[0]->ordinal() << ", " << dir->entries().size();
    } else {
        os << ", 0, 0";
    }
    // FLAGS
    os  << ", RFS_DIRECTORY}";
    return 0;
}

static int generate_file_entry (std::ostream& os, std::shared_ptr<RfsGenEntry>& file) {
    os  << "    // [" << file->ordinal() << "]: " << file->path() << std::endl
        << "    {"
        // name
        << file->name_offset() << ", " << file->name().length()
        // content
        << ", " << file->data_offset() << ", " << (file->size() -  file->name().length())
        // flags
        << ", 0}";
    return 0;
}


static int generate_dir_itr (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir) {
    // directory entires
    for (auto en : dir->entries()) {
        if (en->is_directory()) {
            auto subdir = std::dynamic_pointer_cast<RfsGenDirectory> (en);
            os << "," << std::endl;
            generate_dir_entry(os, subdir);
        } else {
            os << "," << std::endl;
            generate_file_entry(os, en);
        }
    }

    // all subdirectores
    for (auto en : dir->entries()) {
        if (en->is_directory()) {
            auto subdir = std::dynamic_pointer_cast<RfsGenDirectory> (en);
            generate_dir_itr(os, subdir);
        }
    }
    return 0;
}

static int generate_dir (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir) {
    os << "  // directory tree: {name_offset, name_length, data_offset, data_size, flags}" << std::endl;
    os << "  .entries = (RfsEntry[]){" << std::endl;
    generate_dir_entry(os, dir);
    generate_dir_itr(os, dir);
    os << std::endl << "  }";
    return 0;
}

//
//  generate content
//
static int generate_data_dir (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir) {
    os << "  // [" << dir->ordinal() << "] DIR: "  << dir->path() << std::endl;
    // name
    os << "  \"" << dir->name() << "\"" << std::endl;
    return 0;
}

/// https://en.cppreference.com/w/cpp/string/byte/isprint
/// https://en.cppreference.com/w/cpp/language/string_literal
/// https://en.cppreference.com/w/cpp/language/escape
static std::string escape_char(unsigned char ch) {
    switch (ch) {
    case '\\': return "\\\\"; 
    case '\"': return "\\\""; 
    case '\a': return "\\a"; 
    case '\b': return "\\b"; 
    case '\t': return "\\t"; 
    case '\n': return "\\n"; 
    case '\v': return "\\v"; 
    case '\f': return "\\f"; 
    case '\r': return "\\r"; 
    default:
        if (ch >= 32 && ch <= 126) {
            // isprint
            std::string str;
            str.push_back(ch);
            return str;
        } else {
            char tmp[16];
            sprintf(tmp, "\\x%02x", (ch & 0xFF));
            return tmp; 
        }
    }
    return "";
}

static int generate_data_file (std::ostream& os, std::shared_ptr<RfsGenEntry>& file, CodegenContext &ctx) {
    os << "  // [" << file->ordinal() << "] FILE: "  << file->path() << std::endl;
    // name
    os << "  \"" << file->name() << "\"" << std::endl;
    // content
    std::ifstream ifs(file->path());
    unsigned char buf[128];
    int len;
    while (true) {
        ifs.read(reinterpret_cast<char*>(buf), 80);
        len = ifs.gcount();
        if(len <= 0) {
            break;
        }
        os << "  \"";
        for (int i = 0; i < len; i++) {
            auto str = escape_char(buf[i]);
            int cur_escape = (str.length() < 4)? 0 : 1;
            if ((ctx.escape ^ cur_escape) != 0) {
                ctx.escape ^= 1;
                os << "\" \"";
            }
            os << str;
        }
        os << "\"" << std::endl;
    }

    return 0;
}


static int generate_data_dir_itr (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir, CodegenContext &ctx) {
    generate_data_dir(os, dir);

    // directory entires
    for (auto en : dir->entries()) {
        if (en->is_directory()) {
            auto subdir = std::dynamic_pointer_cast<RfsGenDirectory> (en);
            generate_data_dir_itr(os, subdir, ctx);
        } else {
            generate_data_file(os, en, ctx);
        }
    }

    return 0;
}

static int generate_data (std::ostream& os, std::shared_ptr<RfsGenDirectory>& dir){
    os << "  // BEGIN: file/directory names and contents" << std::endl;
    os << "  .data = (uint8_t *)" << std::endl;
    CodegenContext ctx = {0, 0, 0};
    generate_data_dir_itr(os, dir, ctx);
    os << "  // END: file/directory names and contents" << std::endl;
    return 0;
}
