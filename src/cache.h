#ifndef PALANTIR_CACHE_H
#define PALANTIR_CACHE_H

#include <string>
#include "palantir.hpp"

class Cache {

    class redisContext* RedisPtr;

public:
    bool Interact(int i, Palantir::E_EXPANSION Enum, std::string& str, bool query=true);

    Cache(const std::string& IP, int Port);

    ~Cache();
};


#endif //PALANTIR_CACHE_H
