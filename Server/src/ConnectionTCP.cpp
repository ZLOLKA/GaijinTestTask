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
    auto msg = std::make_shared<std::string>();
    boost::asio::async_read_until(socket_, boost::asio::dynamic_buffer(*msg), '\n'
        , boost::bind(
            &ConnectionTCP::handleGet
            , shared_from_this()
            , msg
            , boost::asio::placeholders::error
        )
    );
}

ConnectionTCP::ConnectionTCP(ContextIO& context_io)
    : context_io_(context_io), socket_(*context_io_.net_io_context)
{
}

void ConnectionTCP::handleGet(
    std::shared_ptr<std::string> msg, const boost::system::error_code& error
) {
    if(error && error != boost::asio::error::eof) {
        processNetError();
        return;
    }

    parser_ = std::make_unique<KeyValueParser>(*msg);
    if(not parser_->isHaveCorrectMsg()) {
        processIncorrectMsg();
        return;
    }

    if(*std::begin(*msg) == 'G')
    {
        processGetRequest(msg);
    }
    else {
        processSetRequest();
    }
    start();
}

void ConnectionTCP::processNetError() const {
    // TODO: Error branch
}

void ConnectionTCP::processIncorrectMsg() const {
    // TODO: Error incorrect msg
}

void ConnectionTCP::processGetRequest(std::shared_ptr<std::string> msg) {
    const std::string key = parser_->parseKey();
    auto& storage = KeyValueStorage::create(context_io_);
    storage.async_get(
        key
        , boost::bind(
            &ConnectionTCP::handleReadKVStorage
            , shared_from_this()
            , msg
            , boost::placeholders::_1
        )
    );
}

void ConnectionTCP::handleReadKVStorage(
    std::shared_ptr<std::string> msg, std::string value
) {
    *msg = value;
    boost::asio::async_write(
        socket_
        , boost::asio::buffer(*msg)
        , boost::bind(&ConnectionTCP::handleSend, shared_from_this(), msg)
    );
}

void ConnectionTCP::handleSend(std::shared_ptr<std::string> msg) {
    socket_.send(boost::asio::buffer(*msg));
}

void ConnectionTCP::processSetRequest() {
    const std::string key = parser_->parseKey();
    const std::string value = parser_->parseValue();
    auto& storage = KeyValueStorage::create(context_io_);
    storage.set(key, value);
}

}  // namespace GaijinTestTask

