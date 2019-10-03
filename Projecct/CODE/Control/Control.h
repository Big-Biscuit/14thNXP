#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "common.h"

#define S_D5_MID 830	//舵机中值840
#define S_D5_L 928	//左打死
#define S_D5_R 732	//右打死
#define MOTOR_MAX   9000

extern int16 ChaSu_Ero;//差速偏移

extern int32 Speed_Now,	// 当前实际速度
			 Speed_Min,	// 左右最小速度
			 Speed_Set; // 目标设定速度

extern int32 MOTOR_Duty;
extern int32 MOTOR_Speed;
extern int32 MOTOR_Acc;
extern int32 RC_Get;
extern uint8 Run_Flag;
extern uint8 Stop_Flag;
extern uint8 Run_Stop;
extern uint8 Ready_Stop;
extern char Crazy;
extern uint32 Annulus_Error;
extern char Mode_Set;
extern uint8 Point_Mid;//目标点横坐标

extern uint8 Mid_Point_car;
extern uint8 Mid_Point_s;

extern int16 hangchen;//航程
extern uint8 hangchen_flag;

/*********** 函数声明 ************/
void Speed_Measure(void);	//电机速度测量
void Start_Control(void);	//起跑线检测与停车控制
void Speed_Control(void);
void MOTOR_Control(int32 Duty);	// 电机控制
int32 range_protect(int32 duty, int32 min, int32 max); //限幅保护

void Speed_Set_6(void);
void Speed_Set_7(void);

#endif
