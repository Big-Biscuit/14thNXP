#include "headfile.h"
#include "System.h"
#include "Control.h"
#include "Handle.h"
#include "DaJin.h"
#include "isr.h"
#include "PID.h"
#include "siyuansu.h"

/* 
	0��200 �ٶ�Լ1.9
	1��230 �ٶ�Լ2.1
	2��250 �ٶ�Լ2.3
	3��260 �ٶ�Լ2.5
	4��270 �ٶ�Լ
	5��275 �ٶ�Լ
	6��300 �ٶ�Լ
*/
char Mode_Set = 0;	// ģʽѡ�� 	very important !!!!!!
uint8 Element_Key = 1;//Ԫ�ؿ���0��,1��

uint16 S_D5_Duty = 830;//���ռ�ձ� 735-925
int16 ChaSu_Ero = 0;//����ƫ��
uint8 Mid_Point_car = 79;//���������м�ʱ
uint8 Mid_Point_s = 79;//�趨�������
uint8 Point_s = 79;//Ŀ���
float Z_Point_s = 0;//Z��Ŀ���
int16 A_Point_s = 0;//ADC��ֵ

uint8 zuiyuan = 0;
uint8 Jiasu_Flag = 0;//���ٱ�־
uint16 juli = 0;//����������
int16 hangchen = 0;//����
uint8 hangchen_flag = 0;

uint8 image_two[MT9V032_H * MT9V032_W] = {0};//��ֵ��

int main(void)
{
    get_clk();
	System_Init();
    EnableInterrupts;
	
	//LCD��ʾ
    //4��:00000__P000ERO
	//5��:SD000__D000Z
	//6��:PS000__T000
	//7��:SPEED00000__DAT000
	lcd_showstr(0,4,"AL/AM  P     ERO");//AM=Annulus_Mode
	lcd_showstr(0,5,"AR     D     Z");
	lcd_showstr(0,6,"SD     PS    Y");//SD���ռ�ձ�,PSĿ���
	lcd_showstr(0,7,"SPEED       DAT");
	
    while(1)
    {
		//nrf_rece_packet(nrf_rx_buff);

		/************************ ͼ��ɼ� ***********************/
		if(mt9v032_finish_flag)
		{
			mt9v032_finish_flag = 0;
			binaryzation(image[0],image_two,yuzhi);//��ֵ��;
		}
		
		/************************* ͼ���㷨���� ************************/
		if(!Obstacle_Flag && !Broken_Flag)
		{
			Image_Handle(image_two);			//��ͨͼ����
			zuiyuan = Line_Count;
			
			/************************ ����Ŀ�� *************************/
			Point_s = Point_Average();//���߷�
//			Point_s = Point_Weight();//��Ȩƽ��
			
			ChaSu_Ero =  Mid_Point_s - Point_s;//ƫ��
			/************************* ������� ****************************/
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
		
		/************************* �Ӽ��� ************************/
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
//				Speed_Control();	//�ٶȳ�ʼ��
//				
//				Jiasu_Flag = 0;
//				gpio_set(B18, 0);
//			}
//		}
		
		/************************* Բ�� ************************/
		Annulus_Handle();
		
		/************************* Ԫ���ж� ************************/
		if(Element_Flag && !Obstacle_Flag && !Broken_Flag && !PoDao_Flag && !Annulus_Flag)
		{
			Decide_Element();
			Element_Flag = 0;
		}
		
		/************************* Ԫ�� ************************/
		if(Obstacle_Flag)//�ϰ�����
		{
			Obstacle_Handle();
		}

		if(Broken_Flag)//��·����
		{
			Broken_Handle();
		}
		
		if(PoDao_Flag)//�µ�����
		{
			PoDao_Handle();
		}
		
		/************************* ������� ****************************/
		Duoji_Handle();
		
		/************************ �����߼����ͣ������ٶȿ��� *************************/
		Start_Control();	//�����߼����ͣ��//�ٶȿ��Ʒ��ڶ�ʱ���ж��У�10msִ��һ��
		
		/************************ ���� ***********************/
//		Uart_Handle();
		
		/************************* ��ʾ���� ****************************/
		if(!Run_Flag)
		{
			Show_Handle();//��ʾ
			Key_Handle();//����
		}
		lcd_showint32(40,7,MOTOR_Speed,5);//��ʾ����
		__NOP();
    }
}
