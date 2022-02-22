#include "net.h"

#include <thread>
#include <chrono>
#include <unistd.h>

UNIX_SOCKET::UNIX_SOCKET(const char* fd, qPtr& in) {

    queuePtr = in;
    int keepalive = 1;

    sockaddr_un soc_serv{
        AF_UNIX,
        "/tmp/palantir"
    };

    strcpy(soc_serv.sun_path, fd);

    try{
        unlink(fd);
        bind(local_fd, (sockaddr*)&soc_serv, sizeof soc_serv);
        listen(local_fd, 1);
        setsockopt(local_fd, SOL_SOCKET,
                              SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    }catch(...){Logger::get().write("[Palantir] Socket setup error.", true); return;}


}

UNIX_SOCKET::~UNIX_SOCKET() {

    close(local_fd);

}

[[noreturn]] void UNIX_SOCKET::networkPoll() {

    char ibuff[128];
    ssize_t incoming = 0;

    for(;;) {

        remote_fd = accept(local_fd,
                           NULL, NULL);

        if (remote_fd != -1) {
            sendJSON(E::E_JSON_MESSAGE::E_WELCOME);

            recv_loop:

            bzero(ibuff, sizeof(ibuff));

            incoming = recv(remote_fd,
                            ibuff, sizeof(ibuff), 0);

            if ((incoming > 1) && (incoming <= 128)) {
                try {
                    auto packet = nlohmann::json::parse(ibuff);


                    if(packet["type"] == E::E_INPUT::E_PING) sendJSON(E::E_JSON_MESSAGE::E_HEARTBEAT);

                    else
                        queuePtr.first->push(packet.get<E::dataPacketIn>());

                    fmt::print("[Socket] RECV \n");
                }
                catch (...) {
                    fmt::print("[Socket] Malformed request. \n");
                    Logger::get().write("[Socket] Malformed request.", true);
                    sendJSON(E::E_JSON_MESSAGE::E_ERROR);
                }
            } else {
                sendJSON(E::E_JSON_MESSAGE::E_ERROR);
            }

            //TODO: Wake on SIGIO rather than sleepin
            //sleep(1); //handled by thread caller
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            goto recv_loop;
        }
    }
    //goto recv_loop;
}

void UNIX_SOCKET::sendJSON(E::E_JSON_MESSAGE e, E::dataPacketOut *pPtr) const {

    nlohmann::json j_obj;
    j_obj["status"] = E::E_JSON_MESSAGE(e);

    switch(e){

        case E::E_JSON_MESSAGE::E_FOUND:
        {
            if(pPtr){
                j_obj = *pPtr;
            }
            break;
        }

        case E::E_JSON_MESSAGE::E_ERROR:
        {
            Logger::get().write("[Socket] Error", true);
            fmt::print("[Socket] Error logged! \n");
            break;
        }


        default:
        {
            fmt::print("[Socket] Connected via IPC. \n");
            break;
        }

    }

    auto str = j_obj.dump();
    send(remote_fd, str.c_str(), str.size(), 0);

}

[[noreturn]] void UNIX_SOCKET::networkSend() {

    for(;;) {

        if (auto outP = queuePtr.second->pop()) {

            sendJSON(E::E_JSON_MESSAGE::E_FOUND, &*outP);
            fmt::print("[Socket] SENT \n");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

}