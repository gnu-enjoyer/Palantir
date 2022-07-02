#include "cache.h"

#include <stdexcept>
#include "hiredis.h"
#include "magic_enum.hpp"

Cache::Cache(const std::string& IP, int Port) {

    RedisPtr = redisConnect(IP.c_str(), Port);

    if(!RedisPtr || RedisPtr->err != 0) throw std::runtime_error("Redis could not connect");
}

Cache::~Cache() {

    if(RedisPtr)
        redisFree(RedisPtr);
}

bool Cache::Interact(int i, Palantir::E_EXPANSION Enum, std::string &str, bool query) {

    bool result = false;

    redisReply* reply;

    if (query) {

       reply = static_cast<redisReply*>(redisCommand(
                RedisPtr,
                "GET %s%s .",
                magic_enum::enum_name(Enum).data(),
                std::to_string(i).c_str()
        ));

        if(result = reply->type == REDIS_REPLY_STRING; result)
            str = reply->str;

    } else {

        reply = static_cast<redisReply*>(redisCommand(
                RedisPtr,
                " SET %s%s . %s",
                magic_enum::enum_name(Enum).data(),
                std::to_string(i).c_str(),
                str.c_str()
        ));

        result = reply->type != REDIS_REPLY_ERROR;
    }

    freeReplyObject(reply);

    return result;
}
