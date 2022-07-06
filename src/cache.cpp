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

    redisPtr = redisConnect(IP.c_str(), Port);

    if(!redisPtr || redisPtr->err != 0) throw std::runtime_error("Redis could not connect");

    redisSharedPtr = std::make_shared<class redisContext>(*redisPtr);
}

Cache::~Cache() {

    redisFree(redisPtr);
}

std::optional<std::string> Cache::Interact(const Palantir::dataPacketIn &In, bool query) {

    redisQuery g{redisSharedPtr, std::move(In)};

    return g.Process(query) ? g.buffer : std::optional<std::string>();
}


Cache::redisQuery::~redisQuery() {
    freeReplyObject(reply);
}

bool Cache::redisQuery::Process(bool query) {

    bool result = false;

    if(reply = static_cast<redisReply*>(redisCommand(
                parentRedisContext.lock().get(),
                RedisQuery(query, in.expac, std::to_string(in.id), &buffer).c_str()
        )); reply != nullptr){

        if(reply->type == REDIS_REPLY_STRING && reply->type != REDIS_REPLY_ERROR) {
            buffer = reply->str;
            result = true;
        }
    }

    return result;
}
