#include "manager.hpp"

#include <crow_all.h>
#include <chrono>
#include <stdio.h>
#include "player.hpp"
#include "manager.hpp"

using namespace cv;
int main(int argc, char** argv )
{
    
    Player P("./test_player.json");
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([&]() {
        P.OnDemand("nome");
        return "Hello world!";
    });
    /*
    {
        "ondemand":name_of_sequence
    }
    
    */
    CROW_ROUTE(app, "/json")
        .methods("POST"_method)
    ([&](const crow::request& req){
        auto raw = crow::json::load(req.body);
        if (!raw)
            return crow::response(400);
        
        if(raw.has("ondemand"))
        {
            if(raw["ondemand"].t() == crow::json::type::String)
            {
                std::string name = raw["ondemand"].s();
                if(!P.OnDemand(name)){
                    return crow::response(404);
                }
            }
        }
        return crow::response{"loaded"};
    });

    
    P.Start();
    int count =0;
    app.port(18080).run();
    return 0;
}