#ifndef KEY_HPP
#define KEY_HPP

#include <nlohmann/json.hpp>

namespace Cryptography {

class Key {
public:
    virtual ~Key() = default;
    virtual nlohmann::json toJSON() { return nlohmann::json(); };
};

}

#endif //KEY_HPP
