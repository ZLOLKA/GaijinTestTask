#include "ConnectionTCP.hpp"

#include "ContextIO.hpp"
#include "KeyValueStorage.hpp"

#include <boost/bind.hpp>

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

    parser_ = std::make_unique<KeyValueParser>(message_);
    if(not parser_->isHaveCorrectMsg()) {
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

void ConnectionTCP::processIncorrectMsg() const {
    // TODO: Error incorrect msg
}

void ConnectionTCP::processGetRequest() {
    const std::string key = parser_->parseKey();
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
    const std::string key = parser_->parseKey();
    const std::string value = parser_->parseValue();
    auto& storage = KeyValueStorage::create(context_io_);
    storage.set(key, value);
}

}  // namespace GaijinTestTask

