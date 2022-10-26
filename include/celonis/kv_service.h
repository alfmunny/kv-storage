#pragma once

#include <celonis/kv.h>
#include <eva01/iomanager.h>
#include <eva01/log.h>
#include <rpc/server.h>

namespace celonis {

/* KVService
 * Provide secondary services for kv storage.
 */
class KVService {

public:
    static const constexpr uint64_t default_kvdb_capacity = 1000000;
    using EvictionPolicy = KVDatabase::EvictionPolicy;
    /* Constructor
     */
    KVService(const std::string& primary_node, const uint16_t port,
              const std::string& persistent_db = "", 
              const uint64_t kvdb_capacity = default_kvdb_capacity,
              const EvictionPolicy kvdb_policy = EvictionPolicy::LRU, const size_t thread_pool_size = 1);

    /* GET
     * If key exists, return true and value
     */
    std::tuple<bool, std::string> get(const std::string& key);

    /* PUT
     * If key exists, update value
     * If key does not exist, add key and value
     */
    bool put(const std::string& key, const std::string& val);

    /* DEL
     * If key does not exist, return false
     * If key exists, delete key and value, and return false
     */
    bool del(const std::string& key);

    /* Run service, block current thread
     */
    void run();

private:
    void sendHeartbeat(){};

private:
    std::string m_primary_node; // domain name or address of the primary node
    rpc::server m_server; // rpc server
    KVDatabase m_kvdb; // key-value in-memory db
    // eva01::IOManager m_iom;
};

}  // namespace celonis
