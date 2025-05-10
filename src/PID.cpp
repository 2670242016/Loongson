#include "PID.h"

void MaxMinf(float * val,float absval)
{
  if (*val < (-1.0)*absval) *val = (-1.0)*absval;
	else if (*val > absval) *val = absval;
}

void PIDCalculate(PID *pid,PIDStatus *pidstatus)
{
  pidstatus->departure = (pidstatus->target) - (pidstatus->present);
  pidstatus->before = pidstatus->present;

  pidstatus->time_departure = (pidstatus->time_present)-(pidstatus->time_before);
  pidstatus->time_before = pidstatus->time_present;

  pid->P = pidstatus->departure * pid->Kp;
  MaxMinf(&(pid->P),pid->Plimit);

  pid->I += (pidstatus->before+pidstatus->present)*pidstatus->time_departure/2.0 * (pid->Ki);
  MaxMinf(&(pid->I),pid->Ilimit);

  pid->D = (pidstatus->departure)/pidstatus->time_departure * pid->Kd;
  MaxMinf(&(pid->D),pid->Dlimit);

  pidstatus->Res = pid->P + pid->I + pid->D;
  MaxMinf(&(pidstatus->Res),pid->Reslimit);
}

