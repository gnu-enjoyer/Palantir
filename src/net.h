#ifndef PALANTIR_SOCKET_H
#define PALANTIR_SOCKET_H

#include "palantir.hpp"

#include <vector>
#include <sys/socket.h>
#include <sys/un.h>

#define PATH "/tmp/dbg2"

class UNIX_SOCKET {

    int local_fd = socket(AF_UNIX,
                          SOCK_STREAM, 0);

    const sockaddr soc_serv = {AF_UNIX,
                               PATH};

    int remote_fd;
    qPtr queuePtr;

    inline void sendJSON(E::E_JSON_MESSAGE e, E::dataPacketOut *pPtr = nullptr) const;

public:

    [[noreturn]] void networkPoll();

    [[noreturn]] void networkSend();

    explicit UNIX_SOCKET(qPtr &in);

    ~UNIX_SOCKET();


};

#endif //PALANTIR_SOCKET_H
