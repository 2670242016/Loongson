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

struct pwm_info pwm_1_info;
struct pwm_info pwm_2_info;

#define PWM_1           "/dev/zf_device_pwm_esc_1"
#define PWM_2           "/dev/zf_device_pwm_esc_2"

uint16 duty = 0;

void sigint_handler(int signum) 
{
    printf("收到Ctrl+C，程序即将退出\n");
    exit(0);
}

void cleanup()
{
    printf("程序异常退出，执行清理操作\n");
    // 关闭电机
    pwm_set_duty(PWM_1, 0);   
    pwm_set_duty(PWM_2, 0);    
}

int main(int, char**) 
{

    // 注册清理函数
    atexit(cleanup);

    // 注册SIGINT信号的处理函数
    signal(SIGINT, sigint_handler);

    // 获取PWM设备信息
    pwm_get_dev_info(PWM_1, &pwm_1_info);
    pwm_get_dev_info(PWM_2, &pwm_2_info);

    // 打印PWM频率和duty最大值
    printf("pwm 1 freq = %d Hz\r\n", pwm_1_info.freq);
    printf("pwm 1 duty_max = %d\r\n", pwm_1_info.duty_max);

    printf("pwm 2 freq = %d Hz\r\n", pwm_2_info.freq);
    printf("pwm 2 duty_max = %d\r\n", pwm_2_info.duty_max);


    while(1)
    {
        // 此处编写需要循环执行的代码

        // 50HZ的周期为20ms

        // 计算无刷电调转速   （1ms - 2ms）/20ms * 10000（10000是PWM的满占空比时候的值）
        // 无刷电调转速 0%   为 500
        // 无刷电调转速 20%  为 600
        // 无刷电调转速 40%  为 700
        // 无刷电调转速 60%  为 800
        // 无刷电调转速 80%  为 900
        // 无刷电调转速 100% 为 1000

        // 修改duty的值，可以修改无刷电调转速

        duty++;
        if(800 < duty)
        {
            duty = 500;
        }

        pwm_set_duty(PWM_1, duty);
        pwm_set_duty(PWM_2, 600);
  
        system_delay_ms(10);
    }
}