#include "dispatcher.h"

#include <thread>
#include <memory>
#include "scraper.h"
#include "cache.h"
#include "net.h"

Dispatcher::Dispatcher() {

    Config.LoadConfig();

    auto Cfg = Config.GetConfig();

    SocketPtr = std::make_unique<Socket>(Cfg->ipc_path.c_str());

    CachePtr = std::make_unique<Cache>(Cfg->redis_ip.c_str(), Cfg->redis_port);

    QueuePtr = std::make_shared<Palantir::IPCQueue>();
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::Start(bool blocking) {
    if (!SocketPtr || !CachePtr) throw std::runtime_error("Error starting dispatcher: IPC socket or Redis cache not initialised");

    //don't wake the sleeper:
    std::jthread netLoop(
            [this] {
                SocketPtr->Poll(QueuePtr);
            });

    netLoop.detach();

    std::jthread scrapeLoop(
            [this] {
                Poll();
            });

    blocking ? scrapeLoop.join() : scrapeLoop.detach();
}

[[noreturn]] void Dispatcher::Poll() {
    for(;;)
    {
        if(auto in = QueuePtr->In.pop())
            if(auto buff = CachePtr->Interact(in.value()))
                QueuePtr->Out.push(Palantir::dataPacketOut{buff.value(), Palantir::E_JSON_MESSAGE ::FOUND, in->expac});
            else
                Scraper(in.value(), QueuePtr);

        /* Send loop */
        if(auto in = QueuePtr->Out.pop())
            SocketPtr->SendJSON(in.value());

        std::this_thread::sleep_for(std::chrono::milliseconds (100));
    }
}
