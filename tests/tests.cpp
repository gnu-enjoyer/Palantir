
#include "cache.h"
#include "net.h"
#include "scraper.h"
#include "palantir.hpp"

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

bool testSocket(const std::string& str){

    std::make_unique<Socket>(str.c_str());

    return true;
}


bool testRedis(const std::string& IP, int Port){

    auto cache = Cache(IP, Port);

    return true;
}

int main(int argc, char* argv[]) {

    //Catch2
    int result = Catch::Session().run(
            argc, argv
            );
    return result;
}

TEST_CASE("Packets: JSON Parse") {

    auto testpacketIn = Palantir::dataPacketIn{19019};

    nlohmann::json j_obj = testpacketIn;

    REQUIRE(testpacketIn.id == 19019);

    REQUIRE(testpacketIn.expac == Palantir::E_EXPANSION::CLASSIC);

    REQUIRE(testpacketIn.type == Palantir::E_INPUT::PING);
}

TEST_CASE("Config: Redis") {

    Palantir::Config CfgTest;

    CfgTest.LoadConfig();

    auto Cfg = CfgTest.GetConfig();

    REQUIRE(testRedis(Cfg->redis_ip, Cfg->redis_port) == true);

    REQUIRE(testSocket(Cfg->ipc_path) == true);
}

TEST_CASE("Config: UNIX Socket") {

    Palantir::Config CfgTest;

    CfgTest.LoadConfig();

    auto Cfg = CfgTest.GetConfig();

    REQUIRE(testSocket(Cfg->ipc_path) == true);
}

TEST_CASE("Scraper: HTTPS") {

    auto QueuePtr = std::make_shared<Palantir::IPCQueue>();

    auto S = Scraper(
            Palantir::dataPacketIn{19019}, QueuePtr
            );
}
