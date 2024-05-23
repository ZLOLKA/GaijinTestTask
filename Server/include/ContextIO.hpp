#pragma once

#include <boost/asio.hpp>

#include <memory>

namespace GaijinTestTask {

class ContextIO {
public:
    std::shared_ptr<boost::asio::io_context> net_io_context;
    std::shared_ptr<boost::asio::io_context> fs_io_context;
    std::shared_ptr<boost::asio::io_context> console_io_context;
};  // class ConextIO

}  // namespace GaijinTestTask

