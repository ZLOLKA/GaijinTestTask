#include "ServerTCP.hpp"

#include "ConnectionTCP.hpp"
#include "ContextIO.hpp"

#include <boost/bind.hpp>

using asio_tcp = boost::asio::ip::tcp;

namespace GaijinTestTask {

ServerTCP::ServerTCP(ContextIO& context_io)
    : context_io_(context_io)
    , acceptor_(
        *context_io.net_io_context, asio_tcp::endpoint(asio_tcp::v4(), 1700)
    )
{
    start_accept();
}

void ServerTCP::start_accept() {
    ConnectionTCP::pointer new_connection = ConnectionTCP::create(context_io_);

    acceptor_.async_accept(
        new_connection->getSocket()
        , boost::bind(
            &ServerTCP::handle_accept
            , shared_from_this()
            , new_connection
            , boost::asio::placeholders::error
        )
    );
}

void ServerTCP::handle_accept(
    ConnectionTCP::pointer new_connection
    , const boost::system::error_code& error
) {
    if(not error) {
        new_connection->start();
    }

    start_accept();
}

}  // namespace GaijinTestTask

