#pragma once

#include "ContextIO.hpp"

#include "KeyValueParser.hpp"

#include <boost/asio.hpp>

#include <string>

namespace GaijinTestTask {

class ConnectionTCP: public std::enable_shared_from_this<ConnectionTCP> {
    ContextIO context_io_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<KeyValueParser> parser_;

public:
    typedef std::shared_ptr<ConnectionTCP> pointer;

    static pointer create(ContextIO& context_io);
    decltype(socket_)& getSocket();
    void start();

private:
    ConnectionTCP(ContextIO& context_io);

    void handleGet(
        std::shared_ptr<std::string> msg, const boost::system::error_code& error
    );
    void processNetError() const;
    void processIncorrectMsg() const;

    void processGetRequest(std::shared_ptr<std::string> msg);
    void handleReadKVStorage(std::shared_ptr<std::string> msg, std::string value);
    void handleSend(std::shared_ptr<std::string> msg);

    void processSetRequest();
};  // class ConnectionTCP

}  // namespace GaijinTestTask

