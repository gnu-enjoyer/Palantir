#include <iostream>

#include "dispatcher.h"
#include "scraper.h"
#include "palantir.hpp"

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

bool testRedis(const char* ip, int port){

    //check we have a local connection to Redis on the expected port
    auto result = redisConnect(ip, port);

    if (result == nullptr || result->err)
        return false;
    else
        return true;

}

bool testHTTPS(){

    //check https works (i.e. openssl) and firewall issues
    E::dataPacketIn testPacket{
            19019,
            E::E_INPUT::E_PING,
            E::E_EXPANSION::TBC
    };

    auto scraper = Scraper(testPacket);

    if(auto output = scraper.Scrape();
    output.status != E::E_JSON_MESSAGE::E_NOT_FOUND)
        return true;

    return false;

}

bool testJSON(){

    //check data serialises properly via macros from palantir.hpp
    E::dataPacketIn p1{
            19019,
            E::E_INPUT::E_QUERY,
            E::E_EXPANSION::TBC
    };

    nlohmann::json j = p1;

    if(auto p2 = j.get<E::dataPacketIn>(); p1.id != p2.id)
        return false;

    return true;

}

bool testCfg(){

    //check the cfg file loads ok
    E::configFile const *pCfg;

    if(Logger::get().loadConfig()) {
        pCfg = Logger::get().Cfg();

        std::cout << "IPC file descriptor: " << pCfg->ipc_path << "\n";
        std::cout << "Redis IP: " << pCfg->redis_ip << "\n";
        std::cout << "Redis Port: " << pCfg->redis_port << std::endl;

        return true;
    }

    return false;

}

int main(int argc, char* argv[]) {

    //Catch2
    int result = Catch::Session().run(
            argc, argv
            );
    return result;
}

TEST_CASE("JSON+Config") {
    REQUIRE(testJSON() == true);
    REQUIRE(testCfg() == true);
}

TEST_CASE("External HTTPS") {
    REQUIRE(testHTTPS() == true);
}

TEST_CASE("Redis") {

    if(Logger::get().loadConfig(); E::configFile const *pCfg = Logger::get().Cfg()){
        REQUIRE(testRedis(pCfg->redis_ip.c_str(),
                          pCfg->redis_port) == true);
    }
    else
        REQUIRE(testRedis("127.0.0.1", 6379) == true);

}