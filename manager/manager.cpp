#include "manager.hpp"


#include <chrono>
#include <stdio.h>
#include "player.hpp"
#include "manager.hpp"

using namespace cv;
int main(int argc, char** argv )
{
    
    Player P("./test_player.json");

    P.Start(1080,1920);
    int count =0;
    while(count < 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        count+=1;
    }
    return 0;
}