#ifndef PALANTIR_CACHE_H
#define PALANTIR_CACHE_H

#include <string>
#include "palantir.hpp"

class Cache {

    struct redisQuery {

        std::weak_ptr<class redisContext> parentRedisContext;

        Palantir::dataPacketIn in;

        std::string buffer;

        class redisReply* reply = nullptr;

        bool Process(bool query = true);

        ~redisQuery();
    };

    std::shared_ptr<class redisContext> redisSharedPtr;

    class redisContext* redisPtr;

public:
    std::optional<std::string> Interact(const Palantir::dataPacketIn& In, bool query=true);

    Cache(const std::string& IP, int Port);

    ~Cache();
};


#endif //PALANTIR_CACHE_H
