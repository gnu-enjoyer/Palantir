#include "cache.h"

#include <stdexcept>
#include "hiredis.h"
#include "magic_enum.hpp"

static constexpr std::string RedisQuery(bool query, Palantir::E_EXPANSION Enum,
                                        const std::string& id, std::string* sptr = nullptr) {
    std::string str;

    query ? str = "GET " : str = "SET ";

    str += magic_enum::enum_name(Enum).data();

    str += id;

    if (query && sptr)
        str += *sptr;

    return str;
}

Cache::Cache(const std::string& IP, int Port) {

    RedisPtr = redisConnect(IP.c_str(), Port);

    if(!RedisPtr || RedisPtr->err != 0) throw std::runtime_error("Redis could not connect");
}

Cache::~Cache() {

    redisFree(RedisPtr);
}

bool Cache::Interact(int i, Palantir::E_EXPANSION Enum, std::string &str, bool query) {

    bool result = false;

    redisReply* reply;

    if(reply = static_cast<redisReply*>(redisCommand(
            RedisPtr,
            RedisQuery(query, Enum, std::to_string(i), &str).c_str()
    )); reply != nullptr){

        if(reply->type == REDIS_REPLY_STRING && reply->type != REDIS_REPLY_ERROR) {
            str = reply->str;
            result = true;
        }
    }

    freeReplyObject(reply);

    return result;
}
