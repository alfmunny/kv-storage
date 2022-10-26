#pragma once
#include <eva01/noncopyable.h>
#include <eva01/mutex.h>

#include <string>
#include <unordered_map>
#include <tuple>
#include <list>
#include <memory>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

namespace celonis {

class KVDatabase : public eva01::NonCopyable {
public:
    static const constexpr char* pdb_user = "root";
    static const constexpr char* pdb_pass = "password";

    enum EvictionPolicy {
        LRU,
        LFU,
        FIFO
    };

    KVDatabase(const std::string& persistent_db, const uint64_t capacity, const EvictionPolicy eviction_policy);
    ~KVDatabase();

    bool put(const std::string& key, const std::string& value);
    std::tuple<bool, std::string> get(const std::string& key);
    bool del(const std::string& key);
    
    void evict();

private:
    void updateLRU(const std::string& key, const std::string& value);
private:
    std::string m_pdb;
    uint64_t m_capacity;
    uint64_t m_total_bytes;
    EvictionPolicy m_epolicy;
    using KVVal = std::pair<std::string, std::list<std::string>::iterator>;
    std::unordered_map<std::string, KVVal> m_db;
    std::list<std::string> m_lru;
    mutable eva01::RWMutex m_mutex; // read/write mutex
    sql::Connection* m_conn;
    sql::Statement* m_stmt;
    sql::PreparedStatement* m_pstmt;
    sql::ResultSet* m_res;
};

} // namespace celonis
