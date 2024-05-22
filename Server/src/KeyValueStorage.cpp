#include "KeyValueStorage.hpp"

#include "KeyValueParser.hpp"
#include "KeyValueStorage_Value.hpp"

#include <boost/asio/spawn.hpp>
#include <boost/bind.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

namespace GaijinTestTask {

template<>
KeyValueStorage::Cache deserialize<KeyValueStorage::Cache>(
    const std::string& str
) {
    std::stringstream str_stream(str);
    std::string line;
    KeyValueStorage::Cache res;
    while(std::getline(str_stream, line)) {
        KeyValueParser parser(line);
        if(parser.isHaveCorrectMsg()) {
            res.emplace(
                parser.parseKey()
                , std::make_unique<KeyValueStorage::Value>(
                    deserialize<KeyValueStorage::Value>(parser.parseValue())
                )
            );
        }
    }
    return res;
}

template<>
std::string serialize<KeyValueStorage::Cache>(
    const KeyValueStorage::Cache& cache
) {
    std::string res;
    std::stringstream res_stream(res);
    for(auto&[key, value]: cache) {
        res_stream << key << "=" << value->value_ << std::endl;
    }
    return res;
}

KeyValueStorage& KeyValueStorage::create(ContextIO& context_io) {
    static std::atomic<KeyValueStorage*> obj_ = nullptr;
    static std::shared_ptr<KeyValueStorage> ptr;

    if(obj_.load(std::memory_order_acquire) == nullptr) {
        auto* obj_new = new KeyValueStorage(context_io);
        decltype(obj_new) ptr_buf = nullptr;
        if(not obj_.compare_exchange_strong(
            ptr_buf, obj_new
            , std::memory_order_seq_cst, std::memory_order_acquire
        )) {
            delete obj_new;
        } else {
            ptr = std::shared_ptr<KeyValueStorage>(
                obj_.load(std::memory_order_acquire)
            );
            ptr->init();
        }
    }
    return *(obj_.load(std::memory_order_relaxed));
}

KeyValueStorage::KeyValueStorage(ContextIO& context_io)
    : context_io_(context_io)
    , file_(
        *context_io_.fs_io_context
        , filename_
        , boost::asio::file_base::read_write
    )
    , read_count_(0), write_count_(0)
    , timer_(*context_io_.console_io_context, timer_period_)
{
}

void KeyValueStorage::init() {
    using namespace std::string_literals;

    timer_.async_wait(boost::bind(
        &KeyValueStorage::handleTimer, shared_from_this()
    ));

    boost::system::error_code err;
    file_buffer_.resize(file_.size(err));
    boost::asio::read_at(
        file_
        , 0, boost::asio::buffer(file_buffer_)
        , boost::asio::transfer_all(), err
    );
    if(err && err != boost::asio::error::eof) {
        throw std::runtime_error("File \""s + filename_ + "\" does not open");
    }
    cache_ = deserialize<KeyValueStorage::Cache>(file_buffer_);
}

std::string KeyValueStorage::dropIncorrectChars(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.begin());
    return str;
}

void KeyValueStorage::handleTimer() {
    timer_.expires_after(timer_period_);
    timer_.async_wait(boost::bind(
        &KeyValueStorage::handleTimer
        , shared_from_this()
    ));

    std::scoped_lock lock(cache_mtx_);
    std::size_t old_read_count = read_count_;
    std::size_t old_write_count = write_count_;
    read_count_ = std::reduce(
        cache_.begin(), cache_.end(), std::size_t(0)
        , [](std::size_t res, decltype(*cache_.begin())& el) -> std::size_t {
            return res + el.second->info_.read_count_.load(
                std::memory_order_relaxed
            );
        }
    );
    write_count_ = std::reduce(
        cache_.begin(), cache_.end(), std::size_t(0)
        , [](std::size_t res, decltype(*cache_.begin())& el) -> std::size_t {
            return res + el.second->info_.write_count_;
        }
    );
    boost::asio::post(
        *(context_io_.console_io_context)
        , [old_read_count, old_write_count
            , read_count=read_count_, write_count=write_count_
        ](){
            std::cout
                << "All:"
                << " r=" << std::to_string(read_count) << ";"
                << " w=" << std::to_string(write_count) << ";"
                << "Last " << timer_period_.count() << ":"
                << " r=" << std::to_string(read_count - old_read_count) << ";"
                << " w=" << std::to_string(write_count - old_write_count) << ";"
                << std::endl
            ;
        }
    );
    boost::asio::post(
        *(context_io_.fs_io_context)
        , boost::bind(&KeyValueStorage::write2File, shared_from_this())
    );
}

void KeyValueStorage::write2File() {
    std::scoped_lock lock(cache_mtx_);

    file_buffer_ = std::move(serialize(cache_));
    file_.async_write_some_at(
        0, boost::asio::buffer(file_buffer_)
        , boost::bind(
            &KeyValueStorage::handleWrite2File
            , shared_from_this()
            , boost::asio::placeholders::error
        )
    );
}

void KeyValueStorage::handleWrite2File(boost::system::error_code err) const {
}

void KeyValueStorage::async_get(
    const std::string& key, std::function<void(std::string)> callback
) const {
    boost::asio::post(
        *(context_io_.net_io_context)
        , boost::bind(
            &KeyValueStorage::handleAsyncGet
            , shared_from_this()
            , key
            , callback
        )
    );
}

void KeyValueStorage::handleAsyncGet(
    std::string key, std::function<void(std::string)> callback
) const {
    if(cache_mtx_.try_lock_shared()) {
        std::shared_lock lock(cache_mtx_, std::adopt_lock);

        std::string res = "";
        if(
            auto value_it = cache_.find(dropIncorrectChars(key));
            value_it != cache_.end()
        ) {
            value_it->second->info_.read_count_ += 1;
            res = serialize(*(value_it->second));
        }
        callback(std::move(res));
    } else {
        async_get(key, callback);
    }
}

void KeyValueStorage::set(const std::string& key, const std::string& value) {
    std::scoped_lock lock(cache_mtx_);

    if(
        auto value_it = cache_.find(dropIncorrectChars(key));
        value_it != cache_.end()
    ) {
        value_it->second->info_.write_count_ += 1;
        value_it->second->value_ = dropIncorrectChars(value);
    } else {
        KeyValueStorage::Value::ValueInfo value_info(0, 1);
        cache_.emplace(
            key
            , std::make_unique<KeyValueStorage::Value>(
                value, std::move(value_info)
            )
        );
    }
}

}  // namespace GaijinTestTask

