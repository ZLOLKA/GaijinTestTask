#pragma once

#include "ContextIO.hpp"

#include <boost/asio/random_access_file.hpp>
#include <boost/asio/steady_timer.hpp>

#include <functional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace GaijinTestTask {

template<class T>
T deserialize(const std::string& str);
template<class T>
std::string serialize(const T& value);

class KeyValueStorage: public std::enable_shared_from_this<KeyValueStorage> {
public:
    struct Value;

public:
    static KeyValueStorage& create(ContextIO& context_io);

    void set(const std::string& key, const std::string& value);
    void async_get(
        const std::string& key, std::function<void(std::string)> callback
    ) const;

private:
    using Cache = std::unordered_map<std::string, std::unique_ptr<Value>>;

private:
    explicit KeyValueStorage(ContextIO& context_io);
    void init();

    void handleAsyncGet(
        std::string key, std::function<void(std::string)> callback
    ) const;

    void handleTimer();
    void write2File();
    void handleWrite2File(boost::system::error_code err) const;

private:
    friend KeyValueStorage::Cache deserialize<KeyValueStorage::Cache>(
        const std::string& str
    );
    friend std::string serialize<KeyValueStorage::Cache>(
        const KeyValueStorage::Cache& cache
    );

private:
    static std::string dropIncorrectChars(std::string str);

private:
    ContextIO context_io_;
    boost::asio::random_access_file file_;
    std::string file_buffer_;
    std::size_t read_count_;
    std::size_t write_count_;

    constexpr static std::chrono::seconds timer_period_{5};
    boost::asio::steady_timer timer_;

    mutable std::shared_mutex cache_mtx_;
    Cache cache_;
};  // class KeyValueStorage

}  // namespace GaijinTestTask

