#pragma once

struct PIDStatus
{
  float target;					      //目标量

  float present;					        //当前量
  float before;
  float departure;					      //偏差量

  double time_present;
  double time_before;
  double time_departure;

  float Res;					            //执行量
};

struct PID
{

  float P = 0;					              //比例P
  float I = 0;				                //积分I
  float D = 0;				                //微分D

  float Kp = 0;			              //比例系数
  float Ki = 0;				                //积分系数
  float Kd = 0;			                  //微分系数
  
  float Plimit = 100;
  float Ilimit = 100;
  float Dlimit = 100;
  float Reslimit = 100;


};

void MaxMinf(float * val,float absval);
void PIDCalculate(PID pid,PIDStatus *pidstatus);


