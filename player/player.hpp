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

    void glue(std::string f_name, cv::Mat dst, cv::Rect region);
    std::string config_file;
    void load_config(std::string config_file);
    std::thread work;
    std::atomic_bool alive;
    void transparency(cv::Mat img);
    void work_func();
    std::mutex mu;

public:
    container c;
    void Start();
    void Stop();
    void Restart(){
        Stop();
        load_config(config_file);
        Start();
    }

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