#ifndef PALANTIR_PALANTIR_H
#define PALANTIR_PALANTIR_H

#include <iosfwd>
#include <fstream>
#include <string>
#include <mutex>
#include <optional>
#include <queue>
#include "nlohmann/json.hpp"

namespace Palantir {

    template<typename T>
    class mutexQueue {

        std::queue<T> queue;
        mutable std::mutex mutex;

    public:

        size_t size() const {
            std::scoped_lock lock(mutex);
            return queue.size();
        }

        std::optional<T> pop() {
            std::scoped_lock lock(mutex);
            if (queue.empty()) { return {}; }

            T tmp = queue.front();
            queue.pop();
            return tmp;
        }

        void push(const T &item) {
            std::scoped_lock lock(mutex);
            queue.push(item);
        }

    };

    enum class E_INPUT : int {
        PING,
        QUERY
    };

    enum class E_EXPANSION : int {
        CLASSIC,
        TBC,
        WOTLK
    };

    enum class E_JSON_MESSAGE : int {
        ERROR,
        WELCOME,
        HEARTBEAT,
        FOUND,
        NOT_FOUND
    };

    struct dataPacketIn {

        int id;
        E_INPUT type = E_INPUT::PING;
        E_EXPANSION expac = E_EXPANSION::CLASSIC;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(dataPacketIn, id, type, expac)

    struct dataPacketOut {

        std::string data;
        E_JSON_MESSAGE status = E_JSON_MESSAGE::ERROR;
        E_EXPANSION expac = E_EXPANSION::CLASSIC;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(dataPacketOut, data, status, expac)

    struct configFile {

        std::string ipc_path = "/tmp/palantir";
        std::string redis_ip = "127.0.0.1";
        int redis_port = 6379;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configFile, ipc_path, redis_ip, redis_port)

    struct IPCQueue {

        mutexQueue<dataPacketIn> In;

        mutexQueue<dataPacketOut> Out;
    };

    class Config {

        configFile config;

    public:
        [[nodiscard]] const configFile* GetConfig() const { return &config; }

        void LoadConfig()
            try
            {
                std::ifstream i("palantir.cfg");

                nlohmann::json j;

                i >> j;

                config = j.get<configFile>();
            }
            catch (...) {config = configFile{};}
    };
}


#endif //PALANTIR_PALANTIR_H
