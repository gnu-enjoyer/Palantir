#include "net.h"

#include <thread>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>

Socket::Socket(const char *fd) {

    int keepalive = 1;

    sockaddr_un soc_serv
    {
            AF_UNIX,
            *fd
    };

    strcpy(soc_serv.sun_path, fd);

    try
    {
        unlink(fd);

        bind(local_fd, (sockaddr*)&soc_serv, sizeof soc_serv);

        listen(local_fd, 1);

        setsockopt(local_fd, SOL_SOCKET,
                   SO_KEEPALIVE, &keepalive, sizeof(keepalive));

    }catch(...){ throw std::runtime_error("Error establishing IPC socket, check fd is valid");}
}

Socket::~Socket() {

    close(local_fd);
}

void Socket::SendJSON(const Palantir::dataPacketOut& Data) {

    nlohmann::json j_obj = Data;

    auto str = j_obj.dump();

    send(remote_fd, str.c_str(), str.size(), 0);
}

void Socket::Poll(std::shared_ptr<Palantir::IPCQueue> SharedPtr) {
    char ibuff[128];
    ssize_t incoming = 0;

    for (;;)
    {
        remote_fd = accept(local_fd,
                           nullptr, nullptr);

        if (remote_fd != -1) {
            SendJSON(Palantir::dataPacketOut{"WELCOME ACK", Palantir::E_JSON_MESSAGE::WELCOME});

            recv_loop:

            bzero(ibuff, sizeof(ibuff));

            incoming = recv(remote_fd,
                            ibuff, sizeof(ibuff), 0);

            /* Recv Loop */
            if ((incoming > 1) && (incoming <= 128)) {
                try {
                    SharedPtr->In.push(nlohmann::json::parse(ibuff));
                } catch (...) { SendJSON(Palantir::dataPacketOut{}); }
            }

            /* Send Loop */
            if (SharedPtr->Out.size() != 0)
                SendJSON(SharedPtr->Out.pop().value());

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            goto recv_loop;
        }
    }
}
