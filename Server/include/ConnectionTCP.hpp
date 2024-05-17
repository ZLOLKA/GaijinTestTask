#pragma once

#include <boost/asio.hpp>

#include <string>

namespace GaijinTestTask {

class ContextIO;

class ConnectionTCP: public std::enable_shared_from_this<ConnectionTCP> {
    boost::asio::ip::tcp::socket socket_;
    std::string message_;

public:
    typedef std::shared_ptr<ConnectionTCP> pointer;

    static pointer create(ContextIO& context_io);
    decltype(socket_)& getSocket();
    void start();

private:
    ConnectionTCP(ContextIO& context_io);

    void handleSend();
};  // class ConnectionTCP

}  // namespace GaijinTestTask

