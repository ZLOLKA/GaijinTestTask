#include "Client.hpp"
#include "Settings.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>

int main() {
    const GaijinTestTask::Settings settings{};
    const auto& keys = settings.getKeys();

    const auto keys_begin = std::begin(keys);
    const auto keys_end = std::end(keys);
    const std::size_t keys_len = std::distance(keys_begin, keys_end);
    if(keys_len == 0) {
        return -1;
    }

    GaijinTestTask::Client client{};
    std::srand(std::time(nullptr));
    for(std::size_t i = 0; i < settings.getRequestCount(); ++i) {
        const auto& rand_key = *std::next(keys_begin, std::rand() % keys_len);

        if(std::rand() % 100 > settings.getPercentSending()) {
            const std::string value = client.sendGet(rand_key);
            std::cout << "Get: " << rand_key << "=" << value << std::endl;
        }
        else {
            const std::string rand_value = std::to_string(std::rand());
            client.sendSet(rand_key, rand_value);
            std::cout << "Set: " << rand_key << "=" << rand_value << std::endl;
        }
    }

    return 0;
}
