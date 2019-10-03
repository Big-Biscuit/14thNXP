#ifndef _SIYUANSU_H
#define _SIYUANSU_H

#include "headfile.h"

typedef struct
{
	float X;
	float Y;
	float Z;
} S_FLOAT_XYZ;

typedef struct
{
	int32 X;
	int32 Y;
	int32 Z;
} S_INT32_XYZ;

typedef struct
{
	int16 X;
	int16 Y;
	int16 Z;
} S_INT16_XYZ;

extern S_FLOAT_XYZ 
	GYRO_Real,		// ������ת���������
	ACC_Real,		// ���ٶȼ�ת���������
	Attitude_Angle,	// ��ǰ�Ƕ� 
	Last_Angle,		// �ϴνǶ�
	Target_Angle;	// Ŀ��Ƕ�

extern S_INT16_XYZ
	GYRO,			// ������ԭʼ����
	GYRO_Offset,	// ��������Ʈ
	GYRO_Last,		// �������ϴ�����
	ACC, 			// ���ٶȼ�����
	ACC_Offset,		// ���ٶȼ���Ʈ
	ACC_Last;		// ���ٶȼ��ϴ�����

extern S_INT32_XYZ
	Tar_Ang_Vel,	// Ŀ����ٶ�
	Tar_Ang_Vel_Last;	// �ϴ�Ŀ����ٶ�

void Data_Filter(void);//�����˲�
void KalmanFilter(float ACC_Angle);
void Get_Attitude(void);
void siyuansu_init(void);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);

#endif
