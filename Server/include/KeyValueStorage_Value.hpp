#pragma once

#include "KeyValueStorage.hpp"

#include <atomic>

namespace GaijinTestTask {

struct KeyValueStorage::Value {
    struct ValueInfo {
        std::atomic<std::size_t> read_count_;
        std::size_t write_count_;

    public:
        explicit ValueInfo(std::size_t read_count = 0, std::size_t write_count = 0);
        ValueInfo(ValueInfo&& other);
        ValueInfo& operator=(ValueInfo&& other);

        ValueInfo(const ValueInfo&) = delete;
        ValueInfo& operator=(const ValueInfo&) = delete;
    };  // struct ValueInfo

public:
    std::string value_;
    ValueInfo info_;

public:
    Value(const std::string& value, ValueInfo&& info);
    Value(Value&& other);
    Value& operator=(Value&& other);

    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
};  // struct Value

template<>
KeyValueStorage::Value deserialize<KeyValueStorage::Value>(
    const std::string& str
);
template<>
std::string serialize<KeyValueStorage::Value>(
    const KeyValueStorage::Value& value
);

template<>
KeyValueStorage::Value::ValueInfo deserialize<KeyValueStorage::Value::ValueInfo>(
    const std::string& str
);
template<>
std::string serialize<KeyValueStorage::Value::ValueInfo>(
    const KeyValueStorage::Value::ValueInfo& value
);

}  // namespace GaijinTestTask

