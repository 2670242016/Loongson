#pragma once

#include "AAAdefine.h"
#include "stdio.h"
#include "common_system.h"
#include "common_program.h"

static int16 limit_a_b(int16 x, int a, int b);

float Slope_Calculate(uint16 begin, uint16 end, uint16 *border);

void calculate_s_i(uint16 start, uint16 end, uint16 *border, float *slope_rate, float *intercept);

void cross_fill(uint8(*image)[image_w], uint16 *l_border, uint16 *r_border, uint16 total_num_l, uint16 total_num_r,
										 uint16 *dir_l, uint16 *dir_r, uint16(*points_l)[2], uint16(*points_r)[2]);
										 
void drawLine(uint8_t* img, cv::Point pt1, cv::Point pt2, uint8_t color = 0);