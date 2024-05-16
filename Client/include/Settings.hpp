#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace GaijinTestTask {

class Settings {
    std::vector<std::string> keys_;
    std::size_t requestCount_;
    std::uint8_t percentSending_;

public:
    const decltype(keys_)& getKeys() const;
    std::size_t getRequestCount() const;
    std::uint8_t getPercentSending() const;
};  // class Settings

}  // namespace GaijinTestTask

