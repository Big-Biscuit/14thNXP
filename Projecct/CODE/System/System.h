#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "common.h"
#include "headfile.h"
#include "PID.h"

//定义ADC通道与引脚
#define ADC1    ADC_CH0_A10 //通道0  A10引脚
#define ADC2    ADC_CH3_A15 //通道3  A15引脚
#define ADC3    ADC_CH4_A16 //通道4  A16引脚
#define ADC4    ADC_CH5_A31 //通道5  A31引脚

extern uint16 adc1_result;
extern uint16 adc2_result;
extern uint16 adc3_result;
extern uint16 adc4_result;

//编码器
//定义脉冲引脚
#define SPEED_PIN_l  TIMER3_COUNT0_A4
#define SPEED_PIN_r  TIMER0_COUNT1_A2
//定义方向引脚
#define SPEED_DIR_l  A5
#define SPEED_DIR_r  A3

//定义按键引脚
#define KEY3 B0
#define KEY4 B22

//拨码开关状态变量
extern uint8 sw1_status;
extern uint8 sw2_status;

//开关状态变量
extern uint8 key1_status;
extern uint8 key2_status;
extern uint8 key3_status;
extern uint8 key4_status;

//上一次开关状态变量
extern uint8 key1_last_status;
extern uint8 key2_last_status;
extern uint8 key3_last_status;
extern uint8 key4_last_status;

//开关标志位
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;

//定义激光测距引脚
#define JIG B11

//激光测距状态变量
extern uint8 jig_status;

extern uint8 datbuf;
extern uint8 chaoshengbo_flag;
extern uint8 highbuf;//超声波高位
extern uint8 lowbuf;//超声波低位

extern uint16 duty;//舵机占空比 600-880

void System_Init(void);		 //所有模块初始化

#endif
