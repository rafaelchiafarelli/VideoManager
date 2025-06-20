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
 * The JSON file has many animations;
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
    std::string key;
    std::string order;
    TypeOfImage type;
    int cur_index;
    int cur_repeat;  

    int x, y, w, h, repeat;

    cv::Rect region;

    bool has_last;
    std::string last_one;
    std::string last_image;

    std::vector<std::string> names;
    std::vector<std::string> images;

    std::string tostring(){
        std::string ret = "{";
        ret.append("\"name\":\"");
        ret.append(name);
        ret.append("\", \"key\":\"");
        ret.append(key);        
        ret.append("\", \"order\":\"");
        ret.append(order);
        
        ret.append("\", \"repeat\":\"");
        ret.append(std::to_string(repeat));
        
        ret.append("\", \"pos\":{\"x\":");
        ret.append(std::to_string(x));
        ret.append(",\"y\":");
        ret.append(std::to_string(y));
        ret.append(",\"w\":");
        ret.append(std::to_string(w));
        ret.append(",\"h\":");
        ret.append(std::to_string(h));
        ret.append("}, \"names\":[");
        int name_count = 0;
        for(auto name : names)
        {
            ret.append("\"");
            ret.append(name);
            if(name_count < names.size()-1)
            {
                ret.append("\",");
            }
            else
            {
                ret.append("\"");
            }
            name_count+=1;
        }
        ret.append("], \"type\":");
        if(type == ondemand)
            ret.append("\"ondemand\"");
        else if(type == permanent)
            ret.append("\"permanent\"");
        else if(type == invalid)                
            ret.append("\"invalid\"");
        else 
            ret.append("\"not_allowed\"");

        ret.append(",\"last_one\":{\"has_last\":");
        if(has_last)
            {
                ret.append("true,\"file_name\":\"");
                ret.append(last_one);
                ret.append("\"");
            }
        else
        {
            ret.append("false,\"file_name\":\"\"");
        }
        ret.append("}}");
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
        has_last = false;

        cur_index = 0;
        cur_repeat = 0;
        names.clear();
        
    }
    PlayImage(const PlayImage &p):
                name{p.name},
                order(p.order),
                key(p.key),
                type{p.type},
                last_one{p.last_one},
                last_image{""},
                x{p.x},
                y{p.y},
                w{p.w},
                h{p.h},
                names{p.names},
                region{p.region},
                repeat{p.repeat},
                cur_index{0},
                cur_repeat{0},
                has_last{p.has_last}
            {
        
        images.reserve(names.size());
        for (size_t i = 0; i < names.size(); i++) {
            // Uninitialized Mat of specified size, header constructed in place
            images.emplace_back("");
        }
    };

    PlayImage():name{""},
                key{""},
                order{""},
                cur_index{0},
                cur_repeat{0},
                type{invalid},
                x{0},
                y{0},
                w{0},
                h{0},
                repeat{0},
                region{cv::Rect(0,0,0,0)},
                has_last{false},
                last_one{""}{};
    ~PlayImage(){
        names.clear();
        images.clear();
        
    }
};
class application
{
    public:
        std::string IP;
        std::string character;
        std::string type;
        std::vector<std::string> endpoints;
        int window_x, window_y, window_number, step_ms, ondemand_max, PORT;
        application():
                IP(""),
                character(""),
                PORT(80),
                type(""),
                window_number{0},
                window_x{800},
                window_y{600},
                step_ms{100},
                ondemand_max{5}
        {

        };
        std::string tostring()
        {
            std::string ret;
            ret.append("\"window\":[");
            ret.append(std::to_string(window_x));
            ret.append(",");
            ret.append(std::to_string(window_y));
            ret.append("],\"step_ms\":");
            ret.append(std::to_string(step_ms));        
            ret.append(", \"ondemand_max\":");
            ret.append(std::to_string(ondemand_max));
            ret.append(", \"character\":\"");
            ret.append(character);            
            ret.append("\", \"IP\":\"");
            ret.append(IP);
            ret.append("\", \"PORT\":");
            ret.append(std::to_string(PORT));
            ret.append(",\"type\":\"");
            ret.append(type);
            ret.append("\",\"endpoints\":[");
            int endpoint_counter = 0;
            for(auto endpoint: endpoints)
            {
                ret.append("\"");
                ret.append(endpoint);
                if(endpoint_counter< endpoints.size()-1)
                {
                    ret.append("\",");
                }
                else
                {
                    ret.append("\"");
                }
                endpoint_counter+=1;
            }
            ret.append("]");
            return ret;
        }
};

class container
{
private:
    json raw_data;
    void glue(cv::Mat src, cv::Mat dst, cv::Rect region);
public:
    std::map<std::string,PlayImage> animations;
    application app;
    std::string tostring()
    {
        std::string ret = "{";
        ret.append(app.tostring());
        ret.append(",\"animations\":[");
        int seq_count = 0;
        for(auto sequence : animations)
        {
            ret.append(sequence.second.tostring());
            if(seq_count<animations.size()-1)
            {
                ret.append(",");
            }
            seq_count+=1;
        }
        ret.append("]}");
        return ret;
    }
    bool parser(std::string filename);
};
