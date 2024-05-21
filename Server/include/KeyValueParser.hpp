#pragma once

#include <string>

namespace GaijinTestTask {

class KeyValueParser {
    const std::string& message_;
    decltype(std::cbegin(message_)) eq_it_;

public:
    explicit KeyValueParser(const std::string& str);

    bool isHaveCorrectMsg();
    std::string parseKey() const;
    std::string parseValue() const;
};

}  // namespace GaijinTestTask

