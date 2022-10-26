#include <celonis/kv_service.h>

namespace celonis {

static eva01::Logger::ptr g_logger = EVA_ROOT_LOGGER();

KVService::KVService(const std::string& primary_node,
                     const uint16_t port,
                     const std::string& persistent_db, 
                     const uint64_t kvdb_capacity,
                     const EvictionPolicy kvdb_policy,
                     const size_t thread_pool_size)
    : m_primary_node(primary_node),
      m_server(port),
      m_kvdb(persistent_db, kvdb_capacity, kvdb_policy)

{
    // EVA_LOGGER("system")->setLevel(eva01::LogLevel::INFO);
    // m_iom.addTimer(1000, [&]() { sendHeartbeat(); }, true);

    m_server.bind("PUT", [&](const std::string& key, const std::string& val) {
        return put(key, val);
    });
    m_server.bind("DEL", [&](const std::string& key) { return del(key); });
    m_server.bind("GET", [&](const std::string& key) { return get(key); });
};

bool KVService::put(const std::string& key, const std::string& val)
{
    EVA_LOG_DEBUG(g_logger) << "KVService put {" << key << ", " << val << "}";
    return m_kvdb.put(key, val);
}

std::tuple<bool, std::string> KVService::get(const std::string& key)
{
    EVA_LOG_DEBUG(g_logger) << "KVService get " << key;
    return m_kvdb.get(key);
}

bool KVService::del(const std::string& key)
{
    EVA_LOG_DEBUG(g_logger) << "KVService del " << key;
    return m_kvdb.del(key);
}

void KVService::run()
{
    EVA_LOG_DEBUG(g_logger) << "KVService run";
    m_server.run();
}
}  // namespace celonis
