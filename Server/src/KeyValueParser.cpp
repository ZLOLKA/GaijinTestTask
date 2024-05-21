#include "KeyValueParser.hpp"

#include <algorithm>

namespace GaijinTestTask {

KeyValueParser::KeyValueParser(const std::string& str)
    : message_(str)
{
}

bool KeyValueParser::isHaveCorrectMsg() {
    if(message_.length() < 6) {
        return false;
    }

    const auto begin_msg = std::begin(message_);
    const auto end_msg = std::end(message_);
    auto msg_it = begin_msg;
    if(*msg_it != 'G' && *msg_it != 'S') {
        return false;
    }

    std::advance(msg_it, 1);
    std::string check_str = "et: ";
    if(not std::equal(
        check_str.begin(), check_str.end()
        , msg_it, std::next(msg_it, check_str.length())
    )) {
        return false;
    }

    const bool must_be_eq = *begin_msg == 'S';
    eq_it_ = std::find(begin_msg, end_msg, '=');
    const bool str_have_eq = eq_it_ != end_msg;
    if(must_be_eq != str_have_eq) {
        return false;
    }

    return true;
}

std::string KeyValueParser::parseKey() const {
    return { std::next(std::begin(message_), 5), eq_it_ };
}

std::string KeyValueParser::parseValue() const {
    return { std::next(eq_it_), std::end(message_) };
}

}  // namespace GaijinTestTask

