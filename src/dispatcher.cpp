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
        {
            std::string buff;

            if(CachePtr->Interact(in->id, in->expac,buff))
                QueuePtr->Out.push(Palantir::dataPacketOut{buff, Palantir::E_JSON_MESSAGE ::FOUND, in->expac});
            else
                Scraper(in.value(), QueuePtr);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds (100));
    }
}
