#include "Client.hpp"

#include <boost/asio.hpp>

namespace GaijinTestTask {

Client::Client(std::string ip_str, std::size_t port)
    : io_context_()
    , acceptor_(
        io_context_
        , boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(ip_str), port
        )
    )
    , socket_(io_context_)
{
}

void Client::sendSet(const std::string& key, const std::string& value) const {
    using namespace std::string_literals;

    sendStr("Set "s + key + "=" + value);
}

std::string Client::sendGet(const std::string& key) const {
    using namespace std::string_literals;

    sendStr("Get "s + key);
    return getStr();
}

void Client::sendStr(const std::string& str) const {
    boost::system::error_code err;
    boost::asio::write(socket_, boost::asio::buffer(str), err);
}

std::string Client::getStr() const {
    using namespace std::string_literals;

    std::string res;
    boost::system::error_code err;
    std::size_t len = socket_.read_some(boost::asio::buffer(res), err);
    if(err != boost::asio::error::eof) {
        // TODO: Not processed error branch
        res = "TODO: Error branch: err=\""s + err.message() + '"';
    }
    return res;
}

}  // namespace GaijinTestTask

