#include "player.hpp"
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "opencv2/opencv.hpp"

void Player::OnDemand(std::string name)
{
    /* play on demand */
    OnDemandListName.push_back(name);
}

void Player::Start()
{

    alive = true;
    
    work = std::thread(&Player::work_func,this);
}

void Player::Stop()
{
    alive =false;
    work.join();

}
void Player::work_func(){
    while(alive)
    {

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
Player::Player(std::string config_file)
{
    alive = false;
    if(c.parser(config_file))
    {
        std::cout<<"configuration loaded -- loading images"<<std::endl;
        for(auto &[key, value]: c.sequences)
        {
            std::cout<<"name:"<<key<<std::endl;
            if(value.names.size() != value.images.size())
            {
                std::cout<<"error: wrong size names:"<<value.names.size()<<" images:"<<value.images.size()<<std::endl;
                continue;
            }
            for(unsigned i =0; i < value.names.size(); i++)
            {
                std::string f_name = value.names[i];
                std::cout<<"name of current file:"<<f_name<<std::endl;
                std::ifstream img_f(f_name);
                if(img_f.good())
                {
                    cv::Mat tmp =imread(f_name, cv::IMREAD_UNCHANGED);
                    tmp.copyTo(value.images[i]);
                }
                else
                {
                    std::cout<<"error: image not found "<<std::endl;
                    continue;
                }
            }
        }
    }
    else
    {
        std::cout<<"configuration not ok. missing opportunity"<<std::endl;
    }
}


/**
 * @brief this function will glue the images together. It is used internally to "insert" some image over the image  to be displayed.
 * 
 * @param src is the image source (the image to get pixels from)
 * @param dst is the image destination (the image to get pixels to)
 * @param region is the region to be overriden
 */
void Player::glue(cv::Mat src, cv::Mat dst, cv::Rect region)
{
    cv::Mat mask;
    std::vector<cv::Mat> rgbLayer;
    cv::split(src, rgbLayer); // seperate channels
    cv::Mat cs[3] = {rgbLayer[0], rgbLayer[1], rgbLayer[2]};
    cv::merge(cs, 3, src); // glue together again
    mask = rgbLayer[3];    // png's alpha channel used as mask
    src.copyTo(dst(region), mask);
}
