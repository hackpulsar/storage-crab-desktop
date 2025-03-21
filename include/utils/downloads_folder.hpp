#ifndef DOWNLOADS_FOLDER_HPP
#define DOWNLOADS_FOLDER_HPP

#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>  // For SHGetKnownFolderPath
#pragma comment(lib, "Shell32.lib")  // Link Shell32.lib for SHGetKnownFolderPath
#endif

namespace Utils {

inline std::string GetDownloadsFolder() {
#ifdef _WIN32
    PWSTR path = NULL;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path))) {
        char downloadsPath[MAX_PATH];
        wcstombs(downloadsPath, path, MAX_PATH);  // Convert wide string to regular string
        CoTaskMemFree(path);  // Free memory
        return std::string(downloadsPath);
    }
    return "C:\\Users\\Public\\Downloads";  // Fallback path
#elif __APPLE__ || __linux__
    if (const char* home = getenv("HOME"))
        return std::string(home) + "/Downloads";  // Default ~/Downloads
    return "/tmp";  // Fallback path
#else
    return ".";  // Fallback for unknown platforms
#endif
}

}

#endif //DOWNLOADS_FOLDER_HPP
