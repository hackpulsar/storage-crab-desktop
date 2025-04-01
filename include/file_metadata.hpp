#ifndef FILE_METADATA_HPP
#define FILE_METADATA_HPP

namespace Files {

// Holds file data
struct Metadata {
    std::string path;
    std::string name;
    std::string extension;

    // Extracts file data from path
    explicit Metadata(const std::string& path)
        : path(path)
    {
#ifdef _WIN32
        name = path.substr(path.find_last_of('\') + 1);
#else
        name = path.substr(path.find_last_of('/') + 1);
#endif
        extension = name.substr(path.find_first_of('.') + 1);
    }
};

}

#endif //FILE_METADATA_HPP
