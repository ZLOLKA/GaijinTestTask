#include "ConnectionTCP.hpp"

#include "ContextIO.hpp"
#include "KeyValueStorage.hpp"

#include <boost/bind.hpp>

#include <algorithm>
#include <iterator>

namespace GaijinTestTask {

ConnectionTCP::pointer ConnectionTCP::create(ContextIO& context_io) {
    return pointer(new ConnectionTCP(context_io));
}

auto ConnectionTCP::getSocket() -> decltype(ConnectionTCP::socket_)& {
    return socket_;
}

void ConnectionTCP::start() {
    boost::asio::async_read(socket_, boost::asio::buffer(message_)
        , boost::bind(
            &ConnectionTCP::handleGet
            , shared_from_this()
            , boost::asio::placeholders::error
        )
    );
}

ConnectionTCP::ConnectionTCP(ContextIO& context_io)
    : context_io_(context_io), socket_(*context_io_.net_io_context)
{
}

void ConnectionTCP::handleGet(const boost::system::error_code& error) {
    if(error != boost::asio::error::eof) {
        processNetError();
        return;
    }

    if(not isHaveCorrectMsg()) {
        processIncorrectMsg();
        return;
    }

    if(*std::begin(message_) == 'G')
    {
        processGetRequest();
    }
    else {
        processSetRequest();
    }
}

void ConnectionTCP::processNetError() const {
    // TODO: Error branch
}

bool ConnectionTCP::isHaveCorrectMsg() const {
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

void ConnectionTCP::processIncorrectMsg() const {
    // TODO: Error incorrect msg
}

void ConnectionTCP::processGetRequest() {
    const std::string key = parseKey();
    auto& storage = KeyValueStorage::create(context_io_);
    storage.async_get(
        key
        , boost::bind(
            &ConnectionTCP::handleReadKVStorage
            , shared_from_this()
            , boost::placeholders::_1
        )
    );
}

void ConnectionTCP::handleReadKVStorage(std::string value) {
    message_ = value;
    boost::asio::async_write(
        socket_
        , boost::asio::buffer(message_)
        , boost::bind(&ConnectionTCP::handleSend, shared_from_this())
    );
}

void ConnectionTCP::handleSend() {
}

void ConnectionTCP::processSetRequest() {
    const std::string key = parseKey();
    const std::string value = parseValue();
    auto& storage = KeyValueStorage::create(context_io_);
    storage.async_set(
        key, value
        , boost::bind(&ConnectionTCP::handleWriteKVStorage, shared_from_this())
    );
}

void ConnectionTCP::handleWriteKVStorage() {
}

std::string ConnectionTCP::parseKey() const {
    return { std::next(std::begin(message_), 5), eq_it_ };
}

std::string ConnectionTCP::parseValue() const {
    return { std::next(eq_it_), std::end(message_) };
}

}  // namespace GaijinTestTask

