#pragma once
#include <string.h>
#include <string>
#include "stdio.h"
#include <vector>
#include <container.hpp>
#include <atomic>
#include <thread>
#include <opencv2/opencv.hpp>

class Player
{
private:
    void glue(cv::Mat src, cv::Mat dst, cv::Rect region);
    container c;
    std::vector<std::string> OnDemandListName;
    std::thread work;
    std::atomic_bool alive;
    void work_func();
public:
    void Start();
    void Stop();
    void OnDemand(std::string name);
    Player(std::string config_file);
    ~Player(){
        if(alive)
        {
            alive = false;
            work.join();
        }
    }
};