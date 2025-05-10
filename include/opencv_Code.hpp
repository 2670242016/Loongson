#pragma once

#include "main.hpp"

uint8 original_image[image_h][image_w];
uint8 bin_image[image_h][image_w];

static int my_abs(int value);

static int16 limit_a_b(int16 x, int a, int b);

static uint8 otsuThreshold(uint8* image, uint16 col, uint16 row);

static void turn_to_bin(void);

static uint16 get_start_point(uint16 start_row);

static void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16* l_stastic, uint16* r_stastic, uint16 l_start_x, 
						uint16 l_start_y, uint16 r_start_x, uint16 r_start_y, uint16* hightest);

static void get_left(uint16 total_L);

static void get_right(uint16 total_R);

static void image_filter(uint8(*bin_image)[image_w]);

static void image_draw_rectan(uint8(*image)[image_w]);

static Mat Array_to_Mat(uint8(*image)[image_w]);

static void myImshow(const String& winname, InputArray mat);

static Mat er_zhi(Mat imgxiu);

static int mapDoubleToInt(double value, double fromLow, double fromHigh, int toLow, int toHigh);

cv::Mat resizeImage(const cv::Mat& inputImage);

static void fitLineAndCalculateAngle(const std::vector<cv::Point>& points, double& angle, double& rSquared);

void printArrayAsCSV(const uint16 a[],size_t size);

void filterData(uint16* arr, int size);

void findChangePoints(uint16* data, int size, struct ChangePoint** changePoints, int* changeCount, int threshold);

void cropImage(cv::Mat& image, int height);

void ni_tou();

vector<float> calculateCurvature(const vector<Point>& points);

float calculateServoAngle(const vector<float>& curvature, float baseAngle = 15.0f);

void servodegree(float degree);




