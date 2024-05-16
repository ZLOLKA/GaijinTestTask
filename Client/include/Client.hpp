#pragma once

#include <string>

namespace GaijinTestTask {

class Client {
public:
    std::string sendGet(const std::string& key) const;
    void sendSet(const std::string& key, const std::string& value) const;
};  // class Client

}  // namespace GaijinTestTask

