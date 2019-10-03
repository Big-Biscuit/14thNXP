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
	long SumError;	//����ۼ�	
	int32 LastError;	//Error[-1]
	int32 PrevError;	//Error[-2]	
	int32 LastSpeed;	//Speed[-1]
}PID_S;

extern PID_S S_D5_PID, MOTOR_PID;	//�������͵����PID�����ṹ��
extern float S_D5[7][4];
extern int S_D5_MOTOR[7];//���ֲ���
extern float DKP;

//λ��ʽPID������ʼ��
void PlacePID_Init(PID_S *sptr);

//����ʽPID������ʼ��
void IncPID_Init(PID_S *sptr);

//λ��ʽPID�������
int32 PlacePID_Control(PID_S *sprt, int32 NowPiont, int32 SetPoint);
int32 PlacePID_Control_Obstacle(PID_S *sprt, int32 NowPiont, int32 SetPoint);
int32 PlacePID_Control_Broken(PID_S *sprt, int32 NowPiont, int32 SetPoint);

//����ʽ����PID����
int32 PID_Cascade(PID_S *sprt, int32 NowPiont, int32 SetPoint);

//����ʽPID����
int32 PID_Realize(PID_S *sptr, int32 ActualSpeed, int32 SetSpeed);

#endif
