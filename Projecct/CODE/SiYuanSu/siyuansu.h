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
	GYRO_Real,		// 陀螺仪转化后的数据
	ACC_Real,		// 加速度计转化后的数据
	Attitude_Angle,	// 当前角度 
	Last_Angle,		// 上次角度
	Target_Angle;	// 目标角度

extern S_INT16_XYZ
	GYRO,			// 陀螺仪原始数据
	GYRO_Offset,	// 陀螺仪温飘
	GYRO_Last,		// 陀螺仪上次数据
	ACC, 			// 加速度计数据
	ACC_Offset,		// 加速度计温飘
	ACC_Last;		// 加速度计上次数据

extern S_INT32_XYZ
	Tar_Ang_Vel,	// 目标角速度
	Tar_Ang_Vel_Last;	// 上次目标角速度

void Data_Filter(void);//数据滤波
void KalmanFilter(float ACC_Angle);
void Get_Attitude(void);
void siyuansu_init(void);
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);

#endif
