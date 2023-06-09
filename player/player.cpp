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
    
    work = std::thread(&Player::work_func,this,c.app.window_x,c.app.window_y, c.app.step_ms);
}

void Player::Stop()
{
    alive =false;
    work.join();

}
void Player::work_func(int w, int h, int step){
    //create a black image of the desired size -- check size for OrangePi and RaspberryPi machines.
    
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
        cv::Mat screen_img(w,h,CV_8UC3, cv::Scalar(0,0,0) );        
        for(auto &sequence: c.sequences)
        { 
            
            
            if(sequence.second.type == permanent)
            {
                if(sequence.second.cur_index >= sequence.second.images.size())
                {
                    if(sequence.second.cur_repeat >= sequence.second.repeat)
                    {
                        if(sequence.second.last_one.empty())
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
                //lock mutex 
                bool go_for_it = false;
                std::vector<std::string>::iterator it;
                {
                    std::lock_guard<std::mutex> lock(mu);
                    it = std::find(OnDemandListName.begin(), OnDemandListName.end(),sequence.second.name);
                    go_for_it = (it != OnDemandListName.end());
                }
                

                if(go_for_it)
                {
                    std::cout<<"ondemand :"<<sequence.second.name<<std::endl;
                    if(sequence.second.cur_index >= sequence.second.images.size())
                    {
                        if(sequence.second.cur_repeat >= sequence.second.repeat)
                        {
                            if(sequence.second.last_one.empty())
                            {
                                sequence.second.cur_index = 0;
                                sequence.second.cur_repeat = 0;
                                {
                                    std::lock_guard<std::mutex> lock(mu);
                                    OnDemandListName.erase(it);
                                }
                            }
                            else
                            {
                                //just show the last one
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
                //
            }
        }
        std::cout<<"Display image"<<std::endl;
        cv::imshow("Display",screen_img);
        //display the image mixed
        cv::waitKey(step);
    }
}
Player::Player(std::string config_file)
{
    alive = false;
    if(c.parser(config_file))
    {
        std::cout<<"configuration loaded -- loading images"<<std::endl;
        for(auto &sequence: c.sequences)
        {
            
            std::cout<<"name:"<<sequence.second.name<<std::endl;
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
                    tmp.copyTo(sequence.second.images[i]);
                }
                else
                {
                    std::cout<<"error: image not found "<<std::endl;
                    continue;
                }
            }
            if(!sequence.second.last_one.empty())
            {
                std::cout<<"name of current file:"<<sequence.second.last_one<<std::endl;
                std::ifstream img_f(sequence.second.last_one);
                if(img_f.good())
                {
                    cv::Mat tmp =imread(sequence.second.last_one, cv::IMREAD_UNCHANGED);
                    tmp.copyTo(sequence.second.last_image);
                }
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

    cv::Mat mask;
    std::vector<cv::Mat> rgbLayer;
    
    cv::split(src, rgbLayer); // seperate channels

    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            cv::Vec4b & pixel_src = src.at<cv::Vec4b>(i, j);
            cv::Vec3b & pixel_dst = dst.at<cv::Vec3b>(i, j);
            
            if(pixel_src[3] != 0)
            {
                pixel_dst[0] = pixel_src[0]*(1-1/pixel_src[3]) + pixel_dst[0]*(1/pixel_src[3]);
                pixel_dst[1] = pixel_src[1]*(1-1/pixel_src[3]) + pixel_dst[1]*(1/pixel_src[3]);
                pixel_dst[2] = pixel_src[2]*(1-1/pixel_src[3]) + pixel_dst[2]*(1/pixel_src[3]);
            }
            else
            {
                pixel_dst[0] = pixel_src[0];
                pixel_dst[1] = pixel_src[1];
                pixel_dst[2] = pixel_src[2];
            }
        }
    }

}
