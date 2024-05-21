#pragma once

#include "ContextIO.hpp"

#include <boost/asio.hpp>

#include <string>

namespace GaijinTestTask {

class ConnectionTCP: public std::enable_shared_from_this<ConnectionTCP> {
    ContextIO context_io_;
    boost::asio::ip::tcp::socket socket_;
    std::string message_;
    mutable decltype(std::cbegin(message_)) eq_it_;

public:
    typedef std::shared_ptr<ConnectionTCP> pointer;

    static pointer create(ContextIO& context_io);
    decltype(socket_)& getSocket();
    void start();

protected:
    std::string parseKey() const;
    std::string parseValue() const;

private:
    ConnectionTCP(ContextIO& context_io);

    void handleGet(const boost::system::error_code& error);
    void processNetError() const;
    bool isHaveCorrectMsg() const;
    void processIncorrectMsg() const;

    void processGetRequest();
    void handleReadKVStorage(std::string value);
    void handleSend();

    void processSetRequest();
};  // class ConnectionTCP

}  // namespace GaijinTestTask

