#ifndef PALANTIR_SCRAPER_H
#define PALANTIR_SCRAPER_H

#include <memory>

#include "palantir.hpp"

class Scraper {

    static constexpr std::array nodes
            {
                    "/wowhead/item/json",
                    "/wowhead/item/jsonEquip",
                    "/wowhead/item/htmlTooltip"
            };

    std::shared_ptr<Palantir::IPCQueue> sharedPtr;

    void ParseXML(const std::string& str);

    void SendError();

public:
    Scraper(const Palantir::dataPacketIn& inPacket, std::shared_ptr<Palantir::IPCQueue> Ptr);

    ~Scraper();
};


#endif //PALANTIR_SCRAPER_H
