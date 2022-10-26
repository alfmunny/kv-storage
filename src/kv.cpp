#include <celonis/kv.h>

#include <eva01/mutex.h>
#include <eva01/log.h>


namespace celonis {

static eva01::Logger::ptr g_logger = EVA_ROOT_LOGGER();

KVDatabase::KVDatabase(const std::string& persistent_db,
                       const uint64_t capacity,
                       const EvictionPolicy eviction_policy)
    : m_pdb(persistent_db), m_capacity(capacity), m_epolicy(eviction_policy)

{
    //Connect to persistent database
    sql::ResultSet* m_res;

    try {
        sql::mysql::MySQL_Driver* driver = driver;
        driver = sql::mysql::get_mysql_driver_instance();

        m_conn = driver->connect(persistent_db, pdb_user, pdb_pass);
        m_conn->setSchema("celonis");

        m_stmt = m_conn->createStatement();
        // Initialize the database table if the table is not exists
        m_stmt->execute("CREATE TABLE if not exists CelonisData "
                "(kv_key VARCHAR(255) PRIMARY KEY, kv_value VARCHAR(1023) NOT NULL)");
    } catch(sql::SQLException &e) {
        EVA_LOG_ERROR(g_logger) << "SQLException: " << e.what();
    }
}

std::tuple<bool, std::string> KVDatabase::get(const std::string& key)
{
    std::string ret;

    {
        eva01::MutexReadGuard lk(m_mutex);
        if (m_db.find(key) == m_db.end()) {
            m_pstmt = m_conn->prepareStatement("SELECT kv_value from CelonisData where kv_key=?");
            m_pstmt->setString(1, key);
            m_pstmt->execute();
            m_res = m_pstmt->getResultSet();
            while(m_res->next()) {
                ret = m_res->getString("kv_value");
                EVA_LOG_DEBUG(g_logger) << "getString: " << ret;
                break;
            }

            if (ret.empty()) {
                return { false, ret };
            }
            delete m_pstmt;
            delete m_res;
        }

        if (m_db.find(key) != m_db.end() && ret.empty()) {
            ret = m_db.at(key).first;
        }
    }


    put(key, ret);

    return {true, ret};
}

bool KVDatabase::put(const std::string& key, const std::string& value)
{
    {
        eva01::MutexWriteGuard lk(m_mutex);
        if (m_db.size() == m_capacity && m_db.find(key) == m_db.end()) {
            evict();
        }
    }
    updateLRU(key, value);
    return true;
};

bool KVDatabase::del(const std::string& key)
{
    {
        eva01::MutexReadGuard lk(m_mutex);
        if (m_db.find(key) == m_db.end()) {
            return false;
        }
    }

    {
        eva01::MutexWriteGuard lk(m_mutex);
        m_lru.erase(m_db.at(key).second);
        m_db.erase(key);
        return true;
    }
}

void KVDatabase::updateLRU(const std::string& key, const std::string& value)
{
    eva01::MutexWriteGuard lk(m_mutex);
    if (m_db.find(key) != m_db.end()) {
        m_lru.erase(m_db.at(key).second);
    }

    m_lru.emplace_front(key);
    m_db[key].first = value;
    m_db[key].second = m_lru.begin();
}

void KVDatabase::evict() {
    auto key = m_lru.back();
    auto val = m_db.at(key).first;
    m_db.erase(key);
    m_lru.pop_back();
    m_pstmt = m_conn->prepareStatement("INSERT INTO CelonisData(kv_key, kv_value) VALUES(?, ?) ON DUPLICATE KEY UPDATE kv_key=?, kv_value=?");
    m_pstmt->setString(1, key);
    m_pstmt->setString(2, val);
    m_pstmt->setString(3, key);
    m_pstmt->setString(4, val);
    m_pstmt->execute();
    delete m_pstmt;
}

KVDatabase::~KVDatabase() { 
    delete m_conn;
    delete m_stmt;
    delete m_res;
    delete m_pstmt;
    delete m_conn;
}

};  // namespace celonis
