#include "ContextIO.hpp"
#include "ServerTCP.hpp"

#include <boost/asio.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

struct jthreader {
    std::vector<std::thread>& threads;

    jthreader(decltype(threads)& threads)
        : threads(threads)
    {
    }
    ~jthreader() {
        for(auto& thread: threads) {
            thread.join();
        }
    }
};  // struct jthreader

static void startWork(boost::asio::io_context& contx) {
    auto work = boost::asio::make_work_guard(contx);
    contx.run();
}

int main()
try {
    GaijinTestTask::ContextIO context_io{
        std::make_shared<boost::asio::io_context>()
        , std::make_shared<boost::asio::io_context>()
        , std::make_shared<boost::asio::io_context>()
    };
    GaijinTestTask::ServerTCP server(context_io);

    const std::size_t hardware_thread_count = std::thread::hardware_concurrency();
    const std::size_t thread_count
        = std::max(hardware_thread_count, static_cast<std::size_t>(4));
    std::vector<std::thread> threads;
    jthreader joiner(threads);
    threads.reserve(thread_count);
    for(std::size_t i = 0; i < thread_count; ++i) {
        threads.push_back(
            std::thread([net_io_context=context_io.net_io_context](){
                startWork(*net_io_context);
            })
        );
    }
    threads.push_back(
        std::thread([console_io_context=context_io.console_io_context](){
            startWork(*console_io_context);
        })
    );
    startWork(*(context_io.fs_io_context));

    return 0;
}
catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
}

