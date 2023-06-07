#include "player.hpp"
#include <stdio.h>
#include <iostream>

#include "opencv2/opencv.hpp"

void Player::glue(cv::Mat src, cv::Mat dst, cv::Rect region)
{
    cv::Mat mask;
    std::vector<Mat> rgbLayer;
    cv::split(src, rgbLayer); // seperate channels
    Mat cs[3] = {rgbLayer[0], rgbLayer[1], rgbLayer[2]};
    cv::merge(cs, 3, src); // glue together again
    mask = rgbLayer[3];    // png's alpha channel used as mask
    src.copyTo(dst(region), mask);
}

void Player::