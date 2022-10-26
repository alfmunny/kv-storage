#include <celonis/kv_service.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    const std::string help = 
    R"(Usage: 
       
       secondary primary persistent_node port capacity eviction_policy thread_pool_size

       - primary: domain name or address and port of the primary node 
       - persistent_node: domain name or address and port of the persistent node
       - port: port to use for this secondary node 
       - capacity: capacity of the in memory db in number 
       - policy: eviction policy when capacity reached, 1: LRU, 2: LFU, 3: FIFO. default is 1.
       - thread_pool_size: thread pool size)";

    if (argc < 4) {
        std::cout << help << std::endl;
        return EXIT_FAILURE;
    }

    std::string primary = argv[1];
    std::string persistent = argv[2];
    int port = atoi(argv[3]);

    if (argc == 4) {
        celonis::KVService kv_service(primary, persistent, port);
        kv_service.run();
    } else if (argc >= 5) {
        int db_capacity = atoi(argv[4]);
        if (argc == 6) {
            int policy_nr = atoi(argv[5]);
            using EvictionPolicy = celonis::KVService::EvictionPolicy;
            EvictionPolicy policy;
            switch(policy_nr) {
                case(1):
                    policy = EvictionPolicy::LRU;
                    break;
                case(2):
                    policy = EvictionPolicy::LFU;
                    break;
                case(3):
                    policy = EvictionPolicy::FIFO;
                    break;
                default:
                    policy = EvictionPolicy::LRU;
                    break;
            }
            celonis::KVService kv_service(primary, persistent, port, db_capacity, policy);
            kv_service.run();
        } else {
            celonis::KVService kv_service(primary, persistent, port, db_capacity);
            kv_service.run();
        }
    }


    return EXIT_SUCCESS;
}
