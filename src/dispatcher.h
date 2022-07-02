#ifndef PALANTIR_DISPATCHER_H
#define PALANTIR_DISPATCHER_H

#include <memory>
#include "palantir.hpp"

class Dispatcher {

    Palantir::Config Config;

    std::shared_ptr<Palantir::IPCQueue> QueuePtr;

    std::unique_ptr<class Socket> SocketPtr;

    std::unique_ptr<class Cache> CachePtr;

    [[noreturn]] void Poll();

public:
    void Start(bool blocking=true);

    Dispatcher();

    ~Dispatcher();
};


#endif //PALANTIR_DISPATCHER_H
