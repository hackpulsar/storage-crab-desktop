#ifndef KEY_HPP
#define KEY_HPP

#include <string>

namespace Cryptography {

class Key {
public:
    virtual ~Key() = default;
    virtual void exportTo(const std::string& path) = 0;
};

}

#endif //KEY_HPP
