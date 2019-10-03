#include "headfile.h"
#include "System.h"
#include "Control.h"
#include "Handle.h"
#include "DaJin.h"
#include "isr.h"
#include "PID.h"
#include "siyuansu.h"

/* 
	0：200 速度约1.9
	1：230 速度约2.1
	2：250 速度约2.3
	3：260 速度约2.5
	4：270 速度约
	5：275 速度约
	6：300 速度约
*/
char Mode_Set = 0;	// 模式选择 	very important !!!!!!
uint8 Element_Key = 1;//元素开关0关,1开

uint16 S_D5_Duty = 830;//舵机占空比 735-925
int16 ChaSu_Ero = 0;//差速偏移
uint8 Mid_Point_car = 79;//车在赛道中间时
uint8 Mid_Point_s = 79;//设定点横坐标
uint8 Point_s = 79;//目标点
float Z_Point_s = 0;//Z轴目标点
int16 A_Point_s = 0;//ADC差值

uint8 zuiyuan = 0;
uint8 Jiasu_Flag = 0;//加速标志
uint16 juli = 0;//超声波距离
int16 hangchen = 0;//航程
uint8 hangchen_flag = 0;

uint8 image_two[MT9V032_H * MT9V032_W] = {0};//二值化

int main(void)
{
    get_clk();
	System_Init();
    EnableInterrupts;
	
	//LCD显示
    //4行:00000__P000ERO
	//5行:SD000__D000Z
	//6行:PS000__T000
	//7行:SPEED00000__DAT000
	lcd_showstr(0,4,"AL/AM  P     ERO");//AM=Annulus_Mode
	lcd_showstr(0,5,"AR     D     Z");
	lcd_showstr(0,6,"SD     PS    Y");//SD舵机占空比,PS目标点
	lcd_showstr(0,7,"SPEED       DAT");
	
    while(1)
    {
		//nrf_rece_packet(nrf_rx_buff);

		/************************ 图像采集 ***********************/
		if(mt9v032_finish_flag)
		{
			mt9v032_finish_flag = 0;
			binaryzation(image[0],image_two,yuzhi);//二值化;
		}
		
		/************************* 图像算法分析 ************************/
		if(!Obstacle_Flag && !Broken_Flag)
		{
			Image_Handle(image_two);			//普通图像处理
			zuiyuan = Line_Count;
			
			/************************ 计算目标 *************************/
			Point_s = Point_Average();//中线法
//			Point_s = Point_Weight();//加权平均
			
			ChaSu_Ero =  Mid_Point_s - Point_s;//偏移
			/************************* 计算差速 ****************************/
			if(ChaSu_Ero <= 20 && ChaSu_Ero >= -20)
			{
				ChaSu_Ero = 0;
			}
			else
			{
				ChaSu_Ero = (ChaSu_Ero * MOTOR_Speed) / Speed_Set;
			}
		}
		else
		{
//			Point_s = Mid_Point_s;
			ChaSu_Ero = 0;
		}
		
		/************************* 加减速 ************************/
//		if(!Jiasu_Flag && zuiyuan == 13 && !Obstacle_Flag && !Broken_Flag && !PoDao_Flag && !Annulus_Flag)
//		{
//			if(image[13][80] > yuzhi && image[11][80] > yuzhi && image[9][80] > yuzhi)
//			{
//				if(Point_s >= 75 && Point_s <= 85)
//				{
//					Speed_Set = 300;
//					
//					Jiasu_Flag = 1;
////					gpio_set(B18, 1);
//				}
//			}
//		}
//		else if(image[13][80] < yuzhi || image[11][80] < yuzhi || image[9][80] < yuzhi)
//		{
//			if(Jiasu_Flag)
//			{
//				Speed_Control();	//速度初始化
//				
//				Jiasu_Flag = 0;
//				gpio_set(B18, 0);
//			}
//		}
		
		/************************* 圆环 ************************/
		Annulus_Handle();
		
		/************************* 元素判断 ************************/
		if(Element_Flag && !Obstacle_Flag && !Broken_Flag && !PoDao_Flag && !Annulus_Flag)
		{
			Decide_Element();
			Element_Flag = 0;
		}
		
		/************************* 元素 ************************/
		if(Obstacle_Flag)//障碍处理
		{
			Obstacle_Handle();
		}

		if(Broken_Flag)//断路处理
		{
			Broken_Handle();
		}
		
		if(PoDao_Flag)//坡道处理
		{
			PoDao_Handle();
		}
		
		/************************* 舵机调节 ****************************/
		Duoji_Handle();
		
		/************************ 起跑线检测与停车电机速度控制 *************************/
		Start_Control();	//起跑线检测与停车//速度控制放在定时器中断中，10ms执行一次
		
		/************************ 蓝牙 ***********************/
//		Uart_Handle();
		
		/************************* 显示按键 ****************************/
		if(!Run_Flag)
		{
			Show_Handle();//显示
			Key_Handle();//按键
		}
		lcd_showint32(40,7,MOTOR_Speed,5);//显示车速
		__NOP();
    }
}
