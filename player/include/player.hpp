#pragma once
#include <string.h>
#include <string>
#include "stdio.h"
#include <vector>
#include "opencv2/opencv.h"

class Player
{
private:
    void glue(cv::Mat src, cv::Mat dst, cv::Rect region);
    std::set<std::string, PlayImage> SequenceMap;
    std::vector<std::string> Sequence;

public:
    void OnDemand(std::string name)
    {
        /* play on demand */
        OnDemandListName.push_back(name);
    }
};