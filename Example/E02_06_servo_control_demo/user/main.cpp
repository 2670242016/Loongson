
#include "zf_common_headfile.h"


struct pwm_info servo_pwm_info;

// 定义驱动路劲，该路劲由设备树生成
#define SERVO_MOTOR1_PWM            "/dev/zf_device_pwm_servo"

// 定义主板上舵机频率  请务必注意范围 50-300
// 如果要修改，需要直接修改设备树。
#define SERVO_MOTOR_FREQ            (servo_pwm_info.freq)                       

// 在设备树中，默认设置的10000。如果要修改，需要直接修改设备树。
#define PWM_DUTY_MAX                (servo_pwm_info.duty_max)      

// 定义主板上舵机活动范围 角度                                                     
#define SERVO_MOTOR_L_MAX           (75 )                       
#define SERVO_MOTOR_R_MAX           (105)                       

// ------------------ 舵机占空比计算方式 ------------------
// 
// 舵机对应的 0-180 活动角度对应 控制脉冲的 0.5ms-2.5ms 高电平
// 
// 那么不同频率下的占空比计算方式就是
// PWM_DUTY_MAX/(1000/freq)*(1+Angle/180) 在 50hz 时就是 PWM_DUTY_MAX/(1000/50)*(1+Angle/180)
// 
// 那么 100hz 下 90度的打角 即高电平时间1.5ms 计算套用为
// PWM_DUTY_MAX/(1000/100)*(1+90/180) = PWM_DUTY_MAX/10*1.5
// 
// ------------------ 舵机占空比计算方式 ------------------
#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))


float servo_motor_duty = 90.0;                                                  // 舵机动作角度
float servo_motor_dir = 1;                                                      // 舵机动作状态

void sigint_handler(int signum) 
{
    printf("收到Ctrl+C，程序即将退出\n");
    exit(0);
}

void cleanup()
{
    printf("程序异常退出，执行清理操作\n");
    // 关闭电机
    pwm_set_duty(SERVO_MOTOR1_PWM, 0);   
}

int main(int, char**) 
{
    // 注册清理函数
    atexit(cleanup);

    // 注册SIGINT信号的处理函数
    signal(SIGINT, sigint_handler);

    // 获取PWM设备信息
    pwm_get_dev_info(SERVO_MOTOR1_PWM, &servo_pwm_info);

    // 打印PWM频率和duty最大值
    printf("servo pwm freq = %d Hz\r\n", servo_pwm_info.freq);
    printf("servo pwm duty_max = %d\r\n", servo_pwm_info.duty_max);


    while(1)
    {
        pwm_set_duty(SERVO_MOTOR1_PWM, (uint16)SERVO_MOTOR_DUTY(servo_motor_duty));


        if(servo_motor_dir)
        {
            servo_motor_duty ++;
            if(servo_motor_duty >= SERVO_MOTOR_R_MAX)
            {
                servo_motor_dir = 0x00;
            }
        }
        else
        {
            servo_motor_duty --;
            if(servo_motor_duty <= SERVO_MOTOR_L_MAX)
            {
                servo_motor_dir = 0x01;
            }
        }
        system_delay_ms(50);         
    }
}