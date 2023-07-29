
#include "player.hpp"
#include <stdio.h>
#include <fstream> // included asked for compile version in orange pi
#include <iostream>
#include <chrono>

#include "opencv2/opencv.hpp"

bool Player::OnDemand(std::string name)
{
    std::vector<std::string>::iterator it;
    std::lock_guard<std::mutex> lock(mu);
    for(auto &sequence: c.animations)
    {
        
        if(!name.compare(sequence.second.name)) //found one
        {
            if(OnDemandListName.size() < c.app.ondemand_max)
            {
                OnDemandListName.push_back(name);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
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
    //create a black image of the desired size -- check size for OrangePi and RaspberryPi machines.
    int w = c.app.window_x;
    int h = c.app.window_y;
    int step = c.app.step_ms;
    //create a namedWindow
    //move to the appropriate place
    //set it as full screen
    //cv::namedWindow("Display", cv::WINDOW_AUTOSIZE );
    cv::namedWindow("Display", cv::WND_PROP_FULLSCREEN );
    cv::moveWindow("Display", -1,-1);
    cv::setWindowProperty ("Display", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    while(alive)
    {   
        //the index of the image is the order on witch it is inserted
        cv::Mat screen_img(h,w,CV_8UC3, cv::Scalar(0,0,0) );        
        for(auto &sequence: c.animations)
        { 
            if(sequence.second.type == permanent)
            {
                if(sequence.second.cur_index >= sequence.second.images.size())
                {
                    if(sequence.second.cur_repeat >= sequence.second.repeat)
                    {
                        if(sequence.second.has_last == false) //must restart
                        {
                            sequence.second.cur_index = 0;
                            sequence.second.cur_repeat = 0;
                            glue(sequence.second.images[sequence.second.cur_index], screen_img, sequence.second.region);
                        }
                        else
                        {
                            glue(sequence.second.last_image,screen_img,sequence.second.region);
                        }
                    }
                    else
                    {
                        sequence.second.cur_index = 0;
                        sequence.second.cur_repeat += 1;
                        glue(sequence.second.images[sequence.second.cur_index], screen_img, sequence.second.region);
                    }
                }
                else
                {
                    glue(sequence.second.images[sequence.second.cur_index], screen_img, sequence.second.region);
                    sequence.second.cur_index+=1;
                }                
            }
            else //if it is not permanent, it is ondemand
            {
                bool go_for_it = false;
                std::vector<std::string>::iterator it;
                {
                    std::lock_guard<std::mutex> lock(mu);
                    it = std::find(OnDemandListName.begin(), OnDemandListName.end(),sequence.second.name);
                    go_for_it = (it != OnDemandListName.end());
                }
                if(go_for_it)
                {
                    std::cout<<"must go for it!"<<sequence.second.name.c_str()<<std::endl;
                    if(sequence.second.cur_index >= sequence.second.images.size())
                    {
                        if(sequence.second.cur_repeat >= sequence.second.repeat)
                        {
                            if(sequence.second.has_last == true)
                            {
                                //just show the last one
                                glue(sequence.second.last_image,screen_img,sequence.second.region);
                            }
                            else
                            {
                                sequence.second.cur_index = 0;
                                sequence.second.cur_repeat = 0;
                                {
                                    std::lock_guard<std::mutex> lock(mu);
                                    OnDemandListName.erase(it);
                                }
                            }
                        }
                        else
                        {
                            sequence.second.cur_index = 0;
                            sequence.second.cur_repeat += 1;
                            glue(sequence.second.images[sequence.second.cur_index], screen_img, sequence.second.region);
                        }
                    }
                    else
                    {
                        glue(sequence.second.images[sequence.second.cur_index], screen_img, sequence.second.region);
                        sequence.second.cur_index+=1;
                    }
                }
            }
        }
        
        cv::imshow("Display",screen_img);
        //display the image mixed
        char key = (char)cv::waitKey(step);
        
        std::string key_str{key};
        
        for(auto &sequence: c.animations)
        {
            
            if(!sequence.second.key.compare(key_str))
            {
                std::cout<<"found a key. schedule a job:"<<sequence.second.name.c_str()<<std::endl;
                OnDemand(sequence.second.name);
            }
        }
        
    }
}
Player::Player(std::string external_file)
{
    alive = false;
    config_file = external_file;
    load_config(config_file);
}
void Player::load_config(std::string config_file)
{
    if(c.parser(config_file))
    {
        std::cout<<"configuration loaded -- loading images"<<std::endl;
        for(auto &sequence: c.animations)
        {
            std::cout<<"sequence:"<<sequence.second.tostring()<<std::endl;
            if(sequence.second.x > c.app.window_x ||
                sequence.second.y > c.app.window_y ||
                sequence.second.x+sequence.second.w > c.app.window_x ||
                sequence.second.y+sequence.second.h > c.app.window_y)
                {
                    std::cout<<"error: wrong size."<<sequence.second.names.size()<<" images:"<<sequence.second.images.size()<<std::endl;
                    continue;                    
                }
            
            if(sequence.second.names.size() != sequence.second.images.size())
            {
                std::cout<<"error: wrong size names:"<<sequence.second.names.size()<<" images:"<<sequence.second.images.size()<<std::endl;
                continue;
            }
            for(unsigned i =0; i < sequence.second.names.size(); i++)
            {
                std::string f_name = sequence.second.names[i];
                std::cout<<"name of current file:"<<f_name<<std::endl;
                std::ifstream img_f(f_name);
                if(img_f.good())
                {
                    cv::Mat tmp =imread(f_name, cv::IMREAD_UNCHANGED);
                    cv::resize(tmp, tmp, cv::Size(sequence.second.w, sequence.second.h), cv::INTER_LINEAR);
                    tmp.copyTo(sequence.second.images[i]);
                }
                else
                {
                    std::cout<<"error: image not found "<<std::endl;
                    continue;
                }
            }
            if(sequence.second.has_last == true && !sequence.second.last_one.empty())
            {
                std::cout<<"load last image"<<sequence.second.last_one<<std::endl;
                std::ifstream img_f(sequence.second.last_one);
                if(img_f.good())
                {
                    cv::Mat tmp =imread(sequence.second.last_one, cv::IMREAD_UNCHANGED);
                    cv::resize(tmp, tmp, cv::Size(sequence.second.w, sequence.second.h), cv::INTER_LINEAR);
                    tmp.copyTo(sequence.second.last_image);
                }
            }
            else
            {
                std::cout<<"does not contains a last image"<<std::endl;
            }
        }
    }
    else
    {
        std::cout<<"configuration not ok. missing opportunity"<<std::endl;
    }
}
void Player::transparency(cv::Mat img)
{
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            cv::Vec4b & pixel = img.at<cv::Vec4b>(i, j);
            if(pixel[3] == 255)
            {
                pixel[0] =pixel[1] =pixel[2] = 127;
            }
            else
            {
                pixel[3] = 0;
            }
        }
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
    std::vector<cv::Mat> rgbLayer;
    cv::split(src, rgbLayer); // seperate channels
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            cv::Vec4b & pixel_src = src.at<cv::Vec4b>(i, j);
            cv::Vec3b & pixel_dst = dst.at<cv::Vec3b>(i + region.x, j + region.y);
            if(pixel_src[3] != 0)
            {
                pixel_dst[0] = pixel_src[0]*(1-1/pixel_src[3]) + pixel_dst[0]*(1/pixel_src[3]);
                pixel_dst[1] = pixel_src[1]*(1-1/pixel_src[3]) + pixel_dst[1]*(1/pixel_src[3]);
                pixel_dst[2] = pixel_src[2]*(1-1/pixel_src[3]) + pixel_dst[2]*(1/pixel_src[3]);
            }
        }
    }
}
