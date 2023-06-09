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
    private:

public:
    std::string name;

    TypeOfImage type;
    int cur_index;
    int cur_repeat;  

    int x, y, w, h, repeat;

    cv::Rect region;

    std::string last_one;
    cv::Mat last_image;

    std::vector<std::string> names;
    std::vector<cv::Mat> images;

    std::string tostring(){
        std::string ret;
        ret.append("name:");
        ret.append(name);
        ret.append(", last_one:");
        ret.append(last_one);
        return ret;
    };

    void clear(){
        name = "";
        type = invalid;
        x = -1;
        y = -1;
        w = -1;
        h = -1;
        last_one = "";
        names.clear();
        
    }
    PlayImage(const PlayImage &p):
                name{p.name},
                type{p.type},
                last_one{p.last_one},
                last_image{p.h,p.w,CV_8UC4,cv::Scalar(0,0,0,255)},
                x{p.x},
                y{p.y},
                w{p.w},
                h{p.h},
                names{p.names},
                region{p.region},
                cur_index{0},
                cur_repeat{0}
            {
        
        images.reserve(names.size());
        for (size_t i = 0; i < names.size(); i++) {
            // Uninitialized Mat of specified size, header constructed in place
            images.emplace_back(h, w,CV_8UC4, cv::Scalar(0,0,0,255));
        }
    };

    PlayImage() {};
};
class application
{
    public:
        int window_x, window_y, window_number, step_ms, ondemand_max;
        application():
                window_number{0},
                window_x{800},
                window_y{600},
                step_ms{100},
                ondemand_max{5}
        {

        };
};

class container
{
private:
    json raw_data;
    void glue(cv::Mat src, cv::Mat dst, cv::Rect region);
public:
    std::map<std::string,PlayImage> sequences;
    application app;
    bool parser(std::string filename);
};