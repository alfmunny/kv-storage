#pragma once

#include <celonis/kv.h>
#include <eva01/iomanager.h>
#include <eva01/log.h>
#include <rpc/server.h>

namespace celonis {

/* KVService
 * Provide services for kv storage.
 */
class KVService {
public:
  KVService(const std::string& primary_node, const uint16_t port,
            const size_t thread_pool_size = 1)
      : m_primary_node(primary_node), m_iom(thread_pool_size, "kv_service"),
        m_server(port) {
    EVA_LOGGER("system")->setLevel(eva01::LogLevel::INFO);
    //m_iom.addTimer(1000, [&]() { sendHeartbeat(); }, true);

    m_server.bind("PUT", [&](const std::string& key, const std::string& val) { return put(key, val); });
    m_server.bind("DEL", [&](const std::string& key) { return del(key); });
    m_server.bind("GET", [&](const std::string& key) { return get(key); });
  };

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
  void sendHeartbeat(){};

private:
  std::string m_primary_node;
  eva01::IOManager m_iom;
  rpc::server m_server;
  KVDatabase m_kvdb;
};

} // namespace celonis
