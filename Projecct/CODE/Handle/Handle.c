#include "System.h"
#include "headfile.h"
#include "Handle.h"
#include "Control.h"
#include "siyuansu.h"
#include "isr.h"

uint8 colour[2] = {255, 0},black=1,white=0; //0 �� 1 �ֱ��Ӧ����ɫ
uint8 Weight[60] = {70, 70, 69, 69, 63,   63, 57, 57, 52, 52,
					47, 47, 42, 42, 38,   38, 34, 34, 30, 30,
					27, 27, 24, 24, 21,   21, 19, 19, 17, 17,
					15, 15, 14, 14, 13,   13, 12, 12, 11, 11,
					10, 10,  9,  9,  8,    8,  7,  7,  6,  6,
				    5,   5,  4,  4,  3,    3,  2,  2,  1,  1};	//��Ȩƽ������,����

uint8 Left_Line[62], Right_Line[62], Mid_Line[62];	// ԭʼ���ұ߽�����
uint8 Toushi_Mid_Line[62] = {0};
uint8 Toushi_L_Line[62] = {0};
uint8 Toushi_R_Line[62] = {0};
uint8 Left_Add_Line[62], Right_Add_Line[62];		// ���ұ߽粹������
uint8 Left_Add_Flag[62], Right_Add_Flag[62];		// ���ұ߽粹�߱�־λ
uint8 Jump[62];
uint8 Width_Real[62];	// ʵ���������
uint8 Width_Add[62];	// �����������
uint8 Width_Min;		// ��С�������

uint8 Foresight;	// ǰհ�������ٶȿ���
uint8 Out_Side = 0;	// ���߿���
uint8 Line_Count;	// ��¼�ɹ�ʶ�𵽵���������

uint8 Left_Add_Start, Right_Add_Start;	// ���Ҳ�����ʼ������
uint8 Left_Add_Stop, Right_Add_Stop;	// ���Ҳ��߽���������
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 0, Right_Kb = 0;	// ��С���˷�����

uint8 Left_Hazard_Flag, Right_Hazard_Flag;	// �����ϰ����־
uint8 Left_Max, Right_Min;
int32 Area_Left = 0, Area_Right = 0;	// ���Ҳ������

uint8 Starting_Line_Flag = 0;	// �����߱�־λ

uint8 Element_Flag = 0;//Ԫ��
uint8 Element_Hang = 0;//Ԫ��������
uint8 Jintou_Hang = 0;//��ͷ������

/******************* �ϰ���ر��� *************/
uint8 Obstacle_Flag = 0;
uint8 Obstacle_Mode = 0;//�ϰ�״̬,0��ʼ,1��30,2��30,3��30
int16 Obstacle_Z_Value1 = 0;
int16 Obstacle_Z_Value2 = 0;
int16 Obstacle_Z_Value3 = 0;
/******************* �ϰ���ر��� *************/

/******************* ��·��ر��� *************/
uint8 Broken_Flag = 0;
uint8 Broken_Mode = 0;//��·״̬,0��ʼ,1�ȴ�,2����,3����·
uint8 Broken_Z_Flag = 0;
uint16 BM = 0;
/******************* ��·��ر��� *************/

/******************* �µ���ر��� *************/
uint8 PoDao_Flag = 0;
/******************* �µ���ر��� *************/

/******************* Բ����ر��� *************/
uint8 Annulus_Flag = 0;
uint8 Annulus_L = 0;
uint8 Annulus_R = 0;
uint8 Left_Flag = 0;
uint8 Right_Flag = 0;
uint8 Annulus_Mode = 0;//0�뻷1����2����3����
uint8 Annulus_lastpoint = 80;
uint16 Annulus_yanshi[7] = {180, 180, 180, 180, 180, 180, 180};
int16 Annulus_jiansu[7] = {-1000, -2000, -3000, -3000, -4000, -4000, -4000};

uint16 bl = 0;//����
/******************* Բ����ر��� *************/

uint8 Picture_Flag = 0;//ͼ����ʾ����
uint8 s = 0;

/*
*	ͼ���㷨������ʼ��
*
*	˵������Ӱ���һ�����⴦��
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
*	ͼ�����㷨
*
*	˵����������ͨͼ�񣬰���ʮ�֡��ϰ�
*/
void Image_Handle(uint8 *data)
{
	uint8 i;	// ������
	uint8 res;	// ���ڽ��״̬�ж�
	uint8 Width_Check;
	uint8 Limit_Left, Limit_Right;
	
	Line_Count = 0;	// ����������λ
	Starting_Line_Flag = 0;	// �����߱�־λ��λ
	
	Left_Hazard_Flag = 0;	// ��λ�����ϰ����־λ
	Right_Hazard_Flag = 0;
	
	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;
	
	/***************************** ��һ�����⴦�� *****************************/
	
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// ����
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	
	/*************************** ��һ�����⴦����� ***************************/
	
	for (i = 59; i >= 15;)	// ������ǰ40��ͼ�񣬸��к������20������
	{
		i -= 2;	// ���д�����С��Ƭ������
		
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
		Jump[i] = Corrode_Filter(i, data, Limit_Left, Limit_Right);	// ʹ�ø�ʴ�˲��㷨�ȶԱ�����������Ԥ�����������������
		if (Jump[i] >= 7 && i>= 25 && i <= 41)//
		{
			Starting_Line_Flag = 1;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
		if (!data[i*160 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬����������������Ҳ�����ǻ�·
		{
			if(!Left_Add_Start && !Right_Add_Start)	//���߶�û�в��ߣ����������ϰ����·
			{
				if(!data[(i-1)*160 + Mid_Line[i+2]] && !data[(i-2)*160 + Mid_Line[i+2]])//�����ж��Ǻ�ɫ
				{
					if(i >= 15 && Mid_Line[i+2] >= 60 && Mid_Line[i+2] <= 100)//i >= 19
					{
//						uart_putchar(USART_1,0x55);       //����0����0x55
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
		else	// ʹ��ǰ2���е�������ɨ��߽�
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		}
			
		/**************************** ���߼�⿪ʼ ****************************/
		if (Starting_Line_Flag)
		{
			Width_Check = 7;///////////////////////8
		}
		else
		{
			Width_Check = 2;///////////////////////8
		}
		if (Width_Real[i] > Width_Min + Width_Check)	// ������ȱ��������ʮ�ֻ�·
		{
			if (Left_Add_Line[i] <  Left_Add_Line[i+2])
			{
				if (!Left_Add_Flag[i])
				{
					Left_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
				}
			}
			if (Right_Add_Line[i] > Right_Add_Line[i+2])
			{
				if (!Right_Add_Flag[i])
				{
					Right_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
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
		
		/**************************** ���߼����� ****************************/
		
		
		/**************************** �ϰ���⿪ʼ ****************************/
		
		else
		{
			if ((i != 59) && (Width_Real[i]+12 < Width_Real[i+2]) && (Jump[i] == 2))
			{
				if (Left_Add_Line[i] > Left_Add_Line[i+2] + 6 && !Left_Add_Start)	// ������ǰһ����߽��нϴ�ͻ����û�в���
				{
					Left_Hazard_Flag = i;	// �ϰ����־λ��λ
				}
				if (Right_Add_Line[i] < Right_Add_Line[i+2] - 6 && !Right_Add_Start)	// ������ǰһ���ұ߽��нϴ�ͻ����û�в���
				{
					Right_Hazard_Flag = i;	// �ϰ����־λ��λ
				}
			}
		}
		
		/**************************** �ϰ������� ****************************/
		
		
		/*************************** ��һ�ֲ��߿�ʼ ***************************/
		
		if (Left_Add_Flag[i])	// �����Ҫ����
		{
			if (i >= 53)	// ǰ���в��߲���
			{
				if (!Left_Add_Start)
				{
					Left_Add_Start = i;	// ��¼���߿�ʼ��
					Left_Ka = 0;
					Left_Kb = Left_Add_Line[i+2];
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// ʹ��ǰһ֡ͼ����߽�б�ʲ���
			}
			else
			{
				if (!Left_Add_Start)	// ֮ǰû�в���
				{
					Left_Add_Start = i;	// ��¼��ಹ�߿�ʼ��
					if (Left_Hazard_Flag)	// ������ϰ���
					{
						Left_Ka = 0;
						Left_Kb = Left_Add_Line[i+2];	// ��ֱ���ϲ���
					}
					else	// б�ʲ���
					{
						Curve_Fitting(&Left_Ka, &Left_Kb, &Left_Add_Start, Left_Add_Line, Left_Add_Flag, 1);	// ʹ�����㷨���ֱ��
					}
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// �������
			}
		}
		else
		{
			if (Left_Add_Start)	// �Ѿ���ʼ����
			{
				if (!Left_Add_Stop && !Left_Add_Flag[i+2])
				{
					if (Left_Add_Line[i] >= Left_Add_Line[i+2])
					{
						Left_Add_Stop = i;	// ��¼��ಹ�߽�����
					}
				}
			}
		}
		
		if (Right_Add_Flag[i])	// �Ҳ���Ҫ����
		{
			if (i >= 53)	// ǰ���в��߲���
			{
				if (!Right_Add_Start)
				{
					Right_Add_Start = i;	// ��¼���߿�ʼ��
					Right_Ka = 0;
					Right_Kb = Right_Add_Line[i+2];
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// ʹ��ǰһ֡ͼ���ұ߽�б�ʲ���
			}
			else
			{
				if (!Right_Add_Start)	// ֮ǰû�в���
				{
					Right_Add_Start = i;	// ��¼�Ҳಹ�߿�ʼ��
					if (Right_Hazard_Flag)	// �Ҳ����ϰ���
					{
						Right_Ka = 0;
						Right_Kb = Right_Add_Line[i+2];	// ��ֱ���ϲ���
					}
					else
					{
						Curve_Fitting(&Right_Ka, &Right_Kb, &Right_Add_Start, Right_Add_Line, Right_Add_Flag, 2);	// ʹ�����㷨���ֱ��
					}
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// �������
			}
		}
		else
		{
			if (Right_Add_Start)	// �Ѿ���ʼ����
			{
				if (!Right_Add_Stop && !Right_Add_Flag[i+2])
				{
					if (Right_Line[i] <= Right_Line[i+2])
					{
						Right_Add_Stop = i;	// ��¼�Ҳಹ�߽�����
					}
				}
			}
		}
		
		/*************************** ��һ�ֲ��߽��� ***************************/
		
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���¼����������
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ��������

		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// ������С�������
		}
		if (Width_Add[i] <= 5)	// �������̫С
		{
			break;
		}
		Line_Count = i;	// ��¼�ɹ�ʶ�𵽵���������
	}
	/*************************** �ڶ��ֲ����޸���ʼ ***************************/
	
	if (Left_Add_Start)		// ��߽���Ҫ����
	{
		Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag, 1);
	}
	if (Right_Add_Start)	// �ұ߽���Ҫ����
	{
		Line_Repair(Right_Add_Start, Right_Add_Stop, data, Right_Line, Right_Add_Line, Right_Add_Flag, 2);
	}
	
	/*************************** �ڶ��ֲ����޸����� ***************************/
	
	
	/****************************** �����޸�,͸�ӱ任��ʼ ******************************/
	
	Mid_Line_Repair(Line_Count, data);
	
	/****************************** �����޸�,͸�ӱ任���� ******************************/
}

uint8 Fres = 0;	// ǰհ
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

uint8 Fresight[5][2] = {{29, 39},	// 200	ǰհ����{29, 35}{25, 35}
						{25, 35},	// 250	{21, 35}{25, 35}
						{25, 35},	// 260	{21, 31}
						{23, 33},	// 270	{23, 33}
						{21, 31}};	// 300	{19, 29}
/*
*	����ƽ��
*
*	˵�����������⴦��ֱ��ȡ��ֵ
*/
int32 Point_Average(void)
{
	uint8 i, Point;
	uint8 qishixian = 61;
	int32 Sum = 0;
	static uint8 Last_Point = 80;
	
	Fresight_Calculate();	// ����ǰհ
	
	if (Line_Count <= Fresight[Fres][0])
	{
		Line_Count = Fresight[Fres][0];
		qishixian = Fresight[Fres][1];
	}
	else
	{
		qishixian = 61;
	}
	
	if (Out_Side || Line_Count >= 57)	// �����������ͷͼ���쳣
	{
		if (Last_Point <= 80)
		{
			Point  = 1;			// ʹ���ϴ�Ŀ���
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

		Point = Sum / (qishixian+1-Line_Count);	// ��������ƽ��
		
		Point = Point*0.8 + Last_Point*0.2;	// ��ͨ�˲�
		Point = range_protect(Point, 1, 159);		// �޷�����ֹ�������
		
		Last_Point = Point;	// �����ϴ�Ŀ���
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
*	����㷨
*
*	˵���������������
*/
int32 Area_Calculate(void)
{
	char i;
	uint8 Line_Start = 61;
	uint16 Area_Mid = 79;
	int32 Result;
	static int32 Result_Last = 0;
	
	Area_Left = 0;	// ����������
	Area_Right = 0;	// �Ҳ��������
	
	Fresight_Calculate();	// ����ǰհ

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
		else if (Right_Add_Line[i] <= Area_Mid)	// �����������
		{
			Area_Left += Width_Add[i];
		}
		else if (Left_Add_Line[i] >= Area_Mid)	// ���Ҳ�������
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
*	б�ʼ���
*
*	˵����ʹ��б����Ϊת�������������б�ʵĵ���
*/
float Slope_Weight(uint8 *Mid)
{
	float Slope;
	
	Slope = 1.0 * (Mid[Line_Count] - 80) / (60-Line_Count);
	
	return Slope;
}

/*
*	�����Ƕȼ���
*
*	˵�������ؽ��Ϊ �ң�0��180�㣬1��135�㣬2��90�㣬>= 1����Ϊֱ��ͻ��
*					 ��0��180�㣬-1��135�㣬-2��90�㣬<= -1����Ϊֱ��ͻ��
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
*	���㷨��ֱ��
*
*	˵�������ֱ�� y = Ka * x + Kb   Mode == 1������߽磬Mode == 2�����ұ߽�
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
*	���㲹������
*
*	˵����ʹ�����㷨������ϳ��Ĳ�������
*/
uint8 Calculate_Add(uint8 i, float Ka, float Kb)	// ���㲹������
{
	float res;
	int32 Result;
	
	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 159);
	
	return (uint8)Result;
}

uint8 Test_Jump;
/*
*	��ʴ�˲�
*
*	˵�����������˳�����ͳ�ƺڰ���������������������߼��
*/
uint8 Corrode_Filter(uint8 i, uint8 *data, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j;
	uint8 White_Flag = 0;
	uint8 Jump_Count = 0;	// ��������
	
	Test_Jump = 0;
	
	Right_Max = range_protect(Right_Max, 1, 159);	// �����Ҳಿ�����򣬷�ֹ���
	
	for (j = Left_Min; j <= Right_Max; j++)	// ��������ɨ�裬����Ӱ����
	{
		if (!White_Flag)	// �Ȳ��Ұ׵㣬ֻ�˺ڵ㣬���˰׵�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				White_Flag = 1;	// ��ʼ�Һڵ�
			}
		}
		else
		{
			if (!data[i*160 + j])	// ��⵽�ڵ�
			{
				Jump_Count++;	// ��Ϊһ������
				
				Test_Jump = Jump_Count;
				
				if (!data[i*160 + j+1] && j+1 <= Right_Max)	// ���������ڵ�
				{
					if (!data[i*160 + j+2] && j+2 <= Right_Max)	// ���������ڵ�
					{
						if (!data[i*160 + j+3] && j+3 <= Right_Max)	// �����ĸ��ڵ�
						{
							if (!data[i*160 + j+4] && j+4 <= Right_Max)	// ��������ڵ�
							{
								if (!data[i*160 + j+5] && j+5 <= Right_Max)	// ���������ڵ�
								{
									if (!data[i*160 + j+6] && j+6 <= Right_Max)	// �����߸��ڵ�
									{
										if (!data[i*160 + j+7] && j+7 <= Right_Max)	// �����˸��ڵ�
										{
											if (!data[i*160 + j+8] && j+8 <= Right_Max)	// �����Ÿ��ڵ�
											{
												if (!data[i*160 + j+9] && j+9 <= Right_Max)	// ����ʮ���ڵ�
												{
													if (!data[i*160 + j+10] && j+10 <= Right_Max)	// ����11���ڵ�
													{
														White_Flag = 0;	// ��Ϊ���Ǹ��ţ������κδ����´������׵�
														j += 10;
													}
													else if (j+10 <= Right_Max)
													{
														data[i*160 + j] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+1] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+2] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+3] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+4] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+5] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+6] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+7] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+8] = 255;	// ��������10���ڵ㣬�˳���
														data[i*160+j+9] = 255;	// ��������10���ڵ㣬�˳���
														
														j += 10;
													}
													else
													{
														j += 10;
													}
												}
												else if (j+9 <= Right_Max)
												{
													data[i*160 + j] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+1] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+2] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+3] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+4] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+5] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+6] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+7] = 255;	// ���������Ÿ��ڵ㣬�˳���
													data[i*160+j+8] = 255;	// ���������Ÿ��ڵ㣬�˳���
													
													j += 9;
												}
												else
												{
													j += 9;
												}
											}
											else if (j+8 <= Right_Max)
											{
												data[i*160 + j] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+1] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+2] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+3] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+4] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+5] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+6] = 255;	// ���������˸��ڵ㣬�˳���
												data[i*160+j+7] = 255;	// ���������˸��ڵ㣬�˳���
												
												j += 8;
											}	
											else
											{
												j += 8;
											}
										}
										else if (j+7 <= Right_Max)
										{
											data[i*160 + j] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+1] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+2] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+3] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+4] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+5] = 255;	// ���������߸��ڵ㣬�˳���
											data[i*160+j+6] = 255;	// ���������߸��ڵ㣬�˳���										
											
											j += 7;
										}	
										else
										{
											j += 7;
										}
									}
									else if (j+6 <= Right_Max)
									{
										data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+2] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+3] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+4] = 255;	// �������������ڵ㣬�˳���
										data[i*160+j+5] = 255;	// �������������ڵ㣬�˳���
										
										j += 6;
									}	
									else
									{
										j += 6;
									}
								}
								else if (j+5 <= Right_Max)
								{
									data[i*160 + j] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+1] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+2] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+3] = 255;	// ������������ڵ㣬�˳���
									data[i*160+j+4] = 255;	// ������������ڵ㣬�˳���									
									
									j += 5;
								}	
								else
								{
									j += 5;
								}
							}
							else if (j+4 <= Right_Max)
							{
								data[i*160 + j] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+1] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+2] = 255;	// ���������ĸ��ڵ㣬�˳���
								data[i*160+j+3] = 255;	// ���������ĸ��ڵ㣬�˳���
								
								j += 4;
							}	
							else
							{
								j += 4;
							}
						}
						else if (j+3 <= Right_Max)
						{
							data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
							data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
							data[i*160+j+2] = 255;	// �������������ڵ㣬�˳���
							
							j += 3;
						}	
						else
						{
							j += 3;
						}
					}
					else if (j+2 <= Right_Max)
					{
						data[i*160 + j] = 255;	// �������������ڵ㣬�˳���
						data[i*160+j+1] = 255;	// �������������ڵ㣬�˳���
						
						j += 2;
					}	
					else
					{
						j += 2;
					}
				}
				else if (j+1 <= Right_Max)
				{
					data[i*160 + j] = 255;	// ��һ���ڵ㣬�˳���
					
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
//		Jump_Count++;	// ��Ϊһ������
//	}
	
	return Jump_Count;	// ������������
}

/*
*	��ͷ����
*
*	˵������ĳһ�㿪ʼ��ֱ����������������Զ������
*/
uint8 Limit_Scan(uint8 i, uint8 *data, uint8 Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[160*i + Point])	// �������ڵ�
		{
			break;
		}
	}
	
	return i;	// ������Զ������
}

/*
*	��һ�����⴦��
*
*	˵������ʹ�õ�60���е���Ϊ��59��(��һ��)������ʼλ�ã��ɹ����������ұ߽��
 	����59���е㸳ֵ����60�б�����һ֡ͼ��ʹ�á������60���е��ڱ���Ϊ�ڵ㣬��
 	�ֱ�ʹ����������ұ����ķ��������߽磬��������Ƚϴ�Ľ����Ϊ��59�б߽磬
 	����Ȼ���������߽�������쳣��Ϊ���磬���緵��0
*/
uint8 First_Line_Handle(uint8 *data)
{
	uint8 i;	// ������
	uint8 Weight_Left, Weight_Right;	// �����������
	uint8 Mid_Left, Mid_Right;
	uint8 res;
	
	i = 59;
	
	res = Corrode_Filter(i, data, 1, 159);	// ʹ�ø�ʴ�˲��㷨�ȶԱ�����������Ԥ�����������������
	Jump[59] = res;
	
	if (!data[i*160 + Mid_Line[61]])	// ��61���е��ڵ�59��Ϊ�ڵ�
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 159);	// ����������߽�
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 159);// ���Ҳ������߽�
		if (Weight_Left >= Weight_Right && Weight_Left)	// ��������ȴ�������������Ҳ�Ϊ0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// ʹ���������ȡ�����߽�
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// ʹ���ұ�����ȡ�����߽�
		}
		else	// ˵��û�鵽
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 159, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// ��ǰһ���е�������ɨ��
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
		Mid_Line[61] = Mid_Line[59];	// ���µ�60�������е㣬������һ֡ͼ��ʹ��
	}
	if (Left_Add_Flag[59])
	{
		Left_Add_Start = i;	// ��¼���߿�ʼ��
		Left_Ka = 0;
		Left_Kb = Left_Add_Line[59];
	}
	if (Right_Add_Flag[i])
	{
		Right_Add_Start = i;	// ��¼���߿�ʼ��
		Right_Ka = 0;
		Right_Kb = Right_Add_Line[59];
	}
	
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];
	
	return 1;
}

/*
*	����࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uint8 Traversal_Left(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j, White_Flag = 0;
	uint8 Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Left_Line = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;	// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])//���ڵ�
			{
				Right_Line = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
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
*	���Ҳ࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uint8 Traversal_Right(uint8 i, uint8 *data, uint8 *Mid, uint8 Left_Min, uint8 Right_Max)
{
	uint8 j, White_Flag = 0;
	uint8 Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Right_Line = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;	// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Left_Line = j+1;	//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
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
*	����࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uint8 Traversal_Left_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line)
{
	uint8 j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 1; j < 160; j++)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Left_Line[i] = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;		// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Right_Line[i] = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
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
	
	return White_Flag;	// �����������
}

/*
*	���Ҳ࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uint8 Traversal_Right_Line(uint8 i, uint8 *data, uint8 *Left_Line, uint8 *Right_Line)
{
	uint8 j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 159;
	
	for (j = 159; j > 0; j--)	// �߽����� 1��159
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*160 + j])	// ��⵽�׵�
			{
				Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;		// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*160 + j])	//���ڵ�
			{
				Left_Line[i] = j+1;//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
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
	
	return White_Flag;	// �����������
}

/*
*	���м������������߽�
*
*	˵����������ʹ�ú󽫱���߽�����
*/
void Traversal_Mid_Line(uint8 i, uint8 *data, uint8 Mid, uint8 Left_Min, uint8 Right_Max, uint8 *Left_Line, uint8 *Right_Line, uint8 *Left_Add_Line, uint8 *Right_Add_Line)
{
	uint8 j;
	
	Left_Add_Flag[i] = 1;	// ��ʼ�����߱�־λ
	Right_Add_Flag[i] = 1;
	
	Left_Min = range_protect(Left_Min, 1, 159);	// �޷�����ֹ����
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
	Left_Line[i] = Left_Min;	// �����߽��ʼֵ
	
	for (j = Mid; j >= Left_Min; j--)	// ��ǰһ���е�Ϊ���������ұ߽�
	{
		if (!data[i*160 + j])	// ��⵽�ڵ�
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = j+1;	//��¼��ǰjֵΪ����ʵ����߽�
			Left_Line[i+1] = (Left_Line[i] + Left_Line[i+2]) / 2;
			Left_Add_Line[i] = j+1;	// ��¼ʵ����߽�Ϊ������߽�
			
			break;
		}
	}
	for (j = Mid; j <= Right_Max; j++)	// ��ǰһ���е�Ϊ������Ҳ����ұ߽�
	{
		if (!data[i*160 + j])	//��⵽�ڵ�
		{
			Right_Add_Flag[i] = 0;		//�ұ߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = j-1;	//��¼��ǰjֵΪ�����ұ߽�
			Right_Line[i+1] = (Right_Line[i] + Right_Line[i+2]) / 2;
			Right_Add_Line[i] = j-1;	// ��¼ʵ���ұ߽�Ϊ������߽�
			
			break;
		}
	}
	if (Left_Add_Flag[i+2])	// ��߽�ǰһ����Ҫ����
	{
		if (Left_Add_Line[i] <= Left_Add_Line[i+2])	// �����޶���Ҫ�ϸ�Left_Add_Line[Left_Add_Start]///////////////
		{
			Left_Add_Flag[i] = 1;
		}
	}
	if (Right_Add_Flag[i+2])// �ұ߽�ǰһ����Ҫ����
	{
		if (Right_Add_Line[i] >= Right_Add_Line[i+2])	// �����޶���Ҫ�ϸ�Right_Add_Line[Right_Add_Start]
		{
			Right_Add_Flag[i] = 1;
		}
	}
	if (Left_Add_Flag[i])	// ��߽���Ҫ����
	{
		if (!data[(i-2)*160 + Left_Add_Line[i+2]] || !data[(i-4)*160 + Left_Add_Line[i+2]])	// �����Ƿ������
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = Left_Add_Line[i+2];		//��¼��ǰjֵΪ����ʵ����߽�
			Left_Add_Line[i] = Left_Add_Line[i+2];	// ��¼ʵ����߽�Ϊ������߽�
		}
		else
		{
			if (i >= 55)	// ǰ6��
			{
				Left_Add_Line[i] = Left_Line[59];	// ʹ�õ�������
			}
			else
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];	// ʹ��ǰ2����߽���Ϊ������߽�
			}
		}
	}
	if (Right_Add_Flag[i])	// �ұ߽���Ҫ����
	{
		if (!data[(i-2)*160 + Right_Add_Line[i+2]] || !data[(i-4)*160 + Right_Add_Line[i+2]])	// �����Ƿ������
		{
			Right_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = Right_Add_Line[i+2];		//��¼��ǰjֵΪ����ʵ����߽�
			Right_Add_Line[i] = Right_Add_Line[i+2];	// ��¼ʵ����߽�Ϊ������߽�
		}
		else
		{
			if (i >= 55)	// ǰ6��
			{
				Right_Add_Line[i] = Right_Line[59];	// ʹ�õ�������
			}
			else
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];	// ʹ��ǰ2���ұ߽���Ϊ�����ұ߽�
			}
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// ����ʵ���������
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���㲹���������
	
//	data[80 * i + Right_Line[i] + 2] = 0;//����2λ��ʾ��߽磬������Թ۲�
//	data[80 * i + Left_Line[i] - 2] = 0;//����2λ��ʾ�ұ߽磬������Թ۲�
}

/*
*	�����޸�
*
*	˵������ʼ���ղ�ʹ�ã�ֱ��ʹ������б�ʽ��в���
*/
void Line_Repair(uint8 Start, uint8 Stop, uint8 *data, uint8 *Line, uint8 *Line_Add, uint8 *Add_Flag, uint8 Mode)
{
	float res;
	uint8 i, End;	// ������
//	uint8 Hazard_Width;
	float Ka, Kb;
	
	if ((Mode == 1) && (Right_Add_Start <= Stop) && Stop && Start <= 53 && !Left_Hazard_Flag)	// ��߽粹��
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
	if ((Mode == 2) && (Left_Add_Start <= Stop) && Stop && Start <= 53 && !Right_Hazard_Flag)	// �ұ߽粹��
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
		if (Stop)	// ��ʼ����
		{
			if(((Mode == 1 && Left_Add_Start >= 55) || (Mode == 2 && Right_Add_Start >= 55))
				&& ((Mode == 1 && Line_Add[Left_Add_Stop] > Line_Add[59])
				|| (Mode == 2 && Line_Add[Right_Add_Stop] < Line_Add[59])))// ֻ�н�������Ҫ����
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
			else	// ����ʼ�кͽ����м���б�ʲ���
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
*	�����޸�
*
*	˵������ͨ�������ʹ��ƽ��������ʽ���е㵽��߽����
*/
void Mid_Line_Repair(uint8 count, uint8 *data)
{
	int8 i;	// ������
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
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ���������е�
		Mid_Line[i+1] = (Mid_Line[i] + Mid_Line[i+2]) / 2;
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// �����������
		
		/*************************** ��λ����ʾ�߽� ***************************/
		data[i*160 + Left_Add_Line[i]] = 2;	// ��λ����ʾ���ߺ����߽磬����ʱ����
		data[i*160 + Right_Add_Line[i]] = 3;	// ��λ����ʾ���ߺ���ұ߽磬����ʱ����
		data[i*160 + Mid_Line[i]] = 4;			// ��λ����ʾ���ߣ�����ʱ����
		data[(i+1)*160 + Mid_Line[i+1]] = 4;
//		data[i*160 + Left_Line[i]] = 2;		// ��λ����ʾԭʼ��߽磬����ʱ����
//		data[i*160 + Right_Line[i]] = 3;		// ��λ����ʾԭʼ�ұ߽磬����ʱ����
		/*************************** ��λ����ʾ�߽� ***************************/
		
		//����͸��У��
		if(Annulus_Flag)
		{
//			tx = Left_Add_Line[i];
			tx = Left_Line[i];
			ty = i;
			tu = (3.04f*tx+8.96f*ty-155.36f) / (0.12f*ty+1);
			tv = (9.96f*ty-129.8f) / (0.12f*ty+1);
			sh = (uint8)tv;
			sw = (uint8)tu;
			Toushi_L_Line[sh] = sw;//��ֵ
			
			data[sh*160 + sw] = 3;//����͸��
			
//			tx = Right_Add_Line[i];
			tx = Right_Line[i];
			ty = i;
			tu = (3.04f*tx+8.96f*ty-155.36f) / (0.12f*ty+1);
			tv = (9.96f*ty-129.8f) / (0.12f*ty+1);
			sh = (uint8)tv;
			sw = (uint8)tu;
			Toushi_R_Line[sh] = sw;//��ֵ
			
			data[sh*160 + sw] = 3;//����͸��
		}
	}
	Mid_Line[61] = Mid_Line[59];
}

/****************** ���㷨 ******************/

/*
*	��Ȩƽ��
*
*	˵����Ȩ�����Ҵյģ�Ч������
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
	
	if (Out_Side || Line_Count >= 53)	//�����������ͷͼ���쳣
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
		for (i = 61; i >= Line_Count; )		//ʹ�ü�Ȩƽ��
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 1, 159);

							/*** �ϰ�������������� ***/
		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Left_Hazard_Flag]+6;	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
			if (Left_Hazard_Flag < 80)
			{
				Point += 3;
			}
		}
		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
		{
			Point = Mid_Line[Right_Hazard_Flag]-6;//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
			if (Right_Hazard_Flag < 80)
			{
				Point -= 5;
			}
		}
		Point = Mid_Line[59];
		Point = range_protect(Point, 2, 158);
		Last_Point = Point;
		
							/***** ʹ����Զ�����ݺ�Ŀ�����Ϊǰհ *****/
		if (Line_Count >= 25)
		{
			Point_Mid = Mid_Line[60-30];
		}
		else
		{
			Point_Mid = Mid_Line[60-Line_Count];
		}
	}
	Foresight = 0.8 * Error_Transform(Point_Mid, 80)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
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

//�����˲�
void Mid_Filtering(uint8 *data)//�����˲�
{
	uint8 i, Count;
	
	Count = Line_Count;	//�ж�һ���ж��ٶ��ٿ����е�
	for (i = MT9V032_H-1; i >= Count+2;i--)	//���������˲�
	{
		Mid_Line[i] = Mid_Line[i+1]*0.3 + Mid_Line[i]*0.7;
	}
	for (i = Count+2; i < MT9V032_H; i++)	//�������»����˲�
	{
		Mid_Line[i] = Mid_Line[i-1]*0.3 + Mid_Line[i]*0.7;
	}
	for (i = MT9V032_H-1; i >= Count; i--)
	{
		data[160*i + Mid_Line[i]] = 4;	//��������ͼ������ʾ����
	}
}

/*!
*  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
*  @param      dst             ͼ���ѹĿ�ĵ�ַ
*  @param      src             ͼ���ѹԴ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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
*  @brief      ��ֵ��ͼ��ѹ�����ռ� �� ʱ�� ѹ����
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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
*  @brief      ȡ�߽���
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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
//  @brief      ͼ���ֵ��
//  @param      *data_s				Դ��ַ
//  @param      *data_a				Ŀ�ĵ�ַ
//  @param      v					��ֵ
//  @return     void
//  Sample usage:					binaryzation(da1[0],dat2,v);//����ά����dat1�����ݶ�ֵ����һά����dat2��.				
//-------------------------------------------------------------------------------------------------------------------
void binaryzation(uint8 *data_s,uint8 *data_a,uint8 v)//��ֵ��
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
				siyuansu_init();//��ʼ��
				Attitude_Angle.Z = 0;
				
				MOTOR_Control(-2000);//����
				Speed_Set = 180;
			}
			if(jig_status != 0 && Element_Hang >= 25)//>23
			{
				Broken_Flag = 1;
				
				MOTOR_Control(-4000);//����
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
		Run_Flag = 0;//ͣ��
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
			Run_Flag = 0;//ͣ��
			Stop_Flag = 1;
			if(MOTOR_Speed <= 10 && MOTOR_Speed >= -10)
			{
				hangchen = 0;
				Broken_Mode = 2;
				
				siyuansu_init();//��ʼ��
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
			Run_Flag = 0;//ͣ��
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
			Run_Flag = 0;//ͣ��
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
			Run_Flag = 0;//ͣ��
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
		//��������
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
		
		if(image[55][77] > 70 && image[55][80] > 70 && image[55][83] > 70)//�жϿ�������
		{
			Speed_Control();	//�ٶȳ�ʼ��
			Broken_Mode = 0;
			Broken_Flag = 0;
			
			gpio_set(B18, 0);
		}
	}
	if(Broken_Mode >= 5)
	{
		nrf_tx_buff[0] = 1;  
		nrf_tx_buff[1] = 'o';
		//��������
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
				
				siyuansu_init();//��ʼ��
				Attitude_Angle.Z = 0;
			}
			if(Right_Add_Start > Left_Add_Start)
			{
				Annulus_Flag = 1;
				Annulus_R = 1;
				
				Speed_Set = 180;
//				MOTOR_Control(-4000);
				MOTOR_Control(0);
				
				siyuansu_init();//��ʼ��
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
					MOTOR_Control(Annulus_jiansu[Mode_Set]);//����
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
			if(Annulus_Mode == 1)//����
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
			if(Annulus_Mode == 2)//����
			{
				Point_s = Annulus_lastpoint;
				if(Right_Add_Stop > 50)
				{
					Annulus_Mode = 3;
					gpio_set(B18, 1);	//Beep
				}
			}
			if(Annulus_Mode == 3)//��ʱ
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
	if(!Obstacle_Flag && !Broken_Flag)//��ͨ
	{
//		lcd_showuint8(16,6,Point_s);
		S_D5_Duty = PlacePID_Control(&S_D5_PID, Mid_Point_s, Point_s);	//ʹ��λ��ʽPID����	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//���ռ�ձ��޷�����
		
		/************************* ��� ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//�޸Ķ��ռ�ձ�735 925
	}
	if(Obstacle_Flag)//�ϰ�
	{
//		lcd_showuint8(16,6,Z_Point_s);
		S_D5_Duty = PlacePID_Control_Obstacle(&S_D5_PID, Attitude_Angle.Z, Z_Point_s);	//ʹ��λ��ʽPID����	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//���ռ�ձ��޷�����
		
		/************************* ��� ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//�޸Ķ��ռ�ձ�735 925
	}
	if(Broken_Flag)//��·
	{
//		lcd_showint16(16,6,A_Point_s);
		S_D5_Duty = PlacePID_Control_Broken(&S_D5_PID, A_Point_s, 0);	//ʹ��λ��ʽPID����	
		S_D5_Duty = range_protect(S_D5_Duty, S_D5_R, S_D5_L);//���ռ�ձ��޷�����
		
		/************************* ��� ****************************/
		ctimer_pwm_duty(TIMER4_PWMCH0_A6, S_D5_Duty);//�޸Ķ��ռ�ձ�735 925
	}
}

void Show_Handle(void)
{
	lcd_showuint8(0,3,Annulus_Mode);//AM
	lcd_showuint8(32,3,zuiyuan);
	lcd_showuint8(64,3,image[55][80]);
	
	lcd_showuint16(16,4,adc2_result);//ADleft
	lcd_showfloat(64,4,S_D5[Mode_Set][KP],2,1);//������ʾ2λ,С����ʾ1λ P
	lcd_showuint8(128,4,S_D5_MOTOR[Mode_Set]);//���ֲ���
	
	lcd_showuint16(16,5,adc3_result);//ADright
	lcd_showfloat(64,5,S_D5[Mode_Set][KD],2,1);//D
	lcd_showint32(128,5,Attitude_Angle.Z,5);//Z��
	
	lcd_showuint16(16,6,S_D5_Duty);//SD���ռ�ձ�
	lcd_showuint8(72,6,Point_s);//PSĿ���
	lcd_showint32(128,6,Attitude_Angle.Y,5);//Y��
	
	lcd_showint32(40,7,MOTOR_Speed,5);//����
	lcd_showuint8(120,7,datbuf);//uart����	
//	lcd_showuint8(40,7,zuiyuan);//��ʾ��Ч��

	if(Picture_Flag)//ͼ����ʾ
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
	//���水��״̬;
	key3_last_status = key3_status;
	key4_last_status = key4_status;
	//��ȡ��ǰ����״̬
	key3_status = gpio_get(KEY3);
	key4_status = gpio_get(KEY4);
	//��⵽��������֮�󲢷ſ���λ��־λ
	if(key3_status && !key3_last_status)    key3_flag = 1;
	if(key4_status && !key4_last_status)    key4_flag = 1;
	//��־λ��λ֮�󣬿���ʹ�ñ�־λִ���Լ���Ҫ�����¼�
	if(key3_flag)   
	{
		key3_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
		Run_Flag = 0;
		Picture_Flag = ~Picture_Flag;
	}
	if(key4_flag)   
	{
		key4_flag = 0;//ʹ�ð���֮��Ӧ�������־λ
		Run_Flag = 1;
	}
}
