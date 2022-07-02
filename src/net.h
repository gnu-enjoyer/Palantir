#ifndef PALANTIR_NET_H
#define PALANTIR_NET_H

#include <memory>
#include <sys/socket.h>
#include "palantir.hpp"

class Socket {

    int local_fd = socket(AF_UNIX,
                          SOCK_STREAM, 0);
    int remote_fd = 0;

    void SendJSON(const Palantir::dataPacketOut& Data);

public:
    [[noreturn]] void Poll(std::shared_ptr<Palantir::IPCQueue> SharedPtr);

    explicit Socket(const char* fd);

    ~Socket();
};


#endif //PALANTIR_NET_H
