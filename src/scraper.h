#ifndef PALANTIR_SCRAPER_H
#define PALANTIR_SCRAPER_H

#define CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_THREAD_POOL_COUNT 1
#include "httplib.h"

#include "palantir.hpp"
#include "pugixml.hpp"

class Scraper {

    httplib::Headers headers = {
            {"User-Agent",      "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0) Gecko/20100101 Firefox/95.0"},
            {"Accept-Encoding", "gzip"},
            {"Keep-Alive",      "0"}
    };

    E::dataPacketIn *inPtr = nullptr;
    E::dataPacketOut outPacket;

    const char nodes[3][32] = {
            "/wowhead/item/json",
            "/wowhead/item/jsonEquip",
            "/wowhead/item/htmlTooltip"
    };

    const char extra[3][8] = {
            "hit:",
            "uip:",
            ") Set"
    };


    pugi::xml_document doc;
    //typedef std::pair<const char *, size_t> extraPairs;
    //std::vector<extraPairs> vectorPairs;

    bool gougXML(std::string &in);

    inline bool getXML(httplib::Client *httpPtr, int id, std::string &xml);

public:

    explicit Scraper(E::dataPacketIn &inPacket);

    E::dataPacketOut Scrape();

    ~Scraper();

};


#endif //PALANTIR_SCRAPER_H