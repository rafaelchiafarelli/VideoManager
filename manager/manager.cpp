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

    
    P.Start();
    int count =0;
    app.port(18080).run();
    return 0;
}