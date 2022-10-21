#include <celonis/kv_primary.h>
#include <iostream>

namespace celonis {

KVPrimary::KVPrimary(uint16_t port,
                     std::vector<std::pair<std::string, int>> snodes)
    : m_server(port)
{
    for (auto& node : snodes) {
        auto client = std::make_shared<rpc::client>(node.first, node.second);
        m_clients.emplace_back(std::move(client));
    }

    m_server.bind("PUT", [&](const std::string& key, const std::string& val) {
        return put(key, val);
    });

    m_server.bind("DEL", [&](const std::string& key) {
        return del(key);
    });

    m_server.bind("GET", [&](const std::string& key) {
        return get(key);
    });
}

void KVPrimary::run() { m_server.run(); }

std::tuple<bool, std::string> KVPrimary::get(const std::string &key) {
    return m_clients[selectShard(key)]->call("GET", key).as<std::tuple<bool, std::string>>();
}

bool KVPrimary::put(const std::string &key, const std::string &val) {
    return m_clients[selectShard(key)]->call("PUT", key, val).as<bool>();
}

bool KVPrimary::del(const std::string &key) {
    return m_clients[selectShard(key)]->call("DEL", key).as<bool>();
}

int KVPrimary::selectShard(const std::string& key)
{
    if (key.empty()) {
        return 0;
    }
    // Simple key mapping
    // Use the first character of the key to map to a unique shard
    int shard = (int)key[0] % m_clients.size();
    std::cout << "Shard selected " << shard << std::endl;
    return shard;
}

}  // namespace celonis
