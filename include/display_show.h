#pragma once 

#include "AAAdefine.h"
#include "main.hpp"
#include "zf_device_ips200_fb.h"
#include "zf_common_font.h"
#include "zf_common_function.h"

/**
 * @brief 将OpenCV的Mat图像显示到IPS200屏幕上
 * @param img 输入的图像（支持BGR或灰度格式）
 * @note 图像将被自动缩放以适应屏幕分辨率（假设屏幕分辨率为IPS200_WIDTH x IPS200_HEIGHT）
 */
void displayMatOnIPS200(const cv::Mat& img);

void display_data(int y,const char dat[],int data,int num);

void display_dataf(int y,const char dat[],float data,int num1,int num2);
