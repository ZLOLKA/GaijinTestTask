#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>

namespace GaijinTestTask {

class Client {
    boost::asio::io_context io_context_;
    mutable boost::asio::ip::tcp::socket socket_;

public:
    Client(std::string ip_str = "127.0.0.1", std::size_t port = 1700);

    std::string sendGet(const std::string& key) const;
    void sendSet(const std::string& key, const std::string& value) const;

private:
    void sendStr(const std::string& str) const;
    std::string getStr() const;
};  // class Client

}  // namespace GaijinTestTask

