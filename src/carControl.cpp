

#include "carControl.h"


/**
 * 状态机计算好道路的像素偏移和不同赛道的速度后通过以下代码将其转化为控制信号->舵机角度和电机PWM
 * 舵机角度：像素偏移量 -> 目标角速度 -> 舵机角度(PD:当前角速度，目标角速度)
 * 电机PWM：目标速度 + 编码器转速 -> 电机PWM(PD:编码器当前转速，编码器目标转速)     (最好转化为m/s)
 */



uint64_t get_timestamp_us() 
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// 将像素偏移量转化为角速度
double pixelToSpeed(int pixelOffset,JSON_PIDConfigData JSON_PIDConfigData_p) 
{   
    float x = float(pixelOffset) / 100.0;
    float speed = JSON_PIDConfigData_p.anglespeedpid.Kp*x*x;
    MaxMinf(&speed,JSON_PIDConfigData_p.anglespeedpid.Reslimit);
    return speed;
}

// 将角速度转化为舵机角度
double speedToServoAngle(double speed,JSON_PIDConfigData *JSON_PIDConfigData_p,PIDStatus *pidStatus,Data_Path *Data_Path_p) 
{
    pidStatus->target = speed;
    pidStatus->present = imu660ra_gyro_z/100.0;
    pidStatus->time_present = get_timestamp_us() / 1000000.0;
    
    PIDCalculate(&JSON_PIDConfigData_p->servopid, pidStatus);

    float servoAngle = (SERVO_MOTOR_L_MAX + SERVO_MOTOR_R_MAX) / 2.0 + Data_Path_p -> ServoDir*pidStatus->Res;

    if(servoAngle > SERVO_MOTOR_R_MAX) {
        servoAngle = SERVO_MOTOR_R_MAX;
    } else if(servoAngle < SERVO_MOTOR_L_MAX) {
        servoAngle = SERVO_MOTOR_L_MAX;
    }

    return servoAngle;
}

// 将目标速度转化为电机PWM
void speedToMotorPWM(double speed,JSON_PIDConfigData JSON_PIDConfigData_p) 
{

    motorleft.target = speed;
    motorright.target = speed;

    motorleft.present = encoder_left;
    motorright.present = encoder_right;
    
    motorleft.time_present = get_timestamp_us() / 1000000.0;
    motorright.time_present = get_timestamp_us() / 1000000.0;
    
    PIDCalculate(&JSON_PIDConfigData_p.servopid, &motorleft);
    PIDCalculate(&JSON_PIDConfigData_p.servopid, &motorright);

    motorleft.Res += speed;
    motorright.Res += speed;
    
}

// 将编码器转速转化为轮胎转速（m/s）
double encoderToSpeed(double encoder, double wheelRadius) 
{
    // 计算轮胎转速
    double speed; // 转化为m/s


    return speed;
}

void motorControl(int motor1_pwm, int motor2_pwm) 
{
    if(motor1_pwm < 0) {
        motor1_pwm = -motor1_pwm;
        gpio_set_level(MOTOR1_DIR, 0);                              // DIR输出低电平
    } else {
        gpio_set_level(MOTOR1_DIR, 1);                              // DIR输出高电平
    }
    if(motor2_pwm < 0) {
        motor2_pwm = -motor2_pwm;
        gpio_set_level(MOTOR2_DIR, 0);                              // DIR输出低电平
    } else {
        gpio_set_level(MOTOR2_DIR, 1);                              // DIR输出高电平
    }

    pwm_set_duty(MOTOR1_PWM, motor1_pwm * (PWM_DUTY_MAX / 100));      // 计算占空比
    pwm_set_duty(MOTOR2_PWM, motor2_pwm * (PWM_DUTY_MAX / 100));      // 计算占空比
}

