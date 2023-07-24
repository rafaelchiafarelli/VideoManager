#pragma once
#include <string.h>
#include <string>
#include "stdio.h"
#include <vector>
#include <container.hpp>
#include <atomic>
#include <thread>
#include <opencv2/opencv.hpp>
#include <mutex>
class Player
{
private:

    void glue(cv::Mat src, cv::Mat dst, cv::Rect region);
    container c;
    
    std::thread work;
    std::atomic_bool alive;
    void transparency(cv::Mat img);
    void work_func(int,int,int);
    std::mutex mu;

public:
    void Start();
    void Stop();
    std::vector<std::string> OnDemandListName;
    bool OnDemand(std::string name);
    Player(std::string config_file);
    ~Player(){
        if(alive)
        {
            alive = false;
            work.join();
        }
    }
};