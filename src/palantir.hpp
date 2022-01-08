#ifndef PALANTIR_ENUMS_H
#define PALANTIR_ENUMS_H

#include <fstream>
#include <chrono>
#include <iostream>
#include <syncstream>
#include <thread>

#include "fmt/format.h"
#include "fmt/os.h"
#include "fmt/chrono.h"

#include <optional>
#include <queue>
#include <mutex>
#include "fmt/core.h"
#include "nlohmann/json.hpp"


namespace E {

    enum class E_INPUT : int {
        E_PING,
        E_QUERY
    };

    enum class E_EXPANSION : int {
        NONE,
        CLASSIC,
        TBC,
        WOTLK
    };

    enum class E_JSON_MESSAGE : int {
        E_WELCOME,
        E_ERROR,
        E_HEARTBEAT,
        E_FOUND,
        E_NOT_FOUND
    };

    //{"type":0} to ping -> {"status":2} is heartbeat
    //{"id":19019, "type":1, "expac":1}

    struct dataPacketIn {

        int id;
        E_INPUT type = E_INPUT::E_PING;
        E_EXPANSION expac = E_EXPANSION::NONE;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(dataPacketIn, id, type, expac)

    struct dataPacketOut {

        std::string data;
        E_JSON_MESSAGE status = E_JSON_MESSAGE::E_ERROR;
        E_EXPANSION expac = E_EXPANSION::NONE;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(dataPacketOut, data, status, expac)


}

class Logger {

    //Meyers Singleton
    Logger() = default;

    ~Logger() = default;

    Logger(const Logger &) = delete;

    Logger &operator=(const Logger &) = delete;

    std::ofstream *logfile = new std::ofstream("log.txt",
                                               std::fstream::app);

public:

    static Logger &get() {
        static Logger instance;
        // volatile int dummy{};
        return instance;
    }


    void write(const char *in, bool err = false) {
        *logfile << fmt::format("{0:%F_%T}",
                                std::chrono::system_clock::now());

        err ? *logfile << " [ERROR] " << in << std::endl : *logfile << " [INFO] " << in << std::endl;
    }

};

template<typename T>
class mutexQueue {

    std::queue<T> queue;
    mutable std::mutex mutex;

public:

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) { return {}; }

        T tmp = queue.front();
        queue.pop();
        return tmp;
    }

    void push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(item);
    }

};

typedef std::pair<mutexQueue<E::dataPacketIn> *, mutexQueue<E::dataPacketOut> *> qPtr;

#endif //PALANTIR_ENUMS_H