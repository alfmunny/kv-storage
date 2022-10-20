#include <iostream>
#include <celonis/kv_primary.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout
            << "Usage: provide port for service"
            << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::pair<std::string, int>> kvnodes;

    for (int i = 2; i < argc; ++i) {
        std::stringstream host;
        std::stringstream port;
        bool found = false;

        for (auto& c : std::string{argv[i]}) {
            if (c == ':') {
                found = true;
                continue;
            }

            if (!found) {
                host << c;
            } else {
                port << c;
            }
        }
        std::cout << "host: " << host.str() << ", port: " << port.str() << std::endl;
        kvnodes.push_back({ host.str(), atoi(port.str().c_str())});
    }


    celonis::KVPrimary kvp(atoi(argv[1]), kvnodes);

    kvp.put("alice", "alice@gmail");
    kvp.put("bob", "bob@gmail");

    kvp.run();
    return 0;
}
