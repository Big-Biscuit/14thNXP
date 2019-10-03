#include "System.h"
#include "headfile.h"
#include "Handle.h"
#include "Control.h"
#include "siyuansu.h"
#include "isr.h"

uint8 colour[2] = {255, 0},black=1,white=0; //0 和 1 分别对应的颜色
uint8 Weight[60] = {70, 70, 69, 69, 63,   63, 57, 57, 52, 52,
					47, 47, 42, 42, 38,   38, 34, 34, 30, 30,
					27, 27, 24, 24, 21,   21, 19, 19, 17, 17,
					15, 15, 14, 14, 13,   13, 12, 12, 11, 11,
					10, 10,  9,  9,  8,    8,  7,  7,  6,  6,
				    5,   5,  4,  4,  3,    3,  2,  2,  1,  1};	//加权平均参数,倒叙

uint8 Left_Line[62], Right_Line[62], Mid_Line[62];	// 原始左右边界数据
uint8 Toushi_Mid_Line[62] = {0};
uint8 Toushi_L_Line[62] = {0};
uint8 Toushi_R_Line[62] = {0};
uint8 Left_Add_Line[62], Right_Add_Line[62];		// 左右边界补线数据
uint8 Left_Add_Flag[62], Right_Add_Flag[62];		// 左右边界补线标志位
uint8 Jump[62];
uint8 Width_Real[62];	// 实际赛道宽度
uint8 Width_Add[62];	// 补线赛道宽度
uint8 Width_Min;		// 最小赛道宽度

uint8 Foresight;	// 前瞻，用于速度控制
uint8 Out_Side = 0;	// 丢线控制
uint8 Line_Count;	// 记录成功识别到的赛道行数

uint8 Left_Add_Start, Right_Add_Start;	// 左右补线起始行坐标
uint8 Left_Add_Stop, Right_Add_Stop;	// 左右补线结束行坐标
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 0, Right_Kb = 0;	// 最小二乘法参数

uint8 Left_Hazard_Flag, Right_Hazard_Flag;	// 左右障碍物标志
uint8 Left_Max, Right_Min;
int32 Area_Left = 0, Area_Right = 0;	// 左右侧赛面积

uint8 Starting_Line_Flag = 0;	// 起跑线标志位

uint8 Element_Flag = 0;//元素
uint8 Element_Hang = 0;//元素所在行
uint8 Jintou_Hang = 0;//尽头所在行

/******************* 障碍相关变量 *************/
uint8 Obstacle_Flag = 0;
uint8 Obstacle_Mode = 0;//障碍状态,0开始,1右30,2左30,3左30
int16 Obstacle_Z_Value1 = 0;
int16 Obstacle_Z_Value2 = 0;
int16 Obstacle_Z_Value3 = 0;
/******************* 障碍相关变量 *************/

/******************* 断路相关变量 *************/
uint8 Broken_Flag = 0;
uint8 Broken_Mode = 0;//断路状态,0开始,1等待,2倒车,3出断路
uint8 Broken_Z_Flag = 0;
uint16 BM = 0;
/******************* 断路相关变量 *************/

/******************* 坡道相关变量 *************/
uint8 PoDao_Flag = 0;
/******************* 坡道相关变量 *************/

/******************* 圆环相关变量 *************/
uint8 Annulus_Flag = 0;
uint8 Annulus_L = 0;
uint8 Annulus_R = 0;
uint8 Left_Flag = 0;
uint8 Right_Flag = 0;
uint8 Annulus_Mode = 0;//0入环1环内2出环3环外
uint8 Annulus_lastpoint = 80;
uint16 Annulus_yanshi[7] = {180, 180, 180, 180, 180, 180, 180};
int16 Annulus_jiansu[7] = {-1000, -2000, -3000, -3000, -4000, -4000, -4000};

uint16 bl = 0;//变量
/******************* 圆环相关变量 *************/

uint8 Picture_Flag = 0;//图像显示开关
uint8 s = 0;

/*
*	图像算法参数初始化
*
*	说明：仅影响第一行特殊处理
*/
void Image_Para_Init(void)
{	
	Mid_Line[61] = 80;
	Left_Line[61] = 1;
	Right_Line[61] = 159;
	Left_Add_Line[61] = 1;
	Right_Add_Line[61] = 159;
	Width_Real[61] = 158;
	Width_Add[61] = 158; 
}

/*
*	图像处理算法
*
*	说明：处理普通图像，包括十字、障碍
*/
void Image_Handle(uint8 *data)
{
	uint8 i;	// 控制行
	uint8 res;	// 用于结果状态判断
	uint8 Width_Check;
	uint8 Limit_Left, Limit_Right;
	
	Line_Count = 0;	// 赛道行数复位
	Starting_Line_Flag = 0;	// 起跑线标志位复位
	
	Left_Hazard_Flag = 0;	// 复位左右障碍物标志位
	Right_Hazard_Flag = 0;
	
	Left_Add_Start = 0;		// 复位补线起始行坐标
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;
	
	/***************************** 第一行特殊处理 *****************************/
	
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// 丢线
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	
	/*************************** 第一行特殊处理结束 ***************************/
	
	for (i = 59; i >= 15;)	// 仅处理前40行图像，隔行后仅处理20行数据
	{
		i -= 2;	// 隔行处理，减小单片机负荷
		
		if (Left_Add_Flag[i+2])
		{
			Limit_Left = Left_Line[i+2];
		}
		else
		{
			if (Jump[i] >= 2)
			{
				Limit_Left = Left_Add_Line[i+2];
			}
			else
			{
				Limit_Left = Left_Add_Line[i+2]+1;
			}
			
		}
		
		if (Right_Add_Flag[i+2])
		{
			Limit_Right = Right_Line[i+2];
		}
		else
		{
			if (Jump[i] >= 2)
			{
				Limit_Right = Right_Add_Line[i+2];
			}
			else
			{
				Limit_Right = Right_Add_Line[i+2]-1;
			}
		}
		Jump[i] = Corrode_Filter(i, data, Limit_Left, Limit_Right);	// 使用腐蚀滤波算法先对本行赛道进行预处理，返回跳变点数量
		if (Jump[i] >= 7 && i>= 25 && i <= 41)//
		{
			Starting_Line_Flag = 1;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
		if (!data[i*160 + Mid_Line[i+2]])//前2行中点在本行为黑点，可能是赛道结束，也可能是环路
		{
			if(!Left_Add_Start && !Right_Add_Start)	//两边都没有补线，即遇到了障碍或断路
			{
				if(!data[(i-1)*160 + Mid_Line[i+2]] && !data[(i-2)*160 + Mid_Line[i+2]])//后两行都是黑色
				{
					if(i >= 15 && Mid_Line[i+2] >= 60 && Mid_Line[i+2] <= 100)//i >= 19
					{
//						uart_putchar(USART_1,0x55);       //串口0发送0x55
//						chaoshengbo_flag = 0;
						
						Element_Flag = 1;
						Element_Hang = i;
						
						break;
					}
				}
			}
			else
			{
				if(Annulus_Flag)
				{
					Jintou_Hang = i;
				}
				else
				{
					Jintou_Hang = 0;
				}
			}
			break;
		}
		else	// 使用前2行中点向两边扫描边界
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		}
			
		/**************************** 补线检测开始 ****************************/
		if (Starting_Line_Flag)
		{
			Width_Check = 7;///////////////////////8
		}
		else
		{
			Width_Check = 2;///////////////////////8
		}
		if (Width_Real[i] > Width_Min + Width_Check)	// 赛道宽度变宽，可能是十字或环路
		{
			if (Left_Add_Line[i] <  Left_Add_Line[i+2])
			{
				if (!Left_Add_Flag[i])
				{
					Left_Add_Flag[i] = 1;	// 强制认定为需要补线
				}
			}
			if (Right_Add_Line[i] > Right_Add_Line[i+2])
			{
				if (!Right_Add_Flag[i])
				{
					Right_Add_Flag[i] = 1;	// 强制认定为需要补线
				}
			}
			if (Left_Add_Flag[i] || Right_Add_Flag[i])
			{
				if (Left_Add_Stop && Right_Add_Stop)
				{
					break;
				}
			}
		}
		
		/**************************** 补线检测结束 ****************************/
		
		
		/**************************** 障碍检测开始 ****************************/
		
		else
		{
			if ((i != 59) && (Width_Real[i]+12 < Width_Real[i+2]) && (Jump[i] == 2))
			{
				if (Left_Add_Line[i] > Left_Add_Line[i+2] + 6 && !Left_Add_Start)	// 本行与前一行左边界有较大突变且没有补线
				{
					Left_Hazard_Flag = i;	// 障碍物标志位置位
				}
				if (Right_Add_Line[i] < Right_Add_Line[i+2] - 6 && !Right_Add_Start)	// 本行与前一行右边界有较大突变且没有补线
				{
					Right_Hazard_Flag = i;	// 障碍物标志位置位
				}
			}
		}
		
		/**************************** 障碍检测结束 ****************************/
		
		
		/*************************** 第一轮补线开始 ***************************/
		
		if (Left_Add_Flag[i])	// 左侧需要补线
		{
			if (i >= 53)	// 前三行补线不算
			{
				if (!Left_Add_Start)
				{
					Left_Add_Start = i;	// 记录补线开始行
					Left_Ka = 0;
					Left_Kb = Left_Add_Line[i+2];
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// 使用前一帧图像左边界斜率补线
			}
			else
			{
				if (!Left_Add_Start)	// 之前没有补线
				{
					Left_Add_Start = i;	// 记录左侧补线开始行
					if (Left_Hazard_Flag)	// 左侧有障碍物
					{
						Left_Ka = 0;
						Left_Kb = Left_Add_Line[i+2];	// 竖直向上补线
					}
					else	// 斜率补线
					{
						Curve_Fitting(&Left_Ka, &Left_Kb, &Left_Add_Start, Left_Add_Line, Left_Add_Flag, 1);	// 使用两点法拟合直线
					}
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// 补线完成
			}
		}
		else
		{
			if (Left_Add_Start)	// 已经开始补线
			{
				if (!Left_Add_Stop && !Left_Add_Flag[i+2])
				{
					if (Left_Add_Line[i] >= Left_Add_Line[i+2])
					{
						Left_Add_Stop = i;	// 记录左侧补线结束行
					}
				}
			}
		}
		
		if (Right_Add_Flag[i])	// 右侧需要补线
		{
			if (i >= 53)	// 前三行补线不算
			{
				if (!Right_Add_Start)
				{
					Right_Add_Start = i;	// 记录补线开始行
					Right_Ka = 0;
					Right_Kb = Right_Add_Line[i+2];
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// 使用前一帧图像右边界斜率补线
			}
			else
			{
				if (!Right_Add_Start)	// 之前没有补线
				{
					Right_Add_Start = i;	// 记录右侧补线开始行
					if (Right_Hazard_Flag)	// 右侧有障碍物
					{
						Right_Ka = 0;
						Right_Kb = Right_Add_Line[i+2];	// 竖直向上补线
					}
					else
					{
						Curve_Fitting(&Right_Ka, &Right_Kb, &Right_Add_Start, Right_Add_Line, Right_Add_Flag, 2);	// 使用两点法拟合直线
					}
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// 补线完成
			}
		}
		else
		{
			if (Right_Add_Start)	// 已经开始补线
			{
				if (!Right_Add_Stop && !Right_Add_Flag[i+2])
				{
					if (Right_Line[i] <= Right_Line[i+2])
					{
						Right_Add_Stop = i;	// 记录右侧补线结束行
					}
				}
			}
		}
		
		/*************************** 第一轮补线结束 ***************************/
		
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 重新计算赛道宽度
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// 计算中线

		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// 更新最小赛道宽度
		}
		if (Width_Add[i] <= 5)	// 赛道宽度太小
		{
			break;
		}
		Line_Count = i;	// 记录成功识别到的赛道行数
	}
	/*************************** 第二轮补线修复开始 ***************************/
	
	if (Left_Add_Start)		// 左边界需要补线
	{
		Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag, 1);
	}
	if (Right_Add_Start)	// 右边界需要补线
	{
		Line_Repair(Right_Add_Start, Right_Add_Stop, data, Right_Line, Right_Add_Line, Right_Add_Flag, 2);
	}
	
	/*************************** 第二轮补线修复结束 ***************************/
	
	
	/****************************** 中线修复,透视变换开始 ******************************/
	
	Mid_Line_Repair(Line_Count, data);
	
	/****************************** 中线修复,透视变换结束 ******************************/
}

uint8 Fres = 0;	// 前瞻
void Fresight_Calculate(void)
{
	if (Speed_Set < 250)
	{
		Fres = 0;
	}
	else if (Speed_Set < 260)
	{
		Fres = 1;
	}
	else if (Speed_Set < 270)
	{
		Fres = 2;
	}
	else if (Speed_Set < 300)
	{
		Fres = 3;
	}
	else if (Speed_Set >= 300)
	{
		Fres = 4;
	}
}

uint8 Fresight[5][2] = {{29, 39},	// 200	前瞻控制{29, 35}{25, 35}
						{25, 35},	// 250	{21, 35}{25, 35}
						{25, 35},	// 260	{21, 31}
						{23, 33},	// 270	{23, 33}
						{21, 31}};	// 300	{19, 29}
/*
*	中线平均
*
*	说明：不做特殊处理，直接取均值
*/
int32 Point_Average(void)
{
	uint8 i, Point;
	uint8 qishixian = 61;
	int32 Sum = 0;
	static uint8 Last_Point = 80;
	
	Fresight_Calculate();	// 计算前瞻
	
	if (Line_Count <= Fresight[Fres][0])
	{
		Line_Count = Fresight[Fres][0];
		qishixian = Fresight[Fres][1];
	}
	else
	{
		qishixian = 61;
	}
	
	if (Out_Side || Line_Count >= 57)	// 出界或者摄像头图像异常
	{
		if (Last_Point <= 80)
		{
			Point  = 1;			// 使用上次目标点
		}
		else
		{
			Point = 159;
		}
	}
	else
	{
		for (i = qishixian; i >= Line_Count; i--)
		{
			Sum += Mid_Line[i];
		}

		Point = Sum / (qishixian+1-Line_Count);	// 对中线求平均
		
		Point = Point*0.8 + Last_Point*0.2;	// 低通滤波
		Point = range_protect(Point, 1, 159);		// 限幅，防止补偿溢出
		
		Last_Point = Point;	// 更新上次目标点
	}
	
	return (Point);
}

int16 Toushi_L_Average(void)
{
	int16 sum = 0;
	int8 i = 0;
	uint8 j = 0;
	for(i = 45; i >= 25; i--)//51 15
	{
		sum += Toushi_L_Line[i];
		if(Toushi_L_Line[i])
		{
			j++;
		}
	}
	
	sum = sum / j;
	return sum;
}
	
int16 Toushi_R_Average(void)
{
	int16 sum = 0;
	int8 i = 0;
	uint8 j = 0;
	for(i = 45; i >= 25; i--)//51 15
	{
		sum += Toushi_R_Line[i];
		if(Toushi_R_Line[i])
		{
			j++;
		}
	}
	
	sum = sum / j;
	return sum;
}

/*
*	面积算法
*
*	说明：计算左右面积
*/
int32 Area_Calculate(void)
{
	char i;
	uint8 Line_Start = 61;
	uint16 Area_Mid = 79;
	int32 Result;
	static int32 Result_Last = 0;
	
	Area_Left = 0;	// 左侧面积清零
	Area_Right = 0;	// 右侧面积清零
	
	Fresight_Calculate();	// 计算前瞻

	if (Line_Count <= Fresight[Fres][0])
	{
		Line_Count = Fresight[Fres][0];
	}
	
	for (i = Line_Start; i >= Line_Count+2;)
	{
		i -= 2;
		
		if (Mid_Line[i] == Area_Mid)
		{
			Area_Left += (Area_Mid - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - Area_Mid);
		}
		else if (Right_Add_Line[i] <= Area_Mid)	// 仅左侧有赛道
		{
			Area_Left += Width_Add[i];
		}
		else if (Left_Add_Line[i] >= Area_Mid)	// 仅右侧有赛道
		{
			Area_Right += Width_Add[i];
		}
		else
		{
			Area_Left += (Area_Mid - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - Area_Mid);
		}
	}
	
	Result = 50*(Area_Right - Area_Left)/(Area_Right + Area_Left);
	Result = range_protect(Result, -40, 40);
	
	Result = Result * 0.8 + Result_Last * 0.2;
	Result_Last = Result;
	
	return Result;
}

/*
*	斜率计算
*
*	说明：使用斜率作为转向控制量，返回斜率的倒数
*/
float Slope_Weight(uint8 *Mid)
{
	float Slope;
	
	Slope = 1.0 * (Mid[Line_Count] - 80) / (60-Line_Count);
	
	return Slope;
}

/*
*	赛道角度计算
*
*	说明：返回结果为 右：0：180°，1：135°，2：90°，>= 1可能为直角突变
*					 左：0：180°，-1：135°，-2：90°，<= -1可能为直角突变
*/
float Calculate_Angle(uint8 Point_1, uint8 Point_2, uint8 Point_3)
{
	char K1, K2;
	float Result;
	
	K1 = Point_2 - Point_1;
	K2 = Point_3 - Point_2;
	
	Result = (K2 - K1) * 0.5;
	
	return Result;
}

/*
*	两点法求直线
*
*	说明：拟合直线 y = Ka * x + Kb   Mode == 1代表左边界，Mode == 2代表右边界
*/
void Curve_Fitting(float *Ka, float *Kb, uint8 *Start, uint8 *Line, uint8 *Add_Flag, uint8 Mode)
{
	*Start += 4;
	if (Add_Flag[*Start] == 2)
	{
		if (*Start <= 51)
		{
			*Start += 2;
		}
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
		if (Mode == 2)
		{
			if (*Ka < 0)
			{
				*Ka = 0;
			}
		}
		if (Mode == 1)
		{
			if (*Ka > 0)
			{
				*Ka = 0;
			}
		}
	}
	else
	{
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
	}
	*Kb = 1.0*Line[*Start] - (*Ka * (*Start));
}

/*
*	计算补线坐标
*
*	说明：使用两点法计算拟合出的补线坐标
*/
uint8 Calculate_Add(uint8 i, float Ka, float Kb)	// 计算补线坐标
{
	float res;
	int32 Result;
	
	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 159);
	
	return (uint8)Result;
}

uint8 Test_Jump;
/*
*	腐蚀滤波
*
*	说明：将干扰滤除，并统计黑白跳变点数量，用于起跑线检测
*/
uint8 Corrode_Filter(uint8 i, uint8 *data, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j;
	uint8 White_Flag = 0;
	uint8 Jump_Count = 0;	// 跳变点计数
	
	Test_Jump = 0;
	
	Right_Max = range_protect(Right_Max, 1, 159);	// 保留右侧部分区域，防止溢出
	
	for (j = Left_Min; j <= Right_Max; j++)	// 从左向右扫描，方向不影响结果
	{
		if (!White_Flag)	// 先查找白点，只滤黑点，不滤白点
		{
			if (data[i*160 + j])	// 检测到白点
			{
				White_Flag = 1;	// 开始找黑点
			}
		}
		else
		{
			if (!data[i*160 + j])	// 检测到黑点
			{
				Jump_Count++;	// 视为一次跳变
				
				Test_Jump = Jump_Count;
				
				if (!data[i*160 + j+1] && j+1 <= Right_Max)	// 连续两个黑点
				{
					if (!data[i*160 + j+2] && j+2 <= Right_Max)	// 连续三个黑点
					{
						if (!data[i*160 + j+3] && j+3 <= Right_Max)	// 连续四个黑点
						{
							if (!data[i*160 + j+4] && j+4 <= Right_Max)	// 连续五个黑点
							{
								if (!data[i*160 + j+5] && j+5 <= Right_Max)	// 连续六个黑点
								{
									if (!data[i*160 + j+6] && j+6 <= Right_Max)	// 连续七个黑点
									{
										if (!data[i*160 + j+7] && j+7 <= Right_Max)	// 连续八个黑点
										{
											if (!data[i*160 + j+8] && j+8 <= Right_Max)	// 连续九个黑点
											{
												if (!data[i*160 + j+9] && j+9 <= Right_Max)	// 连续十个黑点
												{
													if (!data[i*160 + j+10] && j+10 <= Right_Max)	// 连续11个黑点
													{
														White_Flag = 0;	// 认为不是干扰，不做任何处理，下次搜索白点
														j += 10;
													}
													else if (j+10 <= Right_Max)
													{
														data[i*160 + j] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+1] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+2] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+3] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+4] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+5] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+6] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+7] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+8] = 255;	// 仅有连续10个黑点，滤除掉
														data[i*160+j+9] = 255;	// 仅有连续10个黑点，滤除掉
														
														j += 10;
													}
													else
													{
														j += 10;
													}
												}
												else if (j+9 <= Right_Max)
												{
													data[i*160 + j] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+1] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+2] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+3] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+4] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+5] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+6] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+7] = 255;	// 仅有连续九个黑点，滤除掉
													data[i*160+j+8] = 255;	// 仅有连续九个黑点，滤除掉
													
													j += 9;
												}
												else
												{
													j += 9;
												}
											}
											else if (j+8 <= Right_Max)
											{
												data[i*160 + j] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+1] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+2] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+3] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+4] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+5] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+6] = 255;	// 仅有连续八个黑点，滤除掉
												data[i*160+j+7] = 255;	// 仅有连续八个黑点，滤除掉
												
												j += 8;
											}	
											else
											{
												j += 8;
											}
										}
										else if (j+7 <= Right_Max)
										{
											data[i*160 + j] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+1] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+2] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+3] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+4] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+5] = 255;	// 仅有连续七个黑点，滤除掉
											data[i*160+j+6] = 255;	// 仅有连续七个黑点，滤除掉										
											
											j += 7;
										}	
										else
										{
											j += 7;
										}
									}
									else if (j+6 <= Right_Max)
									{
										data[i*160 + j] = 255;	// 仅有连续六个黑点，滤除掉
										data[i*160+j+1] = 255;	// 仅有连续六个黑点，滤除掉
										data[i*160+j+2] = 255;	// 仅有连续六个黑点，滤除掉
										data[i*160+j+3] = 255;	// 仅有连续六个黑点，滤除掉
										data[i*160+j+4] = 255;	// 仅有连续六个黑点，滤除掉
										data[i*160+j+5] = 255;	// 仅有连续六个黑点，滤除掉
										
										j += 6;
									}	
									else
									{
										j += 6;
									}
								}
								else if (j+5 <= Right_Max)
								{
									data[i*160 + j] = 255;	// 仅有连续五个黑点，滤除掉
									data[i*160+j+1] = 255;	// 仅有连续五个黑点，滤除掉
									data[i*160+j+2] = 255;	// 仅有连续五个黑点，滤除掉
									data[i*160+j+3] = 255;	// 仅有连续五个黑点，滤除掉
									data[i*160+j+4] = 255;	// 仅有连续五个黑点，滤除掉									
									
									j += 5;
								}	
								else
								{
									j += 5;
								}
							}
							else if (j+4 <= Right_Max)
							{
								data[i*160 + j] = 255;	// 仅有连续四个黑点，滤除掉
								data[i*160+j+1] = 255;	// 仅有连续四个黑点，滤除掉
								data[i*160+j+2] = 255;	// 仅有连续四个黑点，滤除掉
								data[i*160+j+3] = 255;	// 仅有连续四个黑点，滤除掉
								
								j += 4;
							}	
							else
							{
								j += 4;
							}
						}
						else if (j+3 <= Right_Max)
						{
							data[i*160 + j] = 255;	// 仅有连续三个黑点，滤除掉
							data[i*160+j+1] = 255;	// 仅有连续三个黑点，滤除掉
							data[i*160+j+2] = 255;	// 仅有连续三个黑点，滤除掉
							
							j += 3;
						}	
						else
						{
							j += 3;
						}
					}
					else if (j+2 <= Right_Max)
					{
						data[i*160 + j] = 255;	// 仅有连续两个黑点，滤除掉
						data[i*160+j+1] = 255;	// 仅有连续两个黑点，滤除掉
						
						j += 2;
					}	
					else
					{
						j += 2;
					}
				}
				else if (j+1 <= Right_Max)
				{
					data[i*160 + j] = 255;	// 有一个黑点，滤除掉
					
					j += 1;
				}	
				else
				{
					j += 1;
				}
			}
		}
	}
//	if (White_Flag)
//	{
//		Jump_Count++;	// 视为一次跳变
//	}
	
	return Jump_Count;	// 返回跳变点计数
}

/*
*	尽头搜索
*
*	说明：从某一点开始竖直向上搜索，返回最远行坐标
*/
uint8 Limit_Scan(uint8 i, uint8 *data, uint8 Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[160*i + Point])	// 搜索到黑点
		{
			break;
		}
	}
	
	return i;	// 返回最远行坐标
}

/*
*	第一行特殊处理
*
*	说明：先使用第60行中点作为第59行(第一行)搜线起始位置，成功搜索到左右边界后
 	将第59行中点赋值给第60行便于下一帧图像使用。如果第60行中点在本行为黑点，再
 	分别使用左遍历和右遍历的方法搜索边界，以赛道宽度较大的结果作为第59行边界，
 	若仍然搜索不到边界或数据异常认为出界，出界返回0
*/
uint8 First_Line_Handle(uint8 *data)
{
	uint8 i;	// 控制行
	uint8 Weight_Left, Weight_Right;	// 左右赛道宽度
	uint8 Mid_Left, Mid_Right;
	uint8 res;
	
	i = 59;
	
	res = Corrode_Filter(i, data, 1, 159);	// 使用腐蚀滤波算法先对本行赛道进行预处理，返回跳变点数量
	Jump[59] = res;
	
	if (!data[i*160 + Mid_Line[61]])	// 第61行中点在第59行为黑点
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 159);	// 从左侧搜索边界
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 159);// 从右侧搜索边界
		if (Weight_Left >= Weight_Right && Weight_Left)	// 左赛道宽度大于右赛道宽度且不为0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// 使用左遍历获取赛道边界
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// 使用右遍历获取赛道边界
		}
		else	// 说明没查到
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// 从前一行中点向两边扫描
	}
	
	Left_Line[61] = Left_Line[59];
	Right_Line[61] = Right_Line[59];
	Left_Add_Line[61] = Left_Add_Line[59];
	Right_Add_Line[61] = Right_Add_Line[59];
	if (Left_Add_Flag[59] && Right_Add_Flag[59])
	{
		Mid_Line[59] = Mid_Line[61];
	}
	else
	{
		Mid_Line[59] = (Right_Line[59] + Left_Line[59]) / 2;
		Mid_Line[61] = Mid_Line[59];	// 更新第60行虚拟中点，便于下一帧图像使用
	}
	if (Left_Add_Flag[59])
	{
		Left_Add_Start = i;	// 记录补线开始行
		Left_Ka = 0;
		Left_Kb = Left_Add_Line[59];
	}
	if (Right_Add_Flag[i])
	{
		Right_Add_Start = i;	// 记录补线开始行
		Right_Ka = 0;
		Right_Kb = Right_Add_Line[59];
	}
	
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];
	
	return 1;
}

/*
*	从左侧开始搜索边界，返回赛道宽度
*
*	说明：本函数仅仅作为探测赛道使用，仅返回赛道宽度，不保存边界数据
*/
uint8 Traversal_Left(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j, White_Flag = 0;
	uint8 Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Left_Min; j <= Right_Max; j++)	// 边界坐标 1到159
	{
		if (!White_Flag)	// 先查找左边界
		{
			if (data[i*160 + j])	// 检测到白点
			{
				Left_Line = j;	// 记录当前j值为本行左边界
				White_Flag = 1;	// 左边界已找到，必有右边界
				
				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*160 + j])//检测黑点
			{
				Right_Line = j-1;//记录当前j值为本行右边界
				
				break;	// 左右边界都找到，结束循环
			}
		}
	}
	
	if (!White_Flag)	// 未找到左右边界
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
		
		return (Right_Line - Left_Line);
	}
}

/*
*	从右侧开始搜索边界，返回赛道宽度
*
*	说明：本函数仅仅作为探测赛道使用，仅返回赛道宽度，不保存边界数据
*/
uint8 Traversal_Right(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j, White_Flag = 0;
	uint8 Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Right_Max; j >= Left_Min; j--)	// 边界坐标 1到159
	{
		if (!White_Flag)	// 先查找右边界
		{
			if (data[i*160 + j])	// 检测到白点
			{
				Right_Line = j;	// 记录当前j值为本行右边界
				White_Flag = 1;	// 右边界已找到，必有左边界
				
				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*160 + j])	//检测黑点
			{
				Left_Line = j+1;	//记录当前j值为本行左边界
				
				break;	// 左右边界都找到，结束循环
			}
		}
	}
	
	if (!White_Flag)	// 未找到左右边界
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
			
		return (Right_Line - Left_Line);
	}
}

/*
*	从左侧开始搜索边界，保存赛道边界，返回1成功 0失败
*
*	说明：本函数使用后将保存边界数据
*/
uint8 Traversal_Left_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line)
{
	uint8 j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 1; j < 160; j++)	// 边界坐标 1到159
	{
		if (!White_Flag)	// 先查找左边界
		{
			if (data[i*160 + j])	// 检测到白点
			{
				Left_Line[i] = j;	// 记录当前j值为本行左边界
				White_Flag = 1;		// 左边界已找到，必有右边界
				
				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*160 + j])	//检测黑点
			{
				Right_Line[i] = j-1;//记录当前j值为本行右边界
				
				break;	// 左右边界都找到，结束循环
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// 返回搜索结果
}

/*
*	从右侧开始搜索边界，保存赛道边界，返回1成功 0失败
*
*	说明：本函数使用后将保存边界数据
*/
uint8 Traversal_Right_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line)
{
	uint8 j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 159; j > 0; j--)	// 边界坐标 1到159
	{
		if (!White_Flag)	// 先查找右边界
		{
			if (data[i*160 + j])	// 检测到白点
			{
				Right_Line[i] = j;	// 记录当前j值为本行右边界
				White_Flag = 1;		// 右边界已找到，必有左边界
				
				continue;	// 结束本次循环，进入下次循环
			}
		}
		else
		{
			if (!data[i*160 + j])	//检测黑点
			{
				Left_Line[i] = j+1;//记录当前j值为本行左边界
				
				break;	// 左右边界都找到，结束循环
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// 返回搜索结果
}

/*
*	从中间向两边搜索边界
*
*	说明：本函数使用后将保存边界数据
*/
void Traversal_Mid_Line(uint8 i, uint8 *data, uint8 Mid, uint8 Left_Min, uint8 Right_Max, uint8 *Left_Line, uint8 *Right_Line, uint8 *Left_Add_Line, uint8 *Right_Add_Line)
{
	uint8 j;
	
	Left_Add_Flag[i] = 1;	// 初始化补线标志位
	Right_Add_Flag[i] = 1;
	
	Left_Min = range_protect(Left_Min, 1, 159);	// 限幅，防止出错
	if (Left_Add_Flag[i+2])
	{
		Left_Min = range_protect(Left_Min, Left_Add_Line[i+2]-30, 159);
	}
	Right_Max = range_protect(Right_Max, 1, 159);
	if (Right_Add_Flag[i+2])
	{
		Right_Max = range_protect(Right_Max, 1, Right_Add_Line[i+2]+30);
	}
	
	Right_Line[i] = Right_Max;
	Left_Line[i] = Left_Min;	// 给定边界初始值
	
	for (j = Mid; j >= Left_Min; j--)	// 以前一行中点为起点向左查找边界
	{
		if (!data[i*160 + j])	// 检测到黑点
		{
			Left_Add_Flag[i] = 0;	//左边界不需要补线，清除标志位
			Left_Line[i] = j+1;	//记录当前j值为本行实际左边界
			Left_Line[i+1] = (Left_Line[i] + Left_Line[i+2]) / 2;
			Left_Add_Line[i] = j+1;	// 记录实际左边界为补线左边界
			
			break;
		}
	}
	for (j = Mid; j <= Right_Max; j++)	// 以前一行中点为起点向右查找右边界
	{
		if (!data[i*160 + j])	//检测到黑点
		{
			Right_Add_Flag[i] = 0;		//右边界不需要补线，清除标志位
			Right_Line[i] = j-1;	//记录当前j值为本行右边界
			Right_Line[i+1] = (Right_Line[i] + Right_Line[i+2]) / 2;
			Right_Add_Line[i] = j-1;	// 记录实际右边界为补线左边界
			
			break;
		}
	}
	if (Left_Add_Flag[i+2])	// 左边界前一行需要补线
	{
		if (Left_Add_Line[i] <= Left_Add_Line[i+2])	// 本行限定就要严格Left_Add_Line[Left_Add_Start]///////////////
		{
			Left_Add_Flag[i] = 1;
		}
	}
	if (Right_Add_Flag[i+2])// 右边界前一行需要补线
	{
		if (Right_Add_Line[i] >= Right_Add_Line[i+2])	// 本行限定就要严格Right_Add_Line[Right_Add_Start]
		{
			Right_Add_Flag[i] = 1;
		}
	}
	if (Left_Add_Flag[i])	// 左边界需要补线
	{
		if (!data[(i-2)*160 + Left_Add_Line[i+2]] || !data[(i-4)*160 + Left_Add_Line[i+2]])	// 可能是反光干扰
		{
			Left_Add_Flag[i] = 0;	//左边界不需要补线，清除标志位
			Left_Line[i] = Left_Add_Line[i+2];		//记录当前j值为本行实际左边界
			Left_Add_Line[i] = Left_Add_Line[i+2];	// 记录实际左边界为补线左边界
		}
		else
		{
			if (i >= 55)	// 前6行
			{
				Left_Add_Line[i] = Left_Line[59];	// 使用底行数据
			}
			else
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];	// 使用前2行左边界作为本行左边界
			}
		}
	}
	if (Right_Add_Flag[i])	// 右边界需要补线
	{
		if (!data[(i-2)*160 + Right_Add_Line[i+2]] || !data[(i-4)*160 + Right_Add_Line[i+2]])	// 可能是反光干扰
		{
			Right_Add_Flag[i] = 0;	//左边界不需要补线，清除标志位
			Right_Line[i] = Right_Add_Line[i+2];		//记录当前j值为本行实际左边界
			Right_Add_Line[i] = Right_Add_Line[i+2];	// 记录实际左边界为补线左边界
		}
		else
		{
			if (i >= 55)	// 前6行
			{
				Right_Add_Line[i] = Right_Line[59];	// 使用底行数据
			}
			else
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];	// 使用前2行右边界作为本行右边界
			}
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// 计算实际赛道宽度
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// 计算补线赛道宽度
	
//	data[80 * i + Right_Line[i] + 2] = 0;//右移2位显示左边界，方便调试观察
//	data[80 * i + Left_Line[i] - 2] = 0;//左移2位显示右边界，方便调试观察
}

/*
*	补线修复
*
*	说明：有始有终才使用，直接使用两点斜率进行补线
*/
void Line_Repair(uint8 Start, uint8 Stop, uint8 *data, uint8 *Line, uint8 *Line_Add, uint8 *Add_Flag, uint8 Mode)
{
	float res;
	uint8 i, End;	// 控制行
//	uint8 Hazard_Width;
	float Ka, Kb;
	
	if ((Mode == 1) && (Right_Add_Start <= Stop) && Stop && Start <= 53 && !Left_Hazard_Flag)	// 左边界补线
	{
		for (i = Start+2; i >= Stop+2;)
		{
			i -= 2;
			Line_Add[i] = range_protect(Right_Add_Line[i] - Width_Add[i+2]+5, 1, Right_Add_Line[i]); 
			Width_Add[i] = Width_Add[i+2]-3;
			
			if (Width_Add[i] <= 20)
			{
				Line_Count = i;
				break;
			}
		}
	}
	if ((Mode == 2) && (Left_Add_Start <= Stop) && Stop && Start <= 53 && !Right_Hazard_Flag)	// 右边界补线
	{
		for (i = Start+2; i >= Stop+2;)
		{
			i -= 2;
			Line_Add[i] = range_protect(Left_Add_Line[i] + Width_Add[i+2]-5, Left_Add_Line[i], 159); 
			Width_Add[i] = Width_Add[i+2]-3;
			
			if (Width_Add[i] <= 20)
			{
				Line_Count = i;
				break;
			}
		}
	}
	if(Start > Stop)//////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		if (Stop)	// 有始有终
		{
			if(((Mode == 1 && Left_Add_Start >= 55) || (Mode == 2 && Right_Add_Start >= 55))
				&& ((Mode == 1 && Line_Add[Left_Add_Stop] > Line_Add[59])
				|| (Mode == 2 && Line_Add[Right_Add_Stop] < Line_Add[59])))// 只有较少行需要补线
			{
				if(Stop >= Line_Count + 4)
				{
					End = Stop - 4;
				}
				else if(Stop >= Line_Count + 2)
				{
					End = Stop - 2;
				}
				else
				{
					End = 0;
				}
				if (End)
				{
					Ka = 1.0*(Line_Add[Stop] - Line_Add[End]) / (Stop - End);
					Kb = 1.0*Line_Add[Stop] - (Ka * Stop);
					
					if (Mode == 1)
					{
						if (Line_Add[59] > 59 * Ka + Kb)
						{
							Ka = 1.0*(Line_Add[Start] - Line_Add[Stop-2]) / (Start - (Stop-2));
							Kb = 1.0*Line_Add[Start] - (Ka * Start);
						}
					}
					else if (Mode == 2)
					{
						if (Line_Add[59] < 59 * Ka + Kb)
						{
							Ka = 1.0*(Line_Add[Start] - Line_Add[Stop-2]) / (Start - (Stop-2));
							Kb = 1.0*Line_Add[Start] - (Ka * Start);
						}
					}
				}
				else
				{
					Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
					Kb = 1.0*Line_Add[Start] - (Ka * Start);
				}
				
				for (i = 61; i > Stop; )
				{
					i -= 2;
					res = i * Ka + Kb;
					Line_Add[i] = range_protect((int32)res, 1, 159);
				}
			}
			else	// 将起始行和结束行计算斜率补线
			{
				if (Start <= 57)
				{
					Start +=2;
				}
				if (Stop >= Line_Count + 4)
				{
					Stop -= 4;
				}
				else if (Stop >= Line_Count + 2)
				{
					Stop -= 2;
				}
				Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
				Kb = 1.0*Line_Add[Start] - (Ka * Start);
				
				if (Mode == 1)
				{
					if (Line_Add[59] > 59 * Ka + Kb)
					{
						Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
						Kb = 1.0*Line_Add[Start] - (Ka * Start);
					}
				}
				else if (Mode == 2)
				{
					if (Line_Add[59] < 59 * Ka + Kb)
					{
						Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
						Kb = 1.0*Line_Add[Start] - (Ka * Start);
					}
				}
				
				for (i = Stop; i <= Start; )
				{
					i += 2;
					res = i * Ka + Kb;
					Line_Add[i] = range_protect((int32)res, 1, 159);
				}
			}
		}
	}
	
	if (Mode == 1 && Left_Hazard_Flag)
	{
		for (i = Left_Hazard_Flag; i < 59; )
		{
			i += 2;
			Line_Add[i] = range_protect((int32)Line_Add[i-2]-5, Line_Add[i], 159);
		}
	}
	else if (Mode == 2 && Right_Hazard_Flag)
	{
		for (i = Right_Hazard_Flag; i < 59; )
		{
			i += 2;
			Line_Add[i] = range_protect((int32)Line_Add[i-2]+5, 1, Line_Add[i]);
		}
	}
}

/*
*	中线修复
*
*	说明：普通弯道丢线使用平移赛道方式，中点到达边界结束
*/
void Mid_Line_Repair(uint8 count, uint8 *data)
{
	int8 i;	// 控制行
	float tx = 0, ty = 0;
	double tu = 0, tv = 0;
	uint8 sh = 0, sw = 0;
	
	if(Annulus_Flag)
	{
		for(i = 61; i >= 0; i--)
		{
			Toushi_L_Line[i] = 0;
			Toushi_R_Line[i] = 0;
		}
	}
	
	for (i = 61; i >= count+2;)
	{
		i -= 2;
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// 计算赛道中点
		Mid_Line[i+1] = (Mid_Line[i] + Mid_Line[i+2]) / 2;
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// 计算赛道宽度
		
		/*************************** 上位机显示边界 ***************************/
		data[i*160 + Left_Add_Line[i]] = 2;	// 上位机显示补线后的左边界，不用时屏蔽
		data[i*160 + Right_Add_Line[i]] = 3;	// 上位机显示补线后的右边界，不用时屏蔽
		data[i*160 + Mid_Line[i]] = 4;			// 上位机显示中线，不用时屏蔽
		data[(i+1)*160 + Mid_Line[i+1]] = 4;
//		data[i*160 + Left_Line[i]] = 2;		// 上位机显示原始左边界，不用时屏蔽
//		data[i*160 + Right_Line[i]] = 3;		// 上位机显示原始右边界，不用时屏蔽
		/*************************** 上位机显示边界 ***************************/
		
		//中线透视校正
		if(Annulus_Flag)
		{
//			tx = Left_Add_Line[i];
			tx = Left_Line[i];
			ty = i;
			tu = (3.04f*tx+8.96f*ty-155.36f) / (0.12f*ty+1);
			tv = (9.96f*ty-129.8f) / (0.12f*ty+1);
			sh = (uint8)tv;
			sw = (uint8)tu;
			Toushi_L_Line[sh] = sw;//赋值
			
			data[sh*160 + sw] = 3;//中线透视
			
//			tx = Right_Add_Line[i];
			tx = Right_Line[i];
			ty = i;
			tu = (3.04f*tx+8.96f*ty-155.36f) / (0.12f*ty+1);
			tv = (9.96f*ty-129.8f) / (0.12f*ty+1);
			sh = (uint8)tv;
			sw = (uint8)tu;
			Toushi_R_Line[sh] = sw;//赋值
			
			data[sh*160 + sw] = 3;//中线透视
		}
	}
	Mid_Line[61] = Mid_Line[59];
}

/****************** 新算法 ******************/

/*
*	加权平均
*
*	说明：权重是乱凑的，效果不好
*/
uint8 Point_Weight(void)
{
	uint8 i ,Point, Point_Mid;
	static char Last_Point = 80;
	int32 Sum = 0, Weight_Count = 0;
	
	if (Line_Count <= 20)
	{
		Line_Count = 20;
	}
	
	if (Out_Side || Line_Count >= 53)	//出界或者摄像头图像异常
	{
		if (Last_Point == 80)
		{
			Point = Last_Point;
		}
		else if (Last_Point < 80)
		{
			Point = 1;
		}
		else if (Last_Point > 80)
		{
			Point = 159;
		}
	}
	else
	{
		for (i = 61; i >= Line_Count; )		//使用加权平均
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 1, 159);

							/*** 障碍物特殊情况处理 ***/
		if (Left_Hazard_Flag)			//左侧有障碍物且需要补线，即使误判也不会造成影响
		{
			Point = Mid_Line[Left_Hazard_Flag]+6;	//使用障碍物出现的那一行中点作为目标点
			if (Left_Hazard_Flag < 80)
			{
				Point += 3;
			}
		}
		else if (Right_Hazard_Flag)	//右测有障碍物且需要补线，即使误判也不会造成影响
		{
			Point = Mid_Line[Right_Hazard_Flag]-6;//使用障碍物出现的那一行中点作为目标点
			if (Right_Hazard_Flag < 80)
			{
				Point -= 5;
			}
		}
		Point = Mid_Line[59];
		Point = range_protect(Point, 2, 158);
		Last_Point = Point;
		
							/***** 使用最远行数据和目标点作为前瞻 *****/
		if (Line_Count >= 25)
		{
			Point_Mid = Mid_Line[60-30];
		}
		else
		{
			Point_Mid = Mid_Line[60-Line_Count];
		}
	}
	Foresight = 0.8 * Error_Transform(Point_Mid, 80)	//使用最远行偏差和加权偏差确定前瞻
			  + 0.2 * Error_Transform(Point, 	 80);
	
	return Point;
}

char Error_Transform(uint8 Data, uint8 Set)
{
	char Error;
	
	Error = Set - Data;
	if (Error < 0)
	{
		Error = -Error;
	}
	
	return Error;
}

//互补滤波
void Mid_Filtering(uint8 *data)//中线滤波
{
	uint8 i, Count;
	
	Count = Line_Count;	//判断一共有多少多少可用中点
	for (i = MT9V032_H-1; i >= Count+2;i--)	//从下向上滤波
	{
		Mid_Line[i] = Mid_Line[i+1]*0.3 + Mid_Line[i]*0.7;
	}
	for (i = Count+2; i < MT9V032_H; i++)	//从上向下互补滤波
	{
		Mid_Line[i] = Mid_Line[i-1]*0.3 + Mid_Line[i]*0.7;
	}
	for (i = MT9V032_H-1; i >= Count; i--)
	{
		data[160*i + Mid_Line[i]] = 4;	//将中线在图像上显示出来
	}
}

/*!
*  @brief      二值化图像解压（空间 换 时间 解压）
*  @param      dst             图像解压目的地址
*  @param      src             图像解压源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  while(srclen --)
  {
    tmpsrc = *src++;
    *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
  }
}

/*!
*  @brief      二值化图像压缩（空间 换 时间 压缩）
*  @param      dst             图像压缩目的地址
*  @param      src             图像压缩源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  uint8 i, j, k;
  uint32 Count = 0;
  
  for (i = 0; i < 60/2; i++)
  {
	  for (j = 0; j < 160/8; j++)
	  {
		  Count = (i*2+1)*160+j*8;
		  for (k = 0; k < 8; k++)		  
		  {
			  tmpsrc <<= 1;
			  if (!dst[Count++])
			  {
				  tmpsrc |= 1;
			  }
		  }
		  *src++ = tmpsrc;
	  }
  }
  
//  while(srclen --)
//  {
//    tmpsrc=0;
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x80;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x40;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x20;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x10;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x08;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x04;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x02;
//    }
//    
//    if(*dst++ == colour[black])
//    {
//      tmpsrc = tmpsrc + 0x01;
//    }
//    
//    *src++ = tmpsrc;
//  }
}

/*!
*  @brief      取边界线
*  @param      dst             图像压缩目的地址
*  @param      src             图像压缩源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_getline(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc,buff_y,buff_x;
  
  while(srclen --)
  {
    tmpsrc=0;
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x80;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x40;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x20;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x10;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x08;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x04;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x02;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if((*dst != buff_y && srclen % 10 !=0 ) || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x01;
      *(dst-1)=colour[black];
    }
    
    *src++ = tmpsrc;
  }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      图像二值化
//  @param      *data_s				源地址
//  @param      *data_a				目的地址
//  @param      v					阈值
//  @return     void
//  Sample usage:					binaryzation(da1[0],dat2,v);//将二维数组dat1的内容二值化到一维数组dat2里.				
//-------------------------------------------------------------------------------------------------------------------
void binaryzation(uint8 *data_s,uint8 *data_a,uint8 v)//二值化
{
	uint16 x;
	for(x = 0; x < MT9V032_H * MT9V032_W; x++)
	{
			if(*data_s++ > v) *data_a++ = 255;
			else *data_a++ = 0; 
	}
}

void Decide_Element(void)
{
	if(Element_Key)
	{
		if(Attitude_Angle.Y  <= -10 && Element_Hang >= 21)//<-12 >20   <= -12 >= 19
		{
			PoDao_Flag = 1;
		}
		else
		{
			jig_status = gpio_get(JIG);
			
			if(jig_status == 0)
			{
				Obstacle_Flag = 1;
				siyuansu_init();//初始化
				Attitude_Angle.Z = 0;
				
				MOTOR_Control(-2000);//减速
				Speed_Set = 180;
			}
			if(jig_status != 0 && Element_Hang >= 25)//>23
			{
				Broken_Flag = 1;
				
				MOTOR_Control(-4000);//减速
				Speed_Set = 100;
				
				A_Point_s = adc2_result - adc3_result;
			}
		}
	}
	else
	{
		Obstacle_Flag = 0;
		Broken_Flag = 0;
		PoDao_Flag = 0;
	}
}

void Obstacle_Handle(void)
{
	if(Obstacle_Mode == 0)
	{
		Obstacle_Z_Value1 = -40;//Attitude_Angle.Z
		Obstacle_Z_Value2 = 0;
		Obstacle_Z_Value3 = 20;
		Obstacle_Mode = 1;
	}
	if(Obstacle_Mode == 1)
	{
		Z_Point_s = Obstacle_Z_Value1;
		if(Attitude_Angle.Z <= Obstacle_Z_Value1)
		{
			Obstacle_Mode = 2;
			Z_Point_s = Obstacle_Z_Value2;
		}
	}
	if(Obstacle_Mode == 2)
	{
		Z_Point_s = Obstacle_Z_Value2;
		if(Attitude_Angle.Z >= Obstacle_Z_Value2)
		{
			Obstacle_Mode = 3;
			Z_Point_s = Obstacle_Z_Value3;
		}
	}
	if(Obstacle_Mode == 3)
	{
		Z_Point_s = Obstacle_Z_Value3;
		if(Attitude_Angle.Z >= Obstacle_Z_Value3)
		{
			Obstacle_Mode = 0;
			Obstacle_Flag = 0;
			
			Speed_Control();
		}
	}
}

void Broken_Handle(void)
{
	if(Broken_Mode == 0)
	{
		gpio_set(B18, 1);
		
		A_Point_s = adc2_result - adc3_result;
		Run_Flag = 0;//停车
		Stop_Flag = 1;
		
		if(MOTOR_Speed <= Speed_Set)//MOTOR_Speed <= 10 && MOTOR_Speed >= -10
		{
			Broken_Mode = 1;
		}	
	}
	else if(Broken_Mode == 1)
	{
		Speed_Set = 150;
		Run_Flag = 1;
		Stop_Flag = 0;
		hangchen_flag = 1;
		A_Point_s = adc2_result - adc3_result;
		
		if(hangchen >= 6000)
		{
			hangchen_flag = 0;
			Run_Flag = 0;//停车
			Stop_Flag = 1;
			if(MOTOR_Speed <= 10 && MOTOR_Speed >= -10)
			{
				hangchen = 0;
				Broken_Mode = 2;
				
				siyuansu_init();//初始化
				Attitude_Angle.Z = 0;
			}	
		}		
	}
	else if(Broken_Mode == 2)
	{
		Speed_Set = 150;
		Run_Flag = 1;
		Stop_Flag = 0;
		
		A_Point_s = 400;
		if(Attitude_Angle.Z >= 60)
		{
			Run_Flag = 0;//停车
			Stop_Flag = 1;
			if(MOTOR_Speed <= 10 && MOTOR_Speed >= -10)
			{
				Broken_Mode = 3;
			}	
		}
	}
	else if(Broken_Mode == 3)
	{
		Speed_Set = -150;
		Run_Flag = 1;
		Stop_Flag = 0;
		
		A_Point_s = -450;
		if(Attitude_Angle.Z >= 120)
		{
			Broken_Z_Flag = 1;
		}
		if(adc2_result > 100 || adc3_result > 100 && Broken_Z_Flag)
		{
			Run_Flag = 0;//停车
			Stop_Flag = 1;
			if(MOTOR_Speed <= 10 && MOTOR_Speed >= -10)
			{
				Broken_Mode = 4;
				Broken_Z_Flag = 0;
			}
		}
	}
	else if(Broken_Mode == 4)
	{
		Speed_Set = 100;
		Run_Flag = 1;
		Stop_Flag = 0;
		A_Point_s = adc2_result - adc3_result;
		hangchen_flag = 1;
		
		if(hangchen >= 2000)
		{
			hangchen_flag = 0;
			Run_Flag = 0;//停车
			Stop_Flag = 1;
			if(MOTOR_Speed <= 10 && MOTOR_Speed >= -10)
			{
				hangchen = 0;
				Broken_Mode = 5;
			}	
		}
	}
	else if(Broken_Mode == 5)
	{
		//接收数据
		nrf_rece_packet(nrf_rx_buff);
		if(nrf_rx_buff[1] == 'o')
		{
			Broken_Mode = 6;
			nrf_rx_buff[1] = 0;
		}
//		BM++;
//		if(BM >= 100)
//		{
//			BM = 0;
//			Broken_Mode = 6;
//		}
	}
	else if(Broken_Mode == 6)
	{
		Speed_Set = 100;
		Run_Flag = 1;
		Stop_Flag = 0;
		A_Point_s = adc2_result - adc3_result;
		
		if(image[55][77] > 70 && image[55][80] > 70 && image[55][83] > 70)//判断看到赛道
		{
			Speed_Control();	//速度初始化
			Broken_Mode = 0;
			Broken_Flag = 0;
			
			gpio_set(B18, 0);
		}
	}
	if(Broken_Mode >= 5)
	{
		nrf_tx_buff[0] = 1;  
		nrf_tx_buff[1] = 'o';
		//发送数据
		nrf_send_packet(nrf_tx_buff);
	}
}

void PoDao_Handle(void)
{
	gpio_set(B18, 1);
	
	hangchen_flag = 1;
	if(hangchen >= 4000)
	{
		hangchen_flag = 0;
		hangchen = 0;
		PoDao_Flag = 0;
		gpio_set(B18, 0);
	}
}

void Annulus_Handle(void)
{
	if(!Annulus_Flag)
	{
		if(adc2_result > 480 && adc3_result > 480)
		{
//			gpio_set(B18, 1);	//Beep
			
			if(Left_Add_Start > Right_Add_Start)
			{
				Annulus_Flag = 1;
				Annulus_L = 1;
				
				Speed_Set = 180;
//				MOTOR_Control(-4000);
				MOTOR_Control(0);
				
				siyuansu_init();//初始化
				Attitude_Angle.Z = 0;
			}
			if(Right_Add_Start > Left_Add_Start)
			{
				Annulus_Flag = 1;
				Annulus_R = 1;
				
				Speed_Set = 180;
//				MOTOR_Control(-4000);
				MOTOR_Control(0);
				
				siyuansu_init();//初始化
				Attitude_Angle.Z = 0;
			}
		}
	}
	
	if(Annulus_Flag)
	{
		if(Annulus_L)
		{
			if(Annulus_Mode == 0)
			{
				Point_s = Toushi_R_Average();
				Point_s = Point_s - 18;
				if(Left_Add_Start > 55 && (Left_Add_Start - Left_Add_Stop) >= 20)
				{
					Annulus_Mode = 8;
				}
				
				if(Speed_Now > Speed_Set)
				{
					MOTOR_Control(Annulus_jiansu[Mode_Set]);//减速
				}
			}
			if(Annulus_Mode == 8)
			{
				Point_s = 40;
				if(Attitude_Angle.Z >= 30 || Attitude_Angle.Z <= -30)
				{
					Annulus_Mode = 1;
				}
			}
			if(Annulus_Mode == 1)//环中
			{
				Point_s = range_protect(Point_s, 1, 80);
//				Point_s = Point_s + 3;
				
				if(Right_Add_Start > 45 && Right_Line[Right_Add_Start + 2] < 150 && Right_Line[Right_Add_Start + 2] > 80 && (Attitude_Angle.Z >= 60 || Attitude_Angle.Z <= -60))
				{
					Annulus_lastpoint = Point_s;
					Point_s = Annulus_lastpoint;
					Annulus_Mode = 2;
				}
			}
			if(Annulus_Mode == 2)//出环
			{
				Point_s = Annulus_lastpoint;
				if(Right_Add_Stop > 50)
				{
					Annulus_Mode = 3;
					gpio_set(B18, 1);	//Beep
				}
			}
			if(Annulus_Mode == 3)//延时
			{
				bl += 1;
//				Point_s = Point_s + 10;
				Point_s = Toushi_R_Average();
				Point_s = Point_s - 18;
				
				if(bl >= Annulus_yanshi[Mode_Set] && Left_Add_Start < 50 && Right_Add_Start < 50)
				{
					bl = 0;
					
					Annulus_Flag = 0;
					Annulus_L = 0;
					Left_Flag = 0;
					Annulus_Mode = 0;
					
					gpio_set(B18, 0);	//Beep
					Speed_Control();
				}
			}
		}
		
		if(Annulus_R)
		{
			if(Annulus_Mode == 0)
			{
				Point_s = Toushi_L_Average();
				Point_s = Point_s + 18;
				if(Right_Add_Start > 55 && (Right_Add_Start - Right_Add_Stop) >= 20)
				{
					Annulus_Mode = 8;
				}
				
				if(Speed_Now > Speed_Set)
				{
					MOTOR_Control(Annulus_jiansu[Mode_Set]);
				}
			}
			if(Annulus_Mode == 8)
			{
				Point_s = 115;
				if(Attitude_Angle.Z >= 30 || Attitude_Angle.Z <= -30)
				{
					Annulus_Mode = 1;
				}
			}
			if(Annulus_Mode == 1)
			{
				Point_s = range_protect(Point_s, 80, 159);
//				Point_s = Point_s - 3;
				
				if(Left_Add_Start > 45 && Left_Line[Left_Add_Start + 2] > 10 && Left_Line[Left_Add_Start + 2] < 80 && (Attitude_Angle.Z >= 60 || Attitude_Angle.Z <= -60))
				{
					Annulus_lastpoint = Point_s;
					Point_s = Annulus_lastpoint;
					Annulus_Mode = 2;
				}
			}
			if(Annulus_Mode == 2)
			{
				Point_s = Annulus_lastpoint;
				if(Left_Add_Stop > 50)
				{
					Annulus_Mode = 3;
					gpio_set(B18, 1);	//Beep
				}					
			}
			if(Annulus_Mode == 3)
			{
				bl += 1;
//				Point_s = Point_s - 10;
				Point_s = Toushi_L_Average();
				Point_s = Point_s + 18;
				
				if(bl >= Annulus_yanshi[Mode_Set] && Left_Add_Start < 50 && Right_Add_Start < 50)
				{
					bl = 0;
					
					Annulus_Flag = 0;
					Annulus_R = 0;
					Right_Flag = 0;
					Annulus_Mode = 0;
					
					gpio_set(B18, 0);	//Beep
					Speed_Control();
				}
			}
		}
	}
}

void Duoji_Handle(void)
{
	if(!Obstacle_Flag && !Broken_Flag)//普通
	{
//		lcd_showuint8(16,6,Point_s);
		S_D5_Duty = PlacePID_Control(&S_D5_PID, Mid_Point_s, Point_s);	//使用位置式PID解算	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//舵机占空比限幅保护
		
		/************************* 舵机 ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//修改舵机占空比735 925
	}
	if(Obstacle_Flag)//障碍
	{
//		lcd_showuint8(16,6,Z_Point_s);
		S_D5_Duty = PlacePID_Control_Obstacle(&S_D5_PID, Attitude_Angle.Z, Z_Point_s);	//使用位置式PID解算	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//舵机占空比限幅保护
		
		/************************* 舵机 ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//修改舵机占空比735 925
	}
	if(Broken_Flag)//断路
	{
//		lcd_showint16(16,6,A_Point_s);
		S_D5_Duty = PlacePID_Control_Broken(&S_D5_PID, A_Point_s, 0);	//使用位置式PID解算	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//舵机占空比限幅保护
		
		/************************* 舵机 ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//修改舵机占空比735 925
	}
}

void Show_Handle(void)
{
	lcd_showuint8(0,3,Annulus_Mode);//AM
	lcd_showuint8(32,3,zuiyuan);
	lcd_showuint8(64,3,image[55][80]);
	
	lcd_showuint16(16,4,adc2_result);//ADleft
	lcd_showfloat(64,4,S_D5[Mode_Set][KP],2,1);//整数显示2位,小数显示1位 P
	lcd_showuint8(128,4,S_D5_MOTOR[Mode_Set]);//后轮差速
	
	lcd_showuint16(16,5,adc3_result);//ADright
	lcd_showfloat(64,5,S_D5[Mode_Set][KD],2,1);//D
	lcd_showint32(128,5,Attitude_Angle.Z,5);//Z轴
	
	lcd_showuint16(16,6,S_D5_Duty);//SD舵机占空比
	lcd_showuint8(72,6,Point_s);//PS目标点
	lcd_showint32(128,6,Attitude_Angle.Y,5);//Y轴
	
	lcd_showint32(40,7,MOTOR_Speed,5);//车速
	lcd_showuint8(120,7,datbuf);//uart数据	
//	lcd_showuint8(40,7,zuiyuan);//显示有效行

	if(Picture_Flag)//图像显示
	{
		lcd_displayimage032_2(image_two,MT9V032_W,MT9V032_H);
		for(s = 0; s <= 60; s++)
		{
			lcd_drawpoint(80,s,RED);
		}
	}
}

void Uart_Handle(void)
{
	if(datbuf == 1)
	{
		Run_Flag = 1;
		Stop_Flag = 0;
	}
	else if(datbuf == 8)
	{
		Run_Flag = 0;
		Stop_Flag = 1;
	}
	else if(datbuf == 2)
	{
		S_D5[Mode_Set][KP] += 0.1f;
	}
	else if(datbuf == 3)
	{
		S_D5[Mode_Set][KP] -= 0.1f;
	}
	else if(datbuf == 4)
	{
		S_D5[Mode_Set][KD] += 0.1f;
	}
	else if(datbuf == 5)
	{
		S_D5[Mode_Set][KD] -= 0.1f;
	}
	else if(datbuf == 6)
	{
		S_D5[Mode_Set][KT] += 5;
	}
	else if(datbuf == 7)
	{
		S_D5[Mode_Set][KT] -= 5;
	}
	else if(datbuf == 9)
	{
		S_D5_MOTOR[Mode_Set] += 1;
	}
	else if(datbuf == 10)
	{
		S_D5_MOTOR[Mode_Set] -= 1;
	}
}

void Key_Handle(void)
{
	//保存按键状态;
	key3_last_status = key3_status;
	key4_last_status = key4_status;
	//读取当前按键状态
	key3_status = gpio_get(KEY3);
	key4_status = gpio_get(KEY4);
	//检测到按键按下之后并放开置位标志位
	if(key3_status && !key3_last_status)    key3_flag = 1;
	if(key4_status && !key4_last_status)    key4_flag = 1;
	//标志位置位之后，可以使用标志位执行自己想要做的事件
	if(key3_flag)   
	{
		key3_flag = 0;//使用按键之后，应该清除标志位
		Run_Flag = 0;
		Picture_Flag = ~Picture_Flag;
	}
	if(key4_flag)   
	{
		key4_flag = 0;//使用按键之后，应该清除标志位
		Run_Flag = 1;
	}
}
