#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <math.h>		
#include <stdlib.h>
#include "base.h"
#include "common_system.h"
#include "common_program.h"
#include "register.h"
#include "stdio.h"
#include "zf_common_headfile.h"

#include "libimage_process.h"
#include "libdata_process.h"
#include "libdata_store.h"

#include "PID.h"
#include "AAAdefine.h"

#include "myacross.h"
#include "carControl.h"
#include "display_show.h"




struct ChangePoint {
    int startIndex;  // 突变起始索引
    int endIndex;    // 突变结束索引
    int type;        // 1 表示上升沿，-1 表示下降沿
};


extern uint8 image_final[OUT_H][OUT_W];             //逆变换图像
extern double map_square[CAMERA_H][CAMERA_W][2];    //现实映射
extern int map_int[OUT_H][OUT_W][2];                //图像映射


extern uint8 image_thereshold;              //图像分割阈值

extern uint8 bin_image[image_h][image_w];   //图像数组

extern uint16 start_point_l[2];             //左边起点的x，y值
extern uint16 start_point_r[2];             //右边起点的x，y值
extern uint16 points_l[(uint16)USE_num][2]; //左线
extern uint16 points_r[(uint16)USE_num][2]; //右线
extern uint16 dir_r[(uint16)USE_num];       //用来存储右边生长方向
extern uint16 dir_l[(uint16)USE_num];       //用来存储左边生长方向

extern uint16 data_stastics_l;              //统计左边找到点的个数
extern uint16 data_stastics_r;              //统计右边找到点的个数
extern uint16 hightest;                     //最高点

extern uint16 l_border[image_h];            //左线数组
extern uint16 r_border[image_h];            //右线数组
extern uint16 center_line[image_h];         //中线数组

extern Point point;

#define SERVO_MOTOR1_PWM        "/dev/zf_device_pwm_servo"

#define MOTOR1_DIR              "/dev/zf_driver_gpio_motor_1"
#define MOTOR1_PWM              "/dev/zf_device_pwm_motor_1"

#define MOTOR2_DIR              "/dev/zf_driver_gpio_motor_2"
#define MOTOR2_PWM              "/dev/zf_device_pwm_motor_2"

#define ENCODER_1               "/dev/zf_encoder_1"
#define ENCODER_2               "/dev/zf_encoder_2"


#define KEY_0       "/dev/zf_driver_gpio_key_0"
#define KEY_1       "/dev/zf_driver_gpio_key_1"
#define KEY_2       "/dev/zf_driver_gpio_key_2"
#define KEY_3       "/dev/zf_driver_gpio_key_3"
#define SWITCH_0    "/dev/zf_driver_gpio_switch_0"
#define SWITCH_1    "/dev/zf_driver_gpio_switch_1"


// 定义主板上舵机频率  请务必注意范围 50-300
// 如果要修改，需要直接修改设备树。
#define SERVO_MOTOR_FREQ            (servo_pwm_info.freq)                       

// 在设备树中，默认设置的10000。如果要修改，需要直接修改设备树。
#define PWM_DUTY_MAX                (servo_pwm_info.duty_max)      

// 定义主板上舵机活动范围 角度                                                     
#define SERVO_MOTOR_L_MAX           (73 )                       
#define SERVO_MOTOR_R_MAX           (107)                       

#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))

#define MAX_DUTY        (30 )           // 最大 MAX_DUTY% 占空比

struct Motorinfo
{
    int leftspeednow;
    int leftspeedbefore;
    int leftspeedtarget;

    int rightspeednow;
    int rightspeedbefore;
    int rightspeedtarget;

};

extern Motorinfo motorinfo;

extern pwm_info servo_pwm_info;

extern int encoder_left;
extern int encoder_right;

void pit_callback(void);
void sigint_handler(int signum);
void cleanup();

