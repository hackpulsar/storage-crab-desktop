#ifndef FILE_DATA_HPP
#define FILE_DATA_HPP

#include <string>

// Represents a record of a file in server db
struct FileData {
    std::string name;   // file name
    std::string path;   // path to a file on remote storage
    size_t size;        // size in bytes
    size_t id;          // file id
};

#endif //FILE_DATA_HPP
