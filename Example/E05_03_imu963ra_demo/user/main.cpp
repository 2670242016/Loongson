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

void pit_callback()
{
    imu963ra_get_acc();
    imu963ra_get_gyro();
    imu963ra_get_mag();
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
        printf("imu963ra_acc_x  = %d\r\n", imu963ra_acc_x);
        printf("imu963ra_acc_y  = %d\r\n", imu963ra_acc_y);
        printf("imu963ra_acc_z  = %d\r\n", imu963ra_acc_z);

        printf("imu963ra_gyro_x = %d\r\n", imu963ra_gyro_x);
        printf("imu963ra_gyro_y = %d\r\n", imu963ra_gyro_y);
        printf("imu963ra_gyro_z = %d\r\n", imu963ra_gyro_z);

        printf("imu963ra_mag_x = %d\r\n", imu963ra_mag_x);
        printf("imu963ra_mag_y = %d\r\n", imu963ra_mag_y);
        printf("imu963ra_mag_z = %d\r\n", imu963ra_mag_z);
        system_delay_ms(100);
    }
}