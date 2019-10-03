#ifndef __PID_H__
#define __PID_H__
#include "common.h"
#include "Control.h"

#define KP 0
#define KI 1
#define KD 2
#define KT 3

typedef struct
{
	long SumError;	//误差累计	
	int32 LastError;	//Error[-1]
	int32 PrevError;	//Error[-2]	
	int32 LastSpeed;	//Speed[-1]
}PID_S;

extern PID_S S_D5_PID, MOTOR_PID;	//定义舵机和电机的PID参数结构体
extern float S_D5[7][4];
extern int S_D5_MOTOR[7];//后轮差速
extern float DKP;

//位置式PID参数初始化
void PlacePID_Init(PID_S *sptr);

//增量式PID参数初始化
void IncPID_Init(PID_S *sptr);

//位置式PID舵机控制
int32 PlacePID_Control(PID_S *sprt, int32 NowPiont, int32 SetPoint);
int32 PlacePID_Control_Obstacle(PID_S *sprt, int32 NowPiont, int32 SetPoint);
int32 PlacePID_Control_Broken(PID_S *sprt, int32 NowPiont, int32 SetPoint);

//增量式串级PID控制
int32 PID_Cascade(PID_S *sprt, int32 NowPiont, int32 SetPoint);

//增量式PID控制
int32 PID_Realize(PID_S *sptr, int32 ActualSpeed, int32 SetSpeed);

#endif
