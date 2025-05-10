/*********************************************************************************************************************
* LS2K0300 Opensourec Library 即（LS2K0300 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是LS2K0300 开源库的一部分
*
* LS2K0300 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 适用平台          LS2K0300
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者           备注
* 2025-02-27        大W            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

#define MOTOR1_DIR   "/dev/zf_driver_gpio_motor_1"
#define MOTOR1_PWM   "/dev/zf_device_pwm_motor_1"

#define MOTOR2_DIR   "/dev/zf_driver_gpio_motor_2"
#define MOTOR2_PWM   "/dev/zf_device_pwm_motor_2"


struct pwm_info servo_pwm_info;


int8 duty = 0;
bool dir = true;

#define PWM_DUTY_MAX    (10000)         // 在设备树中，设置的10000。如果要修改，需要与设备树对应。

#define MAX_DUTY        (30 )           // 最大 MAX_DUTY% 占空比

void sigint_handler(int signum) 
{
    printf("收到Ctrl+C，程序即将退出\n");
    exit(0);
}

void cleanup()
{
    printf("程序异常退出，执行清理操作\n");
    // 关闭电机
    pwm_set_duty(MOTOR1_PWM, 0);   
    pwm_set_duty(MOTOR2_PWM, 0);    
}

int main(int, char**) 
{

    // 注册清理函数
    atexit(cleanup);

    // 注册SIGINT信号的处理函数
    signal(SIGINT, sigint_handler);

    while(1)
    {
        if(duty >= 0)                                                   // 正转
        {
            gpio_set_level(MOTOR1_DIR, 1);                              // DIR输出高电平
            pwm_set_duty(MOTOR1_PWM, duty * (PWM_DUTY_MAX / 100));      // 计算占空比

            gpio_set_level(MOTOR2_DIR, 1);                              // DIR输出高电平
            pwm_set_duty(MOTOR2_PWM, duty * (PWM_DUTY_MAX / 100));      // 计算占空比
        }
        else
        {
            gpio_set_level(MOTOR1_DIR, 0);                              // DIR输出低电平
            pwm_set_duty(MOTOR1_PWM, -duty * (PWM_DUTY_MAX / 100));     // 计算占空比

            gpio_set_level(MOTOR2_DIR, 0);                              // DIR输出低电平
            pwm_set_duty(MOTOR2_PWM, -duty * (PWM_DUTY_MAX / 100));     // 计算占空比

        }

        if(dir)                                                         // 根据方向判断计数方向 本例程仅作参考
        {
            duty ++;                                                    // 正向计数
            if(duty >= MAX_DUTY)                                        // 达到最大值
            dir = false;                                                // 变更计数方向
        }
        else
        {
            duty --;                                                    // 反向计数
            if(duty <= -MAX_DUTY)                                       // 达到最小值
            dir = true;                                                 // 变更计数方向
        }

        system_delay_ms(50);
    }
}