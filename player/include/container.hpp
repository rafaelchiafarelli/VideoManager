#pragma once
#include <string.h>
#include <string>
#include "stdio.h"
#include <vector>
#include "opencv2/opencv.h"

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

typedef enum
{
    ondemand,
    permanent
} TypeOfImage;

class PlayImage
{
public:
    //    std::string name;
    TypeOfImage type;
    int x, y, w, h;
    std::vector<std::string> names;
    std::string last_one;
    cv::Mat last_image;
    std::vector<cv::Mat> images;
};

class container
{
private:
public:
    std::vector<PlayImage> sequence;
    void parse(std::string filename);
};