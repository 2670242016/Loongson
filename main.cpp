

#define EXAMPLE_CODE 0
#define CARCODE 1

#include "main.hpp"
#include "AAAdefine.h"
// #include "opencv_Code.hpp"

using namespace std;
using namespace cv;

double change_un_Mat[3][3] ={{0.791424,-0.913294,59.608567},{0.008867,0.162653,-0.663674},{0.000089,-0.005162,1.045651}};


JSON_PIDConfigData  JSON_PIDConfigData_c;
JSON_PIDConfigData  *JSON_PIDConfigData_p = &JSON_PIDConfigData_c;

Function_EN         Function_EN_c;
Function_EN         *Function_EN_p = &Function_EN_c;

Data_Path           Data_Path_c;
Data_Path           *Data_Path_p = &Data_Path_c;

Img_Store           Img_Store_c; 
Img_Store           *Img_Store_p = &Img_Store_c;

ImgProcess imgProcess;
Judge judge;
SYNC Sync;

int encoder_Left;
int encoder_Right;
struct pwm_info servo_pwm_info;


int servo_angle;

float motorl,motorr;
PIDStatus statusmotorl,statusmotorr;


#if EXAMPLE_CODE == 0





int main(int argc, char *argv[])
{
    
    // int temp_speedl = atoi(argv[1]);
    // int temp_speedr = atoi(argv[2]);
    // printf("Motor1:%d\tMotor2:%d\r\n",temp_speedl,temp_speedr);


    // ImagePerspective_Init(Img_Store_p,change_un_Mat);
    Sync.ConfigData_SYNC(Data_Path_p,Function_EN_p,JSON_PIDConfigData_p);

    JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.motorpid.Kp,JSON_PIDConfigData_c.motorpid.Ki,JSON_PIDConfigData_c.motorpid.Kd);    
    printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.servopid.Kp,JSON_PIDConfigData_c.servopid.Ki,JSON_PIDConfigData_c.servopid.Kd);
    printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.anglespeedpid.Kp,JSON_PIDConfigData_c.anglespeedpid.Ki,JSON_PIDConfigData_c.anglespeedpid.Kd);
    printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.pixelpid.Kp,JSON_PIDConfigData_c.pixelpid.Ki,JSON_PIDConfigData_c.pixelpid.Kd);
    
    VideoCapture Camera;
    CameraInit(Camera,JSON_FunctionConfigData.Camera_EN,120);

    // Camera.set(cv::CAP_PROP_AUTO_EXPOSURE, JSON_FunctionConfigData.exposure_auto); // 设置自动曝光
    // bool success = Camera.set(cv::CAP_PROP_EXPOSURE, JSON_FunctionConfigData.cap_exposure);
    // if (!success) {
    //     std::cerr << "无法设置曝光参数：" << JSON_FunctionConfigData.cap_exposure << std::endl;
    // }
    // thread CameraImgCapture (CameraImgGetThread,ref(Camera),ref(Img_Store_p));
    // CameraImgCapture.detach();

    Function_EN_p -> Game_EN = true;
    Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP;

    sleep(1);

    #if CARCODE == 1
    pwm_get_dev_info(SERVO_MOTOR1_PWM, &servo_pwm_info);
    pit_ms_init(10, pit_callback);
    atexit(cleanup);
    signal(SIGINT, sigint_handler);

    ips200_init("/dev/fb0");
    ips200_full(RGB565_WHITE);
    #endif

    double delta[4];

    while(Function_EN_p -> Game_EN == true)
    {   
        delta[0] = get_timestamp_us() / 1000.0;

        if (gpio_get_level(KEY_3) == 0)
        {
            Sync.ConfigData_SYNC(Data_Path_p,Function_EN_p,JSON_PIDConfigData_p);
            JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
            JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
            printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.motorpid.Kp,JSON_PIDConfigData_c.motorpid.Ki,JSON_PIDConfigData_c.motorpid.Kd);    
            printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.servopid.Kp,JSON_PIDConfigData_c.servopid.Ki,JSON_PIDConfigData_c.servopid.Kd);
            printf("P:%.2f,\tI:%.2f,\tD:%.2f\n",JSON_PIDConfigData_c.anglespeedpid.Kp,JSON_PIDConfigData_c.anglespeedpid.Ki,JSON_PIDConfigData_c.anglespeedpid.Kd);
        
        }

        // 图像主循环
        while( Function_EN_p -> Loop_Kind_EN == CAMERA_CATCH_LOOP)
        {
            // 前瞻点初始化
            Data_Path_p -> JSON_TrackConfigData_v[0].Forward = Data_Path_p -> JSON_TrackConfigData_v[0].Default_Forward;

            // CameraImgGet(Img_Store_p);
            delta[1] = get_timestamp_us() / 1000.0;
            Camera >> Img_Store_p -> Img_Color;
            delta[2] = get_timestamp_us() / 1000.0;

            Img_Store_p->Img_Color = Img_Store_p->Img_Color(Range(img_resize_h,240),Range(0,320));

            imgProcess.imgPreProc(Img_Store_p,Data_Path_p,Function_EN_p); // 图像预处理

            memcpy(Img_Store_p->bin_image[0], Img_Store_p->Img_OTSU.data, image_h * image_w * sizeof(uint8));
            imgSearch_l_r(Img_Store_p,Data_Path_p);   // 边线八邻域寻线
            
            // ApplyInversePerspectiveAll(Img_Store_p);

            Img_Store_p -> ImgNum++;

            Function_EN_p -> Loop_Kind_EN = JUDGE_LOOP; 
            // cout << "CAMERA_CATCH_LOOP" << endl;
            break;
        }

        // 赛道状态机决策循环
        while( Function_EN_p -> Loop_Kind_EN == JUDGE_LOOP )
        {
            Function_EN_p -> Loop_Kind_EN = judge.TrackKind_Judge(Img_Store_p,Data_Path_p,Function_EN_p);  // 切换至赛道循环
            // cout << "JUDGE_LOOP" << endl;
        }

        // 普通赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == COMMON_TRACK_LOOP )
        {
            // if(Data_Path_p -> Circle_Track_Step == IN_PREPARE)
            // {
            //     CircleTrack_Step_IN_Prepare_Stright(Img_Store_p,Data_Path_p);   // 准备入环补线
            // }
            // if(Data_Path_p -> Circle_Track_Step == OUT_2_STRIGHT)
            // {
            //     Circle2CommonTrack(Img_Store_p,Data_Path_p);    // 出环转直线补线
            // }
            // ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            imgSearch_l_r(Img_Store_p,Data_Path_p);

            judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算x
            judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环

            // cout << "COMMON_TRACK_LOOP" << endl;
        }

        // 左右圆环赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == L_CIRCLE_TRACK_LOOP || Function_EN_p -> Loop_Kind_EN == R_CIRCLE_TRACK_LOOP )
        {
            switch(Data_Path_p -> Circle_Track_Step)
            {
                case IN_PREPARE:
                {
                    CircleTrack_Step_IN_Prepare(Img_Store_p,Data_Path_p);   // 准备入环补线
                    break;
                }
                case IN:
                {
                    CircleTrack_Step_IN(Img_Store_p,Data_Path_p);   // 入环补线
                    break;
                }
                case OUT:
                {
                    CircleTrack_Step_OUT(Img_Store_p,Data_Path_p);   // 出环补线
                    break;
                }
            }
            // ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            imgSearch_l_r(Img_Store_p,Data_Path_p);
            judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算
            judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环
            // cout << "CIRCLE_TRACK_L" << endl;
        
        }

        // 十字赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == ACROSS_TRACK_LOOP )
        {
            AcrossTrack(Img_Store_p,Data_Path_p);   // 十字赛道补线

            // cross_fill(Img_Store_p->bin_image, Data_Path_p->l_border, Data_Path_p->r_border, Data_Path_p->NumSearch[0], Data_Path_p->NumSearch[1],
            // Data_Path_p->dir_l, Data_Path_p->dir_r, Data_Path_p->points_l, Data_Path_p->points_r);
            // ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            imgSearch_l_r(Img_Store_p,Data_Path_p);
            
            judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算
            judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环
            // cout << "ACROSS_TRACK_LOOP" << endl ;
        }
        
        // printf("Speed:%d\tAngle:%d\n\r",Data_Path_p->MotorSpeed,Data_Path_p->ServoAngle);

        
        #if CARCODE == 1

        /*****************************************************************************************************
         * ***********************************      转向PID     **********************************************
         * ***************************************************************************************************/
        PIDStatus pixelStatus;
        pixelPidMath(Data_Path_p->ServoAngle,JSON_PIDConfigData_c,&pixelStatus);
        servo_angle = pixelStatus.Res;
        
        // PIDStatus pidStatus;
        // float anglespeedtarget = pixelToSpeed(Data_Path_p->ServoAngle,JSON_PIDConfigData_c);
        // servo_angle = speedToServoAngle(anglespeedtarget,JSON_PIDConfigData_p,&pidStatus,Data_Path_p);

        // printf("%f\t,%f\t,%f\t,%f\t,%f\t,%f\r\n",
        //     pidStatus.Res,JSON_PIDConfigData_p->servopid.P,JSON_PIDConfigData_p->servopid.D,pidStatus.departure,anglespeedtarget,pidStatus.present);
        
        // pwm_set_duty(SERVO_MOTOR1_PWM, (uint16)SERVO_MOTOR_DUTY(servo_angle));
        pwm_set_duty(SERVO_MOTOR1_PWM, (uint16)SERVO_MOTOR_DUTY((SERVO_MOTOR_L_MAX+SERVO_MOTOR_R_MAX)/2.0+Data_Path_p->ServoDir*servo_angle));
        


        /*****************************************************************************************************
         * ***********************************      电机PID     **********************************************
         * ***************************************************************************************************/
        // motorControl(temp_speedl,temp_speedr);

        // printf("%d,%d\r\n", encoder_Left,encoder_Right);
        motorl = speedToMotorPWM(JSON_PIDConfigData_p->speedl,encoder_Left,JSON_PIDConfigData_c,&statusmotorl);
        motorr = speedToMotorPWM(JSON_PIDConfigData_p->speedr,encoder_Right,JSON_PIDConfigData_c,&statusmotorr);
        motorControl(motorr,motorl);

        printf("%.2f,%.2f,%.2f,%.2f\r\n",statusmotorl.target,statusmotorr.target,statusmotorl.present,statusmotorr.present);














        for (int i = Data_Path_p->hightest; i < image_h-JSON_TrackConfigData.Path_Search_Start; i++)
        {
            Data_Path_p->center_line[i] = (Data_Path_p->l_border[i] + Data_Path_p->r_border[i]) >> 1;//求中线
        }
        
        if (JSON_FunctionConfigData.VideoShow_EN == true)
        {
            imgProcess.ImgLabel(Img_Store_p,Data_Path_p,Function_EN_p);
            imgProcess.ImgInflectionPointDraw(Img_Store_p,Data_Path_p); 
            imgProcess.ImgBendPointDraw(Img_Store_p,Data_Path_p); 
            imgProcess.ImgReferenceLine(Img_Store_p,Data_Path_p);
            if(JSON_FunctionConfigData.imgshownum == 0)
            {
                displayMatOnIPS200(Img_Store_p->Img_Track); 
            }
            else if(JSON_FunctionConfigData.imgshownum == 1)
            {
                Mat resizedImg;
                resize(Img_Store_p->Img_OTSU, resizedImg, cv::Size(240, 180));
                uint8_t* image_otsu = new uint8_t[180 * 240];
                const uint8_t* image_ptr = resizedImg.data;
                for (int i = 0; i < 180; ++i) 
                {
                    for (int j = 0; j < 240; ++j) 
                    {
                        image_otsu[i * 240 + j] = image_ptr[i * resizedImg.step + j];
                    }
                }
                ips200_show_gray_image(0,0,image_ptr,240,180);
            }
            // ips200_show_string(0,16*12,("TrackKind:" + imgProcess.TextTrackKind[int(Data_Path_p -> Track_Kind)]).c_str());
            // ips200_show_string(0,16*13,("CircleTrack:" + imgProcess.TextCircleTrackStep[int(Data_Path_p -> Circle_Track_Step)]).c_str());
            // ips200_show_string(0,16*14,("Gyroscope_EN:" + imgProcess.TextGyroscope[int(Function_EN_p -> Gyroscope_EN)]).c_str());
            // ips200_show_string(0,16*15,("Control_EN:" + imgProcess.TextControl[int(Function_EN_p -> Control_EN)]).c_str());


            // ips200_show_string(0,16*12,"GZ:");ips200_show_int(8*3,16*16,int32(imu660ra_gyro_z),5);
            // ips200_show_string(0,16*13,"Offset:");ips200_show_int(8*7,16*17,int32(Data_Path_p->ServoAngle),3);

            display_data(12,"GZ:",imu660ra_gyro_z,5);
            display_data(13,"Offset:",Data_Path_p->ServoAngle,3);
            display_dataf(14,"angSpeed:",servo_angle,4,8);
        }
        

        delta[3] = get_timestamp_us() / 1000.0;
        // printf("img_ms:%.2f\tprocess_ms:%.2f\tall_ms:%.2f\n\r",delta[2]-delta[1],delta[3]-delta[2],delta[3]-delta[0]);
        
        // speedToMotorPWM(Data_Path_p->MotorSpeed,JSON_PIDConfigData_c);
        // printf("SpeedL:%d\tSpeedR:%d\tAngle:%d\n\r",motorleft.Res,motorright.Res,servo_angle);
        // SERVO_MOTOR_DUTY(servo_angle);
        #endif


        #if CARCODE == 0
        waitKey(0);
        imgProcess.ImgShow(Img_Store_p,Data_Path_p,Function_EN_p);
        #endif
        
        // DataPrint(Data_Path_p,Function_EN_p);
        

    }
    
    return 0;

}

void pit_callback(void)
{
    imu660ra_get_acc();
    imu660ra_get_gyro();
    encoder_Left  = encoder_get_count(ENCODER_1);
    encoder_Right = -encoder_get_count(ENCODER_2);
}

void sigint_handler(int signum) 
{
    printf("收到Ctrl+C，程序即将退出\n");
    pwm_set_duty(MOTOR1_PWM, 0);   
    pwm_set_duty(MOTOR2_PWM, 0);   
    exit(0);
}

void cleanup()
{
    printf("程序异常退出，执行清理操作\n");
    // 关闭电机
    pwm_set_duty(MOTOR1_PWM, 0);   
    pwm_set_duty(MOTOR2_PWM, 0);    
}
#endif
#if EXAMPLE_CODE == 1

void pit_callback()
{
    imu660ra_get_acc();
    imu660ra_get_gyro();
}

int main(int, char**) 
{

    imu_get_dev_info();
    if(DEV_IMU660RA == imu_type)
    {
        printf("IMU DEV IS IMU660RA\r\n");
    }
    else if(DEV_IMU660RB == imu_type)
    {
        printf("IMU DEV IS IMU660RB\r\n");
    }
    else if(DEV_IMU963RA == imu_type)
    {
        printf("IMU DEV IS IMU963RA\r\n");
    }
    else
    {
        printf("NO FIND IMU DEV\r\n");
        return -1;
    }
    
    // 创建一个定时器10ms周期，回调函数为pit_callback
    pit_ms_init(10, pit_callback);

    while(1)
    {
        // printf("imu660ra_acc_x  = %d\r\n", imu660ra_acc_x);
        // printf("imu660ra_acc_y  = %d\r\n", imu660ra_acc_y);
        // printf("imu660ra_acc_z  = %d\r\n", imu660ra_acc_z);

        // printf("imu660ra_gyro_x = %d\r\n", imu660ra_gyro_x);
        // printf("imu660ra_gyro_y = %d\r\n", imu660ra_gyro_y);
        // printf("imu660ra_gyro_z = %d\r\n", imu660ra_gyro_z);

        printf("%d,%d,%d,%d,%d,%d\r\n",imu660ra_acc_x,imu660ra_acc_y,imu660ra_acc_z,
            imu660ra_gyro_x,imu660ra_gyro_y,imu660ra_gyro_z);
        system_delay_ms(10);
    }
}
#endif

#if EXAMPLE_CODE == 2

#include <cstdlib> // for atoi()

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

int main(int argc, char *argv[]) 
{
    // 注册清理函数
    atexit(cleanup);

    // 注册SIGINT信号的处理函数
    signal(SIGINT, sigint_handler);

    // 获取PWM设备信息
    pwm_get_dev_info(SERVO_MOTOR1_PWM, &servo_pwm_info);

    int angle = atoi(argv[1]);

    while(1)
    {
        pwm_set_duty(SERVO_MOTOR1_PWM, (uint16)SERVO_MOTOR_DUTY(angle));
     
    }
}

#endif

#if EXAMPLE_CODE == 3

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

int main(int argc, char *argv[]) 
{
    // 注册清理函数
    atexit(cleanup);

    // 注册SIGINT信号的处理函数
    signal(SIGINT, sigint_handler);

    // 获取PWM设备信息
    pwm_get_dev_info(SERVO_MOTOR1_PWM, &servo_pwm_info);

    int angle1 = atoi(argv[1]);
    int angle2 = atoi(argv[2]);
    
    // 打印PWM频率和duty最大值
    printf("servo pwm freq = %d Hz\r\n", servo_pwm_info.freq);
    printf("servo pwm duty_max = %d\r\n", servo_pwm_info.duty_max);


    while(1)
    {
        pwm_set_duty(SERVO_MOTOR1_PWM, (uint16)SERVO_MOTOR_DUTY(servo_motor_duty));


        if(servo_motor_dir)
        {
            servo_motor_duty ++;
            if(servo_motor_duty >= angle2)
            {
                servo_motor_dir = 0x00;
            }
        }
        else
        {
            servo_motor_duty --;
            if(servo_motor_duty <= angle1)
            {
                servo_motor_dir = 0x01;
            }
        }
        system_delay_ms(50);         
    }
}
#endif

#if EXAMPLE_CODE == 4

void cleanup()
{
    printf("程序异常退出，执行清理操作\n");
    // 关闭电机
    pwm_set_duty(MOTOR1_PWM, 0);   
    pwm_set_duty(MOTOR2_PWM, 0);    
}
void sigint_handler(int signum) 
{
    printf("收到Ctrl+C，程序即将退出\n");
    pwm_set_duty(MOTOR1_PWM, 0);   
    pwm_set_duty(MOTOR2_PWM, 0);    
    exit(0);
}

void pit_callback()
{
    encoder_Left  = encoder_get_count(ENCODER_1);
    encoder_Right = encoder_get_count(ENCODER_2);
}

int main(int argc, char *argv[])
{
    atexit(cleanup);
    signal(SIGINT, sigint_handler);
    pit_ms_init(5, pit_callback);

    int speedl = atoi(argv[1]);
    int speedr = atoi(argv[2]);
    printf("Motor1:%d\tMotor2:%d\r\n",speedl,speedr);
    motorControl(speedl,speedr);
    while(true)
    {
        printf("%d,%d\r\n", encoder_Left,encoder_Right);
        system_delay_ms(5);
    }


}

#endif
