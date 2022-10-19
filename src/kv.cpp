#include "eva01/mutex.h"
#include <celonis/kv.h>

namespace celonis {

std::tuple<bool, std::string> KVDatabase::get(const std::string& key) const {
    eva01::MutexReadGuard lk(m_mutex);
    if (m_db.find(key) == m_db.end()) {
        return { false, std::string{} };
    }
    return { true, m_db.at(key) };
}

bool KVDatabase::put(const std::string& key, const std::string& value) {
    eva01::MutexWriteGuard lk(m_mutex);
    m_db[key] = value;
    return true;
};

bool KVDatabase::del(const std::string& key) {
    eva01::MutexWriteGuard lk(m_mutex);
    return m_db.erase(key) > 0;
};

};
