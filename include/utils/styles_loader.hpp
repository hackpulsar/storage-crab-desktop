#ifndef STYLES_LOADER_HPP
#define STYLES_LOADER_HPP

#include <fstream>
#include <sstream>

namespace Utils::StylesLoader {

const std::string STYLES_BASE_PATH = "../styles/";

// Reads the stylesheet from file to QString.
// Returns an empty QString on failure.
inline QString loadStyleFromFile(const std::string& filename) {
    std::ifstream file(STYLES_BASE_PATH + filename);

    if (!file.is_open()) return QString();

    std::stringstream buffer;

    std::string line;
    while (getline(file, line))
        buffer << line << "\n";

    file.close();

    return QString::fromStdString(buffer.str());
}

}

#endif //STYLES_LOADER_HPP
