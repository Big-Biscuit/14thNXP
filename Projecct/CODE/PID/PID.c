#include "PID.h"

PID_S S_D5_PID, MOTOR_PID;	//定义舵机和电机的PID参数结构体
float S_D5[7][4] = {{2.0, 200, 7.8, 0},{2.0, 230, 8.0, 0}, {2.0, 250, 10.0, 0}, {2.0, 260, 10.0, 0}, {2.0, 270, 15.0, 0}, {3.3, 275, 8.0, 0}, {3.3, 300, 8.0, 0}};	//舵机PID {2.5, 0, 5.5, 250}

int S_D5_MOTOR[7] = {10, 10, 15, 15, 15, 15, 15};//后轮差速
float DKP = 0;	

//位置式PID参数初始化
float MOTOR[3] = {4, 0.7, 0};	//电机PID 1.6, 0, 1.2  /4, 0.7, 0

void PlacePID_Init(PID_S *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]
}
//增量式PID参数初始化
void IncPID_Init(PID_S *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]	
	sptr->LastSpeed = 0;
}

//位置式PID舵机控制
int32 PlacePID_Control(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//当前误差
	float Actual;	//最后得出的实际输出值
//	float	Kp;		//动态P

	iError = SetPoint - NowPiont;	//计算当前误差
	
//	Kp = 1.0 * (iError*iError) / S_D5[Mode_Set][KT] + S_D5[Mode_Set][KP];	//P值与差值成二次函数关系
//	DKP = Kp;
//	Actual = Kp * iError + S_D5[Mode_Set][KD] * (iError - sprt->LastError);//只用PD
	Actual = S_D5[Mode_Set][KP] * iError + S_D5[Mode_Set][KD] * (iError - sprt->LastError);
	
	sprt->LastError = iError;		//更新上次误差

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

//位置式PID舵机控制Obstacle
int32 PlacePID_Control_Obstacle(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//当前误差
	float Actual;	//最后得出的实际输出值

	iError = -(SetPoint - NowPiont);	//计算当前误差
	
	Actual = 10 * iError + 2 * (iError - sprt->LastError);
	
	sprt->LastError = iError;		//更新上次误差

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

//位置式PID舵机控制Broken
int32 PlacePID_Control_Broken(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//当前误差
	float Actual;	//最后得出的实际输出值

	iError = SetPoint - NowPiont;	//计算当前误差
	
	Actual = 0.2 * iError + 1 * (iError - sprt->LastError);//p0.2 d1.0
	
	sprt->LastError = iError;		//更新上次误差

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

int32 PID_Cascade(PID_S *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	//当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	register int32 iError,	    //当前误差
				   Increase;	//最后得出的实际增量
	
	iError = SetSpeed - ActualSpeed;//计算当前误差
	
	Increase = - MOTOR[KP] * (ActualSpeed - sptr->LastSpeed)	//加速度
			   + MOTOR[KI] * iError
			   + MOTOR[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);
//	Increase = MOTOR[KP] * iError + MOTOR[KD] * (iError - sptr->LastError);
	
	sptr->PrevError = sptr->LastError;	//更新前次误差
	sptr->LastError = iError;		  	//更新上次误差
	sptr->LastSpeed = ActualSpeed;		//更新上次速度
	
	return Increase;   
}	
