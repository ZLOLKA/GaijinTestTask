#pragma once

#include "ConnectionTCP.hpp"

#include <boost/asio.hpp>

namespace GaijinTestTask {

class ContextIO;

class ServerTCP {
public:
    ServerTCP(ContextIO& context_io);

private:
    void start_accept();
    void handle_accept(
        ConnectionTCP::pointer new_connection
        , const boost::system::error_code& error
    );
};  // class ServerTCP

}  // namespace GaijinTestTask

