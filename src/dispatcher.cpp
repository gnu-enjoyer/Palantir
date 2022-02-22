#include <thread>
#include <chrono>
#include "dispatcher.h"
#include "scraper.h"
#include "fmt/format.h"
#include "magic_enum.hpp"

Dispatcher::Dispatcher() {

    if(Logger::get().loadConfig())
        pCfg = Logger::get().Cfg();

    auto socket  = UNIX_SOCKET(pCfg->ipc_path.c_str(),
                               pairPtr);
    socketPtr = &socket;  //no return no problem

    pRedis = redisConnect(pCfg->redis_ip.c_str(),
                          pCfg->redis_port);

    if(!checkRedis()) return;

    Logger::get().write("[Redis] Initialised OK.");

    initSocket();
    processQueue();

}

Dispatcher::~Dispatcher() {

    if(pRedis)
        redisFree(pRedis);

}

bool Dispatcher::initSocket() {
    /// polls the IPC socket on another thread

    if(!socketPtr) return false;

    std::thread netLoop(
            [this]{socketPtr->networkPoll();}
            );

    netLoop.detach();

    //don't wake the sleeper:
    std::thread sendLoop(
            [this]{
                socketPtr->networkSend();
            });

    sendLoop.detach();

    return true;

}

[[noreturn]] void Dispatcher::processQueue() {
    ///waits for queue to fill and dispatches tasks

    for(;;)
    {
        if(auto latestPacket = inQ.pop()){

            fmt::print("[Dispatcher] IN \n");
            E::dataPacketOut outPacket;

            if(queryRedis(latestPacket->id, latestPacket->expac, outPacket.data)){
                outPacket.status = E::E_JSON_MESSAGE::E_FOUND;
                fmt::print("[Dispatcher] Item exists in Redis \n");

            }else{
                auto scraper = Scraper(*latestPacket);
                outPacket = scraper.Scrape();
                writeRedis(latestPacket->id, &outPacket.data, outPacket.expac);
            }

            //verify, then push..
            outQ.push(outPacket);
            fmt::print("[Dispatcher] OUT \n");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

bool Dispatcher::checkRedis(){

    if (pRedis == NULL || pRedis->err) {
        if (pRedis)
            Logger::get().write("[Redis] Error", true);
        else
            Logger::get().write("[Redis] Can't allocate Redis context", true);

        return false;
    }

    return true;
}

bool Dispatcher::queryRedis(int in, E::E_EXPANSION e, std::string& value) {
    if(!checkRedis()) return false;

    auto key = fmt::format_int(in).c_str();
    //TODO: Sane input

    redisReply* first = (redisReply *) redisCommand(
            pRedis,
            "GET %s%s .",
            magic_enum::enum_name(e).data(),
            key);

    if(first->type != REDIS_REPLY_STRING) {
        fmt::print("[Redis] Key not found. \n");
        freeReplyObject(first);
        return false;
    }

    value = first->str;
    freeReplyObject(first);
    return true;
}

bool Dispatcher::writeRedis(int in, const std::string* sPtr, const E::E_EXPANSION e) {
    if(!checkRedis()) return false;

    auto c = fmt::format_int(in).c_str();
    //TODO: Sane inputs before writing DB

    redisReply* reply = (redisReply*) redisCommand(
            pRedis,
            " SET %s%s . %s",
            magic_enum::enum_name(e).data(),
            c,
            sPtr->c_str()
    );

    if(reply->type == REDIS_REPLY_ERROR){
        Logger::get().write("[Redis] Error saving item", true);
        freeReplyObject(reply);
        return false;
    }

    fmt::print("[Redis] Saved item. \n");
    freeReplyObject(reply);
    return true;
}