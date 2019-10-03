#include "System.h"
#include "headfile.h"
#include "Control.h"
#include "Handle.h"
#include "PID.h"

int32 Speed_Now = 0,	// 当前实际速度
      Speed_Min = 0,	// 最小速度
      Speed_Set = 0; 	// 目标设定速度

uint8 Run_Flag = 0;
uint8 Stop_Flag = 0;
uint8 Run_Stop = 1;
uint8 Ready_Stop = 0;
char Crazy = 0;	// 电机疯转
int32 RC_Get = 1500;	// 遥控器的通道值

int32 MOTOR_Duty  = 0;
int32 MOTOR_Speed = 0; 
int32 MOTOR_Speed_Last = 0;
int32 MOTOR_Acc = 0;

uint32 Annulus_Error = 0;	// 环路误判控制

uint8 se = 0;

/******* 电机速度测量 ********/
void Speed_Measure(void)
{
	int32 Pulses_l, Pulses_r;
	static int32 Speed_Last = 0;
	static int32 Crazy_Count = 0;
	
	/******* 电机速度相关控制 ********/
	Pulses_l = ctimer_count_read(SPEED_PIN_l);//读取采集到的编码器脉冲数
	Pulses_r = ctimer_count_read(SPEED_PIN_r);
	ctimer_count_clean(SPEED_PIN_l);//计数器清零
	ctimer_count_clean(SPEED_PIN_r);
	//采集方向信息
	if(gpio_get(SPEED_DIR_l)) Pulses_l = Pulses_l;// 得到转速
	else                      Pulses_l = -Pulses_l;
	if(gpio_get(SPEED_DIR_r)) Pulses_r = -Pulses_r;// 得到转速
	else                      Pulses_r = Pulses_r;
	
	MOTOR_Speed = (Pulses_l + Pulses_r) / 2;
	
	if(hangchen_flag)
	{
		hangchen += MOTOR_Speed;
	}

	MOTOR_Acc = MOTOR_Speed - MOTOR_Speed_Last;	// 计算加速度
	if (MOTOR_Acc > 100)
	{
		Crazy = 1;	// 疯转
	}
	if (MOTOR_Speed > Speed_Set + 200)
	{
		Crazy = 2;	// 疯转
	}
	if (MOTOR_Speed < -250)
	{
		Crazy = -1;	// 倒转
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
		MOTOR_Speed_Last = MOTOR_Speed;	// 更新速度
	}
	else
	{
		MOTOR_Speed = MOTOR_Speed*0.5 + MOTOR_Speed_Last*0.5;
		MOTOR_Speed_Last = MOTOR_Speed;	// 更新速度
	}
	/******* 电机速度相关控制结束 ********/
	
	
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
	
	/******* 电机疯转特殊处理 ********/
	if (Crazy > 0)
	{
		Speed_Now = MOTOR_Speed_Last;			//疯转，使用上次速度作为当前实际速度
	}
	else
	{
		Speed_Now = MOTOR_Speed;	//计算车子实际速度
	}
	
	Speed_Now = Speed_Now *0.9 + Speed_Last * 0.1;
	Speed_Last = Speed_Now;
}
/* 
	0：200 速度约
	1：230 速度约
	2：250 速度约
	3：260 速度约
	4：270 速度约
	5：275 速度约
	6：300 速度约
*/
void Speed_Control(void)
{
		switch(Mode_Set)
		{
		case 0:		Speed_Set = 200;
				 	break;
		case 1:		Speed_Set = 230;
				 	break;
		case 2:		Speed_Set = 250;	// 默认初始速度
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
	
	/**************差速*************/
	Duty_l = Duty - ChaSu_Ero * S_D5_MOTOR[Mode_Set];
	Duty_r = Duty + ChaSu_Ero * S_D5_MOTOR[Mode_Set];
	
	if (Duty_l >= 0)
	{
		Duty_l = range_protect(Duty_l, 0, MOTOR_MAX);	// 限幅保护
		ctimer_pwm_duty(TIMER1_PWMCH0_A18, 0);		//l电机正转
		ctimer_pwm_duty(TIMER1_PWMCH1_A20, Duty_l);	//占空比最大10000！！！！
	}
	else
	{
		Duty_l = range_protect(-Duty_l, 0, MOTOR_MAX);	// 限幅保护
		ctimer_pwm_duty(TIMER1_PWMCH1_A20, 0);    	//l电机反转
		ctimer_pwm_duty(TIMER1_PWMCH0_A18, Duty_l);	//占空比最大10000！！！
	}
	if (Duty_r >= 0)
	{
		Duty_r = range_protect(Duty_r, 0, MOTOR_MAX);	// 限幅保护
		ctimer_pwm_duty(TIMER2_PWMCH0_B5, 0);		//r电机正转
		ctimer_pwm_duty(TIMER2_PWMCH1_B4, Duty_r);	//占空比最大10000！！！！
	}
	else
	{
		Duty_r = range_protect(-Duty_r, 0, MOTOR_MAX);	// 限幅保护
		ctimer_pwm_duty(TIMER2_PWMCH1_B4, 0);		//r电机反转
		ctimer_pwm_duty(TIMER2_PWMCH0_B5, Duty_r);	//占空比最大10000！！！！
	}
}

void Start_Control(void)
{
	static uint8 Start_OK = 0;
	static uint16 No_Start_Line_Count = 0;
	
	//接收数据
	nrf_rece_packet(nrf_rx_buff);
	if(Ready_Stop)
	{
		if(se == 0)
		{
			nrf_tx_buff[0] = 1;  
			nrf_tx_buff[1] = 'k';
			//发送数据
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
		/** 起跑阶段检测 **/
		if (!Starting_Line_Flag && !Start_OK)	//没有检测到起跑线且未成功出发
		{
			No_Start_Line_Count++;			//相当于延时约5s
			if (No_Start_Line_Count >= 300)	//连续200次没有检测到起跑线，避免起跑时的光线异常干扰
			{
				No_Start_Line_Count = 0;
				Start_OK = 1;	//出发成功
			}
		}
		/** 结束阶段检测 **/
		if (Start_OK && Starting_Line_Flag)	//成功起跑后检测到终点线
		{
			MOTOR_Control(-3800);
			Ready_Stop = 1;	//准备停车
			Stop_Flag = 1;
			Run_Flag = 0;			//停车
		}
	}
}

/******** 限幅保护 *********/
int32 range_protect(int32 duty, int32 min, int32 max)//限幅保护
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
