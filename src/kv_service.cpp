#include <celonis/kv_service.h>

namespace celonis {

static eva01::Logger::ptr g_logger = EVA_ROOT_LOGGER();

bool KVService::put(const std::string& key, const std::string& val) {
    EVA_LOG_DEBUG(g_logger) << "KVService put";
    return m_kvdb.put(key, val);
}

std::tuple<bool, std::string> KVService::get(const std::string& key) {
    EVA_LOG_DEBUG(g_logger) << "KVService get";
    return m_kvdb.get(key);
}

bool KVService::del(const std::string& key)  {
    EVA_LOG_DEBUG(g_logger) << "KVService del";
    return m_kvdb.del(key);
}

void KVService::run()  {
    EVA_LOG_DEBUG(g_logger) << "KVService run";
    m_server.run();
}
}
