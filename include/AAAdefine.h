#pragma once

#define image_w	320	        //图像宽度
#define img_resize_h    60
#define image_h	(240-img_resize_h)	//图像高度

#define white_pixel	255
#define black_pixel	0

#define bin_jump_num	1//跳过的点数
#define border_max	image_w-3 //边界最大值
#define border_min	2	//边界最小值	

#define USE_num	image_h*4

#define threshold_max	255*5//此参数可根据自己的需求调节
#define threshold_min	255*2//此参数可根据自己的需求调节

typedef   signed          char int8;
typedef   signed short     int int16;
typedef   signed           int int32;
typedef unsigned          char uint8;
typedef unsigned short     int uint16;
typedef unsigned           int uint32;

#define CAMERA_H  image_h
#define CAMERA_W  image_w
#define OUT_H  image_h
#define OUT_W  image_w

#define RESULT_ROW  180//结果图行列
#define RESULT_COL  320


