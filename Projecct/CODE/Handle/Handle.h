#ifndef __HANDLE_H__
#define __HANDLE_H__
#include "common.h"

extern uint8 Foresight;
extern uint8 Fres;
extern uint8 Element_Flag;//元素
extern uint8 Element_Hang;//元素所在行
extern uint8 Jintou_Hang;//尽头所在行
extern uint8 Obstacle_Flag;
extern uint8 Obstacle_Mode;//障碍状态,0开始,1右30,2左30,3左30
extern int16 Obstacle_Z_Value1;
extern int16 Obstacle_Z_Value2;
extern int16 Obstacle_Z_Value3;
extern uint8 Broken_Flag;
extern uint8 Broken_Mode;//断路状态,0开始,1等待,2倒车,3出断路
extern uint8 PoDao_Flag;//坡道
extern uint8 Annulus_Flag;//环
extern uint8 Annulus_L;
extern uint8 Annulus_R;
extern uint8 Annulus_Mode;//0入环1环内2出环
extern uint8 Out_Side;
extern uint8 Line_Count;
extern uint8 Left_Add_Start, Right_Add_Start;
extern uint8 Left_Add_Stop, Right_Add_Stop;
extern uint8 Starting_Line_Flag;
extern uint8 Left_Max, Right_Min;
extern uint8 Left_Line[62];
extern uint8 Right_Line[62];
extern uint8 Mid_Line[62];
extern uint8 Left_Add_Line[62];
extern uint8 Right_Add_Line[62];
extern int32 Area_Left, Area_Right;

extern uint8 Picture_Flag;//图像显示开关
extern uint8 image_two[MT9V032_H * MT9V032_W];//二值化
extern uint16 S_D5_Duty;//舵机占空比 735-925
extern uint8 Point_s;//目标点
extern int16 A_Point_s;//ADC差值
extern float Z_Point_s;//Z轴目标点
extern uint16 juli;//超声波距离
extern uint8 Element_Key;//元素开关,0关,1开
extern uint8 zuiyuan;

void Image_Para_Init(void);
void Image_Handle(uint8 *data);
void Fresight_Calculate(void);
int32 Area_Calculate(void);
int32 Point_Average_Left(void);
int32 Point_Average_Right(void);
int32 Point_Average(void);
float Slope_Weight(uint8 *Mid);
uint8 First_Line_Handle(uint8 *data);
uint8 Traversal_Left(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max);
uint8 Traversal_Right(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max);
uint8 Traversal_Left_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line);
uint8 Traversal_Right_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line);
uint8 Corrode_Filter(uint8 i, uint8 *data, uint8 Left_Min, uint8 Right_Max);
void Traversal_Mid_Line(uint8 i, uint8 *data, uint8 Mid, uint8 Left_Min, uint8 Right_Max,
uint8 *Left_Line, uint8 *Right_Line, uint8 *Left_Add_Line, uint8 *Right_Add_Line);
void Line_Repair(uint8 Start, uint8 Stop, uint8 *data, uint8 *Line, uint8 *Line_Add, uint8 *Add_Flag, uint8 Mode);
void Mid_Line_Repair(uint8 count, uint8 *data);

float Calculate_Angle(uint8 Point_1, uint8 Point_2, uint8 Point_3);
void Curve_Fitting(float *Ka, float *Kb, uint8 *Start, uint8 *Line, uint8 *Add_Flag, uint8 Mode);
uint8 Calculate_Add(uint8 i, float Ka, float Kb);
uint8 Limit_Scan(uint8 i, uint8 *data, uint8 Point);
uint8 Point_Weight(void);
char Error_Transform(uint8 Data, uint8 Set);

void Mid_Filtering(uint8 *data);//中线滤波

void img_extract(uint8 *dst, uint8 *src, uint32 srclen);
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen);
void img_getline(uint8 *dst, uint8 *src, uint32 srclen);
void binaryzation(uint8 *data_s,uint8 *data_a,uint8 v);//二值化

void Decide_Element(void);
void Obstacle_Handle(void);
void Broken_Handle(void);
void PoDao_Handle(void);
void Annulus_Handle(void);
void Duoji_Handle(void);//舵机
void Show_Handle(void);
void Uart_Handle(void);
void Key_Handle(void);

#endif
