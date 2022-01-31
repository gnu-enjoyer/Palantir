#include "scraper.h"
#include "fmt/format.h"
#include "magic_enum.hpp"

Scraper::~Scraper() {
    fmt::print("[Scraper] Exit. \n");
}

Scraper::Scraper(E::dataPacketIn &inPacket) {
    inPtr = &inPacket;
    //vectorPairs.reserve(5);
}

E::dataPacketOut Scraper::Scrape() {

    assert(this->inPtr);

    E::dataPacketOut outP;
    outP.expac = inPtr->expac;

    switch(inPtr->expac){

        default: {
            Logger::get().write("[Scraper] Internal error", true);
            return outP;
        }

        case E::E_EXPANSION::CLASSIC:
        {
            httplib::Client cli = httplib::Client("https://classic.wowhead.com");
            getXML(&cli, inPtr->id, outP.data);
            break;
        }

        case E::E_EXPANSION::TBC:
        {
            httplib::Client cli = httplib::Client("https://tbc.wowhead.com");
            getXML(&cli, inPtr->id, outP.data);
            break;
        }

    }

    outP.data.empty() ?  outP.status = E::E_JSON_MESSAGE::E_NOT_FOUND : outP.status = E::E_JSON_MESSAGE::E_FOUND;

    if(!outP.data.empty())
        gougXML(outP.data);

    fmt::print("[Scraper] Completed. \n");
    return outP;
}

bool Scraper::getXML(httplib::Client *httpPtr, int id, std::string& xml) {

    httpPtr->set_decompress(true);
    httpPtr->set_follow_location(true);
    httpPtr->set_default_headers(headers);

    std::string path = "/item=" + fmt::format_int(id).str() + "&xml";

    auto res = httpPtr->Get(path.c_str(), headers);

    if(res->status != 200) {
        Logger::get().write("[Scraper] HTTP Error.", true);
        return false;
    }

    if(res->body.find("<erro") != std::string::npos) {
        Logger::get().write("[Scraper] HTTP OK, but item not found.", false);
        return false;
    }

    xml = res->body;
    return true;

}

bool Scraper::gougXML(std::string& in) {

    pugi::xml_parse_result result = doc.load_string(in.c_str());

    if (result.status != pugi::status_ok) return false;

    const size_t npos = std::string::npos;

    nlohmann::json j_obj;
    nlohmann::json j_array = nlohmann::json::array();

    std::string buff;
    std::string ex_buff;

    for (auto &n: nodes) {

        if (n != nodes[2]) {
            buff = "{";
            buff.append(doc.select_nodes(n).first().node().text().as_string());
            buff.append("}");
            j_obj.emplace_back(nlohmann::json::parse(buff));
        } else {
            buff = doc.select_nodes(n).first().node().text().as_string();

            size_t pos;
            size_t offset = 0;

            for (auto &e: extra) {

                start:
                if (pos = buff.find(e, ++offset); pos != npos) {

                    for (--pos; pos < buff.size(); ++pos)
                    {
                        if (buff.at(pos) == '<' && buff.at(++pos) == '/')
                            break;

                        ex_buff.push_back(buff.at(pos));
                    }

                    j_array.push_back(ex_buff);

                    offset = pos;
                    goto start;

                }
            }
        }
    }

    j_obj.push_back(j_array);
    in = j_obj.dump();
    return true;

}