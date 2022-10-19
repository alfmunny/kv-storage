#include <iostream>
#include <celonis/kv_primary.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout
            << "Usage: provide port for service"
            << std::endl;
        return EXIT_FAILURE;
    }

    const std::vector<std::pair<std::string, int>> kvnodes = {
        {"localhost", 8081},
        {"localhost", 8082},
    };

    celonis::KVPrimary kvp(atoi(argv[1]), kvnodes);

    kvp.put("alice", "alice@gmail");
    kvp.put("bob", "bob@gmail");

    kvp.run();
    return 0;
}
