#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "common.h"

#define S_D5_MID 830	//�����ֵ840
#define S_D5_L 928	//�����
#define S_D5_R 732	//�Ҵ���
#define MOTOR_MAX   9000

extern int16 ChaSu_Ero;//����ƫ��

extern int32 Speed_Now,	// ��ǰʵ���ٶ�
			 Speed_Min,	// ������С�ٶ�
			 Speed_Set; // Ŀ���趨�ٶ�

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
extern uint8 Point_Mid;//Ŀ��������

extern uint8 Mid_Point_car;
extern uint8 Mid_Point_s;

extern int16 hangchen;//����
extern uint8 hangchen_flag;

/*********** �������� ************/
void Speed_Measure(void);	//����ٶȲ���
void Start_Control(void);	//�����߼����ͣ������
void Speed_Control(void);
void MOTOR_Control(int32 Duty);	// �������
int32 range_protect(int32 duty, int32 min, int32 max); //�޷�����

void Speed_Set_6(void);
void Speed_Set_7(void);

#endif
