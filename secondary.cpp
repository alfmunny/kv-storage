#include <celonis/kv_service.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout
            << "Usage: provide address of primary and port to use for service"
            << std::endl;
        return EXIT_FAILURE;
    }

    std::string primary = argv[1];
    int port = atoi(argv[2]);
    celonis::KVService kv_service(primary, port);
    kv_service.run();

    return EXIT_SUCCESS;
}
