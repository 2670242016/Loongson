#pragma once
/**
 * @file    carControl.h
 * @brief   智能车控制模块
 * @author  崔锦翔(安徽师范大学计信学院 物联网工程 24届)
 * @date    2025-04-23
 * @version 1.0
 * 
 * @license MIT License
 *          Copyright (c) 2024 崔锦翔
 *          智能车比赛极速光电-龙芯组别专用代码
 * 
 *          仅供学习交流使用，禁止用于商业用途
 */

#include "AAAdefine.h"
#include "main.hpp"
#include <chrono>


using Clock = std::chrono::steady_clock;



/**
 * @brief   获取当前时间戳(微秒级)
 * @return  返回时间戳(ms)
 */
uint64_t get_timestamp_us() ;

/**
 * @brief   将像素偏移计算为目标角速度
 * @param   [in] pixelOffset  像素偏移量
 * @param   [in] JSON_PIDConfigData_p JSON文件储存的pid参数
 * @return  目标角速度
 */
double pixelToSpeed(int pixelOffset,JSON_PIDConfigData JSON_PIDConfigData_p) ;

/**
 * @brief   将目标角速度转化为舵机角度
 * @param   [in] pixelOffset  像素偏移量
 * @param   [in] JSON_PIDConfigData_p JSON文件储存的pid参数
 * @return  目标角度
 */
double speedToServoAngle(double speed,JSON_PIDConfigData *JSON_PIDConfigData_p,PIDStatus *pidStatus,Data_Path *Data_Path_p) ;

/**
 * @brief   将目标速度转化为电机PWM
 * @param   [in] speed  目标速度
 * @param   [in] JSON_PIDConfigData_p JSON文件储存的pid参数
 * @param   [out] motorleft  电机PWM
 * @param   [out] motorright  电机PWM
 */
void speedToMotorPWM(double speed,JSON_PIDConfigData JSON_PIDConfigData_p) ;

void motorControl(int motor1_pwm, int motor2_pwm);

