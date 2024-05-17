#pragma once

#include "ConnectionTCP.hpp"
#include "ContextIO.hpp"

#include <boost/asio.hpp>

namespace GaijinTestTask {

class ServerTCP : public std::enable_shared_from_this<ServerTCP> {
public:
    ServerTCP(ContextIO& context_io);

private:
    void start_accept();
    void handle_accept(
        ConnectionTCP::pointer new_connection
        , const boost::system::error_code& error
    );

private:
    ContextIO context_io_;
    boost::asio::ip::tcp::acceptor acceptor_;
};  // class ServerTCP

}  // namespace GaijinTestTask

