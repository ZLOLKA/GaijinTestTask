#include "KeyValueStorage_Value.hpp"

namespace GaijinTestTask {

KeyValueStorage::Value::ValueInfo::ValueInfo(
    std::size_t read_count, std::size_t write_count
)
    : read_count_(read_count), write_count_(write_count)
{
}

KeyValueStorage::Value::ValueInfo::ValueInfo(
    KeyValueStorage::Value::ValueInfo&& other
)
    : read_count_(other.read_count_.load(std::memory_order_relaxed))
    , write_count_(other.write_count_)
{
}

KeyValueStorage::Value::ValueInfo& KeyValueStorage::Value::ValueInfo::operator=(
    KeyValueStorage::Value::ValueInfo&& other
) {
    KeyValueStorage::Value::ValueInfo oth(std::move(other));
    std::swap(oth, *this);
    return *this;
}

KeyValueStorage::Value::Value(
    const std::string& value, KeyValueStorage::Value::ValueInfo&& info
)
    : value_(value), info_(std::move(info))
{
}

KeyValueStorage::Value::Value(KeyValueStorage::Value&& other)
    : value_(std::move(other.value_)), info_(std::move(other.info_))
{
}

KeyValueStorage::Value& KeyValueStorage::Value::operator=(
    KeyValueStorage::Value&& other
) {
    KeyValueStorage::Value oth(std::move(other));
    std::swap(oth, *this);
    return *this;
}

template<>
KeyValueStorage::Value deserialize<KeyValueStorage::Value>(
    const std::string& str
) {
    return { str, KeyValueStorage::Value::ValueInfo() };
}

template<>
std::string serialize<KeyValueStorage::Value>(
    const KeyValueStorage::Value& value
) {
    return value.value_ + serialize(value.info_);
}

template<>
KeyValueStorage::Value::ValueInfo deserialize<KeyValueStorage::Value::ValueInfo>(
    const std::string& str
) {
    return KeyValueStorage::Value::ValueInfo();
}

template<>
std::string serialize<KeyValueStorage::Value::ValueInfo>(
    const KeyValueStorage::Value::ValueInfo& value
) {
    std::string res;
    std::stringstream res_stream(res);
    res_stream
        << "; r=" << value.read_count_.load(std::memory_order_relaxed)
        << "; w=" << value.write_count_
        << ";"
    ;
    return res;
}

}  // namespace GaijinTestTask

