#include "Settings.hpp"

namespace GaijinTestTask {

Settings::Settings()
    : keys_({{"q"}, {"w"}, {"e"}}), requestCount_(10'000), percentSending_(1)
{
}

auto Settings::getKeys() const -> const decltype(keys_)& {
    return keys_;
}

std::size_t Settings::getRequestCount() const {
    return requestCount_;
}

std::uint8_t Settings::getPercentSending() const {
    return percentSending_;
}

}  // namespace GaiginTestTask

