#include <celonis/kv_service.h>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    const std::string help = 
    R"(Usage: 
       
       secondary primary port persistent_node capacity eviction_policy thread_pool_size

       - primary: domain name or address and port of the primary node 
       - port: port to use for this secondary node 
       - (Optional) persistent_node: domain name or address and port of the persistent node
       - (Optional) capacity: capacity of the in-memory db in number, default is 1.000.000 records
       - (Optional) policy: eviction policy when capacity reached, 1: LRU, 2: LFU, 3: FIFO. default is 1.
       - (Optional) thread_pool_size: thread pool size, default is 1)";

    if (argc < 3) {
        std::cout << help << std::endl;
        return EXIT_FAILURE;
    }

    std::string primary = argv[1];
    int port = atoi(argv[2]);
    std::string persistent;

    if (argc == 3) {
        celonis::KVService kv_service(primary, port);
        kv_service.run();
    }
    else if (argc == 4) {
        persistent = argv[3];
        celonis::KVService kv_service(primary, port, persistent);
        kv_service.run();
    } else if (argc >= 5) {
        persistent = argv[3];
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
            celonis::KVService kv_service(primary, port, persistent, db_capacity, policy);
            kv_service.run();
        } else {
            celonis::KVService kv_service(primary,  port, persistent, db_capacity);
            kv_service.run();
        }
    }

    return EXIT_SUCCESS;
}
