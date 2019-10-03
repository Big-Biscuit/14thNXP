#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "common.h"
#include "headfile.h"
#include "PID.h"

//����ADCͨ��������
#define ADC1    ADC_CH0_A10 //ͨ��0  A10����
#define ADC2    ADC_CH3_A15 //ͨ��3  A15����
#define ADC3    ADC_CH4_A16 //ͨ��4  A16����
#define ADC4    ADC_CH5_A31 //ͨ��5  A31����

extern uint16 adc1_result;
extern uint16 adc2_result;
extern uint16 adc3_result;
extern uint16 adc4_result;

//������
//������������
#define SPEED_PIN_l  TIMER3_COUNT0_A4
#define SPEED_PIN_r  TIMER0_COUNT1_A2
//���巽������
#define SPEED_DIR_l  A5
#define SPEED_DIR_r  A3

//���尴������
#define KEY3 B0
#define KEY4 B22

//���뿪��״̬����
extern uint8 sw1_status;
extern uint8 sw2_status;

//����״̬����
extern uint8 key1_status;
extern uint8 key2_status;
extern uint8 key3_status;
extern uint8 key4_status;

//��һ�ο���״̬����
extern uint8 key1_last_status;
extern uint8 key2_last_status;
extern uint8 key3_last_status;
extern uint8 key4_last_status;

//���ر�־λ
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;

//���弤��������
#define JIG B11

//������״̬����
extern uint8 jig_status;

extern uint8 datbuf;
extern uint8 chaoshengbo_flag;
extern uint8 highbuf;//��������λ
extern uint8 lowbuf;//��������λ

extern uint16 duty;//���ռ�ձ� 600-880

void System_Init(void);		 //����ģ���ʼ��

#endif
