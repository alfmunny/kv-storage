#include <iostream>
#include <rpc/client.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: Provide host and port of the primary node" << std::endl;
    }

    rpc::client client(argv[1], atoi(argv[2]));

    std::cout << "Connecting to " << argv[1] << ":" << argv[2] << std::endl;

    char action[10];
    char key[1024];
    char value[1024];

    int counter = 0;
    std::stringstream ss;

    while (true) {
        std::cout << ">";
        scanf("%s %s %s", action, key, value);
        auto ss = std::string(action);

        if  (ss == "GET") {
            std::cout << client.call("GET", key).as<std::pair<bool, std::string>>().second << std::endl;
        } else if (ss == "PUT") {
            client.call("PUT", key, value);
        } else if (ss == "DEL") {
            client.call("DEL", key);
        } else if (ss == "exit") {
            break;
        }
        ss.clear();
    }

    return 0;
}
