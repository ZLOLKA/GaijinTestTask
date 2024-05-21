#pragma once

#include "ContextIO.hpp"

#include <functional>
#include <string>

namespace GaijinTestTask {

class KeyValueStorage {
public:
    static KeyValueStorage& create(ContextIO& context_io);

    void set(const std::string& key, const std::string& value);
    void async_get(
        const std::string& key, std::function<void(std::string)> callback
    ) const;

private:
    KeyValueStorage(ContextIO& context_io);

private:
    ContextIO context_io_;
};  // class KeyValueStorage

}  // namespace GaijinTestTask

