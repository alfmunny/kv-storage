#include <iostream>
#include <rpc/client.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: Provide host and port of the primary node" << std::endl;
    }

    rpc::client client(argv[1], atoi(argv[2]));

    std::cout << "Connecting to " << argv[1] << ":" << argv[2] << std::endl;

    std::cout << "Inserting {alice, alice@gmail.com}" << std::endl;
    client.call("PUT", "alice", "alice@gmail.com");
    std::cout << "Inserting {bob, bob@gmail.com}" << std::endl;
    client.call("PUT", "bob", "bob@gmail.com");

    std::cout << "Get bob" << std::endl;
    std::cout << client.call("GET", "bob").as<std::pair<bool, std::string>>().second << std::endl;
    std::cout << "Get alice" << std::endl;
    std::cout << client.call("GET", "alice").as<std::pair<bool, std::string>>().second << std::endl;
    std::cout << "Get nothing" << std::endl;
    std::cout << client.call("GET", "nothing").as<std::pair<bool, std::string>>().second << std::endl;

    std::cout << "Delete bob" << std::endl;
    client.call("DEL", "bob");

    std::cout << "Get bob: empty" << std::endl;
    std::cout << client.call("GET", "bob").as<std::pair<bool, std::string>>().second << std::endl;

    std::cout << "Congratulation, databases are working!" << std::endl;

    return 0;
}
