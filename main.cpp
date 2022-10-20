#include <iostream>
#include <rpc/client.h>

int main() {
    rpc::client client("localhost", 8080);
    std::cout << client.call("GET", "bob").as<std::pair<bool, std::string>>().second << std::endl;
    std::cout << client.call("GET", "alice").as<std::pair<bool, std::string>>().second << std::endl;
    std::cout << client.call("GET", "haha").as<std::pair<bool, std::string>>().second << std::endl;

    client.call("PUT", "wei", "wei@gmail.com");

    std::cout << client.call("GET", "wei").as<std::pair<bool, std::string>>().second << std::endl;

    client.call("DEL", "wei");

    std::cout << client.call("GET", "wei").as<std::pair<bool, std::string>>().second << std::endl;

    return 0;
}
