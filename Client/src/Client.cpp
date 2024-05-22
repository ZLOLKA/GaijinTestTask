#include "Client.hpp"

#include <boost/asio.hpp>

namespace GaijinTestTask {

Client::Client(std::string ip_str, std::size_t port)
    : io_context_()
    , socket_(io_context_)
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    auto endpoints = resolver.resolve(ip_str, std::to_string(port));
    boost::asio::connect(socket_, endpoints);
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
    socket_.send(boost::asio::buffer(str));
}

std::string Client::getStr() const {
    using namespace std::string_literals;

    std::string res;
    boost::system::error_code err;
    boost::asio::read_until(socket_, boost::asio::dynamic_buffer(res), '\n', err);
    if(err && err != boost::asio::error::eof) {
        // TODO: Not processed error branch
        res = "TODO: Error branch: err=\""s + err.message() + '"';
    }
    return res;
}

}  // namespace GaijinTestTask

