#pragma once
#include <string.h>
#include <string>
#include "stdio.h"
#include <vector>
#include <map>
#include "json.hpp"
#include "opencv2/opencv.hpp"

/**
 * This file plays the videos as they are presented at the configuration file.
 * The configuration file is a JSON that contains witch sequence is played at each space.
 * A "sequence" is a sequence of PNG with alfa channel (no background) that can be added to the image to be displayed.
 * A sequence has a place (X,Y), a size (W,H), a set of file names and finaly a last item to be displayed.
 * for example:
 * {
 * name:"sequence_one",
 * type:"permanent",
 * pos:{x:0, y:0. w:1080, h:1920},
 * names:[
 * "/home/raspberrypi/background/sleep_1.png",
 * "/home/raspberrypi/background/sleep_2.png",
 * "/home/raspberrypi/background/sleep_3.png",
 * "/home/raspberrypi/background/sleep_4.png",
 * "/home/raspberrypi/background/sleep_5.png",
 * ],
 * last_one:""/home/raspberrypi/background/sleep_over.png""
 * }
 *
 * The business logic:
 * When there is no "last_one", the animation should restart.
 * If it is missing any of the other, the sequence must be ignored.
 * The "type" can be "permanent", "ondemand".
 *  - "ondemand" means it will show only when it is required
 *  - "permanent" means it will show at the begining and it will not end.
 *  - "????"
 * The JSON file has many sequences;
 *
 * Once the configuration is parsed, all files should be loaded into memory.
 * The player will start the play as intented, the "permanent" first and then the ondemand ones.
 *
 *
 *
 *
 * */
using json = nlohmann::json;

typedef enum
{
    ondemand,
    permanent,
    invalid
} TypeOfImage;

class PlayImage
{
public:
    std::string name;
    TypeOfImage type;
    int x, y, w, h;
    std::string last_one;
    cv::Mat last_image;

    std::vector<std::string> names;
    std::vector<cv::Mat> images;

    int cur_index;
    void clear(){
        name = "";
        type = invalid;
        x = -1;
        y = -1;
        w = -1;
        h = -1;
        last_one = "";
        last_image.release();
        names.clear();
        
    }
    PlayImage(const PlayImage &p):
                type{p.type},
                last_one{p.last_one},
                last_image{p.h,p.w,CV_8UC4,cv::Scalar(0,0,0,255)},
                x{p.x},
                y{p.y},
                w{p.w},
                h{p.h},
                names{p.names},
                cur_index{0}
            {
        images.reserve(names.size());
        for (size_t i = 0; i < names.size(); i++) {
            // Uninitialized Mat of specified size, header constructed in place
            images.emplace_back(h, w,CV_8UC4, cv::Scalar(0,0,0,255));
        }
    };

    PlayImage(TypeOfImage  t,
            int _x, 
            int _y,
            int _w,
            int _h,
            std::vector<std::string> _names, 
            std::string _last_one):
                type{t},
                last_one{_last_one},
                last_image{h,w,CV_8UC4,cv::Scalar(0,0,0,255)},
                x{_x},
                y{_y},
                w{_w},
                h{_h},
                names{_names},
                cur_index{0}
            {
        images.reserve(names.size());
        for (size_t i = 0; i < names.size(); i++) {
            // Uninitialized Mat of specified size, header constructed in place
            images.emplace_back(h, w,CV_8UC4, cv::Scalar(0,0,0,255));
        }
    };
    PlayImage() {};
};

class container
{
private:
    json raw_data;
public:
    std::map<std::string,PlayImage> sequences;
    bool parser(std::string filename);
};