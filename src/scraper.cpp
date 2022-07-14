#define CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_THREAD_POOL_COUNT 1

#include "scraper.h"
#include <utility>
#include "httplib.h"
#include "pugixml.hpp"
#include "magic_enum.hpp"

static constexpr std::string BuildURL(Palantir::E_EXPANSION Enum) {

    return "https://" + std::string(magic_enum::enum_name(Enum)) + ".wowhead.com";
}

static const httplib::Headers headers
        {
                {"User-Agent",      "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0) Gecko/20100101 Firefox/95.0"},
                {"Accept-Encoding", "gzip"},
                {"Keep-Alive",      "0"}
        };

Scraper::~Scraper() {
}

Scraper::Scraper(const Palantir::dataPacketIn &inPacket, std::shared_ptr<Palantir::IPCQueue> Ptr)
        : sharedPtr(std::move(Ptr)) {
    std::string path = "/item=" + std::to_string(inPacket.id) + "&xml";

    httplib::Client client(BuildURL(inPacket.expac));

    auto res = client.Get(path.c_str());

    res->status == 200 ? ParseXML(res->body) : SendError();
}

void Scraper::ParseXML(const std::string &str) {

    pugi::xml_document doc;

    pugi::xml_parse_result res = doc.load_string(str.c_str());

    if (res.status != pugi::status_ok)
        SendError();
    else
        for (auto &i: doc.select_nodes("wowhead/item/json"))
            i.node().child_value() ? sharedPtr->Out.emplace(
                    Palantir::dataPacketOut{i.node().child_value()}
            ) : SendError();
}

void Scraper::SendError() {

    sharedPtr->Out.emplace(
            Palantir::dataPacketOut{"ERROR"}
    );

    //Log error
}

