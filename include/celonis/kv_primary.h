#pragma once

#include <rpc/client.h>
#include <rpc/server.h>

#include <unordered_map>
#include <memory>

namespace celonis {

/* KVPrimary
 * Provide primary services for kv storage.
 */
class KVPrimary {
public:
    /*
     * Constructor
     */
    KVPrimary(uint16_t port, std::vector<std::pair<std::string, int>> snodes);

    /* GET
     * If key exists, return true and value
     */
    std::tuple<bool, std::string> get(const std::string &key);

    /* PUT
     * If key exists, update value
     * If key does not exist, add key and value
     */
    bool put(const std::string &key, const std::string &val);

    /* DEL
     * If key does not exist, return false
     * If key exists, delete key and value, and return false
     */
    bool del(const std::string &key);

    /* Run service, block current thread
     */
    void run();

private:
    std::unordered_map<std::string, int> m_ltb;
    std::vector<std::pair<std::string, int>> m_snodes;
    std::vector<std::shared_ptr<rpc::client>> m_clients;
    rpc::server m_server;

    int selectShard(const std::string& key);
};

}  // namespace celonis
