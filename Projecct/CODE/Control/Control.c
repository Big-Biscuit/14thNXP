#include "System.h"
#include "headfile.h"
#include "Control.h"
#include "Handle.h"
#include "PID.h"

int32 Speed_Now = 0,	// ��ǰʵ���ٶ�
      Speed_Min = 0,	// ��С�ٶ�
      Speed_Set = 0; 	// Ŀ���趨�ٶ�

uint8 Run_Flag = 0;
uint8 Stop_Flag = 0;
uint8 Run_Stop = 1;
uint8 Ready_Stop = 0;
char Crazy = 0;	// �����ת
int32 RC_Get = 1500;	// ң������ͨ��ֵ

int32 MOTOR_Duty  = 0;
int32 MOTOR_Speed = 0; 
int32 MOTOR_Speed_Last = 0;
int32 MOTOR_Acc = 0;

uint32 Annulus_Error = 0;	// ��·���п���

uint8 se = 0;

/******* ����ٶȲ��� ********/
void Speed_Measure(void)
{
	int32 Pulses_l, Pulses_r;
	static int32 Speed_Last = 0;
	static int32 Crazy_Count = 0;
	
	/******* ����ٶ���ؿ��� ********/
	Pulses_l = ctimer_count_read(SPEED_PIN_l);//��ȡ�ɼ����ı�����������
	Pulses_r = ctimer_count_read(SPEED_PIN_r);
	ctimer_count_clean(SPEED_PIN_l);//����������
	ctimer_count_clean(SPEED_PIN_r);
	//�ɼ�������Ϣ
	if(gpio_get(SPEED_DIR_l)) Pulses_l = Pulses_l;// �õ�ת��
	else                      Pulses_l = -Pulses_l;
	if(gpio_get(SPEED_DIR_r)) Pulses_r = -Pulses_r;// �õ�ת��
	else                      Pulses_r = Pulses_r;
	
	MOTOR_Speed = (Pulses_l + Pulses_r) / 2;
	
	if(hangchen_flag)
	{
		hangchen += MOTOR_Speed;
	}

	MOTOR_Acc = MOTOR_Speed - MOTOR_Speed_Last;	// ������ٶ�
	if (MOTOR_Acc > 100)
	{
		Crazy = 1;	// ��ת
	}
	if (MOTOR_Speed > Speed_Set + 200)
	{
		Crazy = 2;	// ��ת
	}
	if (MOTOR_Speed < -250)
	{
		Crazy = -1;	// ��ת
	}
	
	if (Crazy)
	{
		if (MOTOR_Acc <= 100)
		{
			if (MOTOR_Speed < Speed_Set + 200 && MOTOR_Speed > 0)
			{
				Crazy = 0;
			}
		}
	}
	
	if (!Crazy)
	{
		MOTOR_Speed = MOTOR_Speed*0.9 + MOTOR_Speed_Last*0.1;
		MOTOR_Speed_Last = MOTOR_Speed;	// �����ٶ�
	}
	else
	{
		MOTOR_Speed = MOTOR_Speed*0.5 + MOTOR_Speed_Last*0.5;
		MOTOR_Speed_Last = MOTOR_Speed;	// �����ٶ�
	}
	/******* ����ٶ���ؿ��ƽ��� ********/
	
	
	if (Crazy || (Crazy && MOTOR_Speed < 20))
	{
		Crazy_Count++;
		if (Crazy_Count >= 40)
		{
			Crazy_Count = 0;
			Run_Flag = 0;
		}
	}
	else
	{
		Crazy = 0;
	}
	
	/******* �����ת���⴦�� ********/
	if (Crazy > 0)
	{
		Speed_Now = MOTOR_Speed_Last;			//��ת��ʹ���ϴ��ٶ���Ϊ��ǰʵ���ٶ�
	}
	else
	{
		Speed_Now = MOTOR_Speed;	//���㳵��ʵ���ٶ�
	}
	
	Speed_Now = Speed_Now *0.9 + Speed_Last * 0.1;
	Speed_Last = Speed_Now;
}
/* 
	0��200 �ٶ�Լ
	1��230 �ٶ�Լ
	2��250 �ٶ�Լ
	3��260 �ٶ�Լ
	4��270 �ٶ�Լ
	5��275 �ٶ�Լ
	6��300 �ٶ�Լ
*/
void Speed_Control(void)
{
		switch(Mode_Set)
		{
		case 0:		Speed_Set = 200;
				 	break;
		case 1:		Speed_Set = 230;
				 	break;
		case 2:		Speed_Set = 250;	// Ĭ�ϳ�ʼ�ٶ�
				 	break;
		case 3:		Speed_Set = 260;
				 	break;
		case 4:		Speed_Set = 270;
				 	break;
		case 5:		Speed_Set = 275;
				 	break;
		case 6:		Speed_Set = 300;
					break;
		default:	Speed_Set = 0;
					break;
		}
}

void MOTOR_Control(int32 Duty)
{
	int32 Duty_l, Duty_r;
	
	/**************����*************/
	Duty_l = Duty - ChaSu_Ero * S_D5_MOTOR[Mode_Set];
	Duty_r = Duty + ChaSu_Ero * S_D5_MOTOR[Mode_Set];
	
	if (Duty_l >= 0)
	{
		Duty_l = range_protect(Duty_l, 0, MOTOR_MAX);	// �޷�����
		ctimer_pwm_duty(TIMER1_PWMCH0_A18, 0);		//l�����ת
		ctimer_pwm_duty(TIMER1_PWMCH1_A20, Duty_l);	//ռ�ձ����10000��������
	}
	else
	{
		Duty_l = range_protect(-Duty_l, 0, MOTOR_MAX);	// �޷�����
		ctimer_pwm_duty(TIMER1_PWMCH1_A20, 0);    	//l�����ת
		ctimer_pwm_duty(TIMER1_PWMCH0_A18, Duty_l);	//ռ�ձ����10000������
	}
	if (Duty_r >= 0)
	{
		Duty_r = range_protect(Duty_r, 0, MOTOR_MAX);	// �޷�����
		ctimer_pwm_duty(TIMER2_PWMCH0_B5, 0);		//r�����ת
		ctimer_pwm_duty(TIMER2_PWMCH1_B4, Duty_r);	//ռ�ձ����10000��������
	}
	else
	{
		Duty_r = range_protect(-Duty_r, 0, MOTOR_MAX);	// �޷�����
		ctimer_pwm_duty(TIMER2_PWMCH1_B4, 0);		//r�����ת
		ctimer_pwm_duty(TIMER2_PWMCH0_B5, Duty_r);	//ռ�ձ����10000��������
	}
}

void Start_Control(void)
{
	static uint8 Start_OK = 0;
	static uint16 No_Start_Line_Count = 0;
	
	//��������
	nrf_rece_packet(nrf_rx_buff);
	if(Ready_Stop)
	{
		if(se == 0)
		{
			nrf_tx_buff[0] = 1;  
			nrf_tx_buff[1] = 'k';
			//��������
			nrf_send_packet(nrf_tx_buff);
		}
		else
		{
			se++;
			if(se >= 20)
			{
				se = 0;
			}
		}
		if(nrf_rx_buff[1] == 'k')
		{
			Run_Flag = 1;
			Stop_Flag = 0;
			hangchen_flag = 1;//hangchen += MOTOR_Speed;
			Mid_Point_s = Mid_Point_car - 20;
		}
		if(hangchen >= 6000)
		{
			Run_Flag = 0;
			Stop_Flag = 1;
			Ready_Stop = 0;
			hangchen = 0;
			Mid_Point_s = Mid_Point_car;
		}
	}
	
	if(Run_Flag && !Ready_Stop)
	{									
		/** ���ܽ׶μ�� **/
		if (!Starting_Line_Flag && !Start_OK)	//û�м�⵽��������δ�ɹ�����
		{
			No_Start_Line_Count++;			//�൱����ʱԼ5s
			if (No_Start_Line_Count >= 300)	//����200��û�м�⵽�����ߣ���������ʱ�Ĺ����쳣����
			{
				No_Start_Line_Count = 0;
				Start_OK = 1;	//�����ɹ�
			}
		}
		/** �����׶μ�� **/
		if (Start_OK && Starting_Line_Flag)	//�ɹ����ܺ��⵽�յ���
		{
			MOTOR_Control(-3800);
			Ready_Stop = 1;	//׼��ͣ��
			Stop_Flag = 1;
			Run_Flag = 0;			//ͣ��
		}
	}
}

/******** �޷����� *********/
int32 range_protect(int32 duty, int32 min, int32 max)//�޷�����
{
	if (duty >= max)
	{
		return max;
	}
	if (duty <= min)
	{
		return min;
	}
	else
	{
		return duty;
	}
}
