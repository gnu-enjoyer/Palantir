#ifndef PALANTIR_DISPATCHER_H
#define PALANTIR_DISPATCHER_H

#include <hiredis/hiredis.h>
#include "palantir.hpp"
#include "net.h"

class Dispatcher {

    mutexQueue<E::dataPacketIn> inQ;
    mutexQueue<E::dataPacketOut> outQ;
    qPtr pairPtr{&inQ, &outQ};

    UNIX_SOCKET *socketPtr;
    redisContext *pRedis;

    bool initSocket();

    bool queryRedis(int in, E::E_EXPANSION e, std::string &value);

    bool writeRedis(int in, const std::string *sPtr, const E::E_EXPANSION e);

public:

    inline bool checkRedis();

    [[noreturn]] void processQueue();

    Dispatcher();

    ~Dispatcher();

};


#endif //PALANTIR_DISPATCHER_H