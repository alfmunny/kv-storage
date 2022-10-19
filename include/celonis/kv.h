#pragma once
#include <eva01/noncopyable.h>
#include <eva01/mutex.h>

#include <string>
#include <unordered_map>
#include <tuple>

namespace celonis {

class KVDatabase : public eva01::NonCopyable {
public:
    bool put(const std::string& key, const std::string& value);
    std::tuple<bool, std::string> get(const std::string& key) const;
    bool del(const std::string& key);
private:
    std::unordered_map<std::string, std::string> m_db;
    mutable eva01::RWMutex m_mutex; // read/write mutex
};

} // namespace celonis
