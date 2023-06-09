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

    app.port(18080).run();
    P.Start();
    int count =0;
    while(count < 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        count+=1;
    }
    return 0;
}