/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��179029047
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		isr
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴LPC546XX_config.h�ļ��ڰ汾�궨��
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 ********************************************************************************************************************/

#include "headfile.h"
#include "isr.h"
#include "Handle.h"
#include "Control.h"
#include "System.h"
#include "DaJin.h"
#include "PID.h"
#include "siyuansu.h"

uint8 i = 0;
uint8 j = 0;
uint8 Flag_10ms = 0;
uint8 yuzhi = 0;//��ֵ��

extern uint8 datbuf;
extern uint8 chaoshengbo_flag;
extern uint8 highbuf;//��������λ
extern uint8 lowbuf;//��������λ

void GINT0_DriverIRQHandler(void)
{
    if(1 == GINT0_FLAG)
    { 
        CLEAR_GINT0_FLAG;
    }
}

void FLEXCOMM0_DriverIRQHandler(void)
{
    vuint32 flag;
    flag = UART0_FIFO_FLAG;
    
    if(flag & USART_FIFOINTSTAT_RXLVL_MASK)//����FIFO�ﵽ�趨ˮƽ����Ĭ���趨ˮƽ ������FIFO��һ�����ݵ�ʱ�򴥷��жϣ�
    {
        uart_query(USART_0,&datbuf);		//��ȡ����
    }
    
    if(flag & USART_FIFOINTSTAT_TXLVL_MASK)//����FIFO�ﵽ�趨ˮƽ����Ĭ���趨ˮƽ ������FIFOΪ�յ�ʱ�򴥷��жϣ�
    {
        
    }
    
    if(flag & USART_FIFOINTSTAT_RXERR_MASK)//����FIFO����
    {
        USART0->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
        USART0->FIFOCFG  |= USART_FIFOCFG_EMPTYRX_MASK;//���RX FIFO
    }
}

//void FLEXCOMM1_DriverIRQHandler(void)
//{
//    vuint32 flag;
//    flag = UART1_FIFO_FLAG;
//    
//    if(flag & USART_FIFOINTSTAT_RXLVL_MASK)//����FIFO�ﵽ�趨ˮƽ����Ĭ���趨ˮƽ ������FIFO��һ�����ݵ�ʱ�򴥷��жϣ�
//    {
//		if(chaoshengbo_flag == 0)
//		{
//			uart_query(USART_1,&highbuf);		//��ȡ����
//			chaoshengbo_flag = 1;
//		}
//		else
//		{
//			uart_query(USART_1,&lowbuf);		//��ȡ����
//			chaoshengbo_flag = 0;
//		}
//    }
//    
//    if(flag & USART_FIFOINTSTAT_TXLVL_MASK)//����FIFO�ﵽ�趨ˮƽ����Ĭ���趨ˮƽ ������FIFOΪ�յ�ʱ�򴥷��жϣ�
//    {
//        
//    }
//    
//    if(flag & USART_FIFOINTSTAT_RXERR_MASK)//����FIFO����
//    {
//        USART1->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
//        USART1->FIFOCFG  |= USART_FIFOCFG_EMPTYRX_MASK;//���RX FIFO
//    }
//}

void RIT_DriverIRQHandler(void)
{
	PIT_FLAG_CLEAR;
	
//	Flag_10ms = 1;
	i++;
	
	if(i > 12)
	{
		i = 1;
	}
	
	Speed_Measure();//����10ms
	if((i == 1) || (i == 4) || (i == 7) || (i == 10))//30ms
	{
		if(Run_Flag)
		{
			MOTOR_Duty += PID_Cascade(&MOTOR_PID, MOTOR_Speed, Speed_Set);//ʹ�ô�������ʽPID���е���
//			MOTOR_Duty += PID_Realize(&MOTOR_PID, MOTOR_Speed, Speed_Set);
			MOTOR_Duty = range_protect(MOTOR_Duty, -9000, 9000);	//�޷�����
			
			MOTOR_Control(MOTOR_Duty);	// �������ҵ��
		}
		else
		{
			if (Stop_Flag)
			{
				MOTOR_Duty = 0;
				if (Speed_Now > 20)
				{
					MOTOR_Control(-5000);//-3800
				}
				else if(Speed_Now < -20)
				{
					MOTOR_Control(1000);//3800
				}
				else
				{
					MOTOR_Control(0);
				}
			}
			else
			{
				MOTOR_Control(0);
			}
		}
	}
	
	//�ɼ�ADC����10ms
	adc2_result = adc_convert(ADC2,ADC_10BIT);
	adc3_result = adc_convert(ADC3,ADC_10BIT);
	
	if((i == 3) || (i == 6) || (i == 9) || (i == 12))//30ms��ȡһ����ֵ
	{
		GetHistGram(image[0]);//��ȡͼ��ĻҶ���Ϣ
		yuzhi = OSTUThreshold();//��򷨻�ȡͼ����ֵ
		if(yuzhi < 60)
		{
			yuzhi = 60;
		}
	}
	
	if((i == 2) || (i == 5) || (i == 8) || (i == 11))//30ms��ȡһ��
	{
		Get_Attitude();//��̬����30ms
	}
}

void PIN_INT7_DriverIRQHandler(void)
{
    mt9v032_vsync();        //����糡�жϴ��룬��ʹ��������ʱ��ִ�иô���
//  ov7725_vsync();       	//С��糡�жϴ��룬��ʹ��С����ʱ��ִ�иô���
}

void DMA0_DriverIRQHandler(void)
{
    if(READ_DMA_FLAG(MT9V032_DMA_CH))
    {
//      CLEAR_DMA_FLAG(MT9V032_DMA_CH);
        mt9v032_dma();      //�����dma�жϴ��룬��ʹ��������ʱ��ִ�иô���
//      ov7725_dma();       //С���dma�жϴ��룬��ʹ��С����ʱ��ִ�иô���
    }
}

void FLEXCOMM5_DriverIRQHandler(void)
{
    vuint32 flag;
    flag = UART5_FIFO_FLAG;
    
    if(flag & USART_FIFOINTSTAT_RXLVL_MASK)//����FIFO�ﵽ�趨ˮƽ����Ĭ���趨ˮƽ ������FIFO��һ�����ݵ�ʱ�򴥷��жϣ�
    {
        mt9v032_cof_uart_interrupt();
    }
    
    if(flag & USART_FIFOINTSTAT_RXERR_MASK)//����FIFO����
    { 
        USART5->FIFOCFG  |= USART_FIFOCFG_EMPTYRX_MASK;//���RX FIFO
        USART5->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
    }
}

uint32 tt;
void MRT0_DriverIRQHandler(void)
{
    if(MRT_FLAG_READ(MRT_CH0))
    {
        MRT_FLAG_CLR(MRT_CH0);
    }
    
    if(MRT_FLAG_READ(MRT_CH1))
    {
        MRT_FLAG_CLR(MRT_CH1);
        tt++;
    }
    
    if(MRT_FLAG_READ(MRT_CH2))
    {
        MRT_FLAG_CLR(MRT_CH2);
    }
    
    if(MRT_FLAG_READ(MRT_CH3))
    {
        MRT_FLAG_CLR(MRT_CH3);
    }
}

/*
�жϺ������ƣ��������ö�Ӧ���ܵ��жϺ���
Sample usage:��ǰ���������ڶ�ʱ�� ͨ��0���ж�
void PIT_CH0_IRQHandler(void)
{
    ;
}
�ǵý����жϺ������־λ

WDT_BOD_DriverIRQHandler
DMA0_DriverIRQHandler
GINT0_DriverIRQHandler
GINT1_DriverIRQHandler
PIN_INT0_DriverIRQHandler
PIN_INT1_DriverIRQHandler
PIN_INT2_DriverIRQHandler
PIN_INT3_DriverIRQHandler
UTICK0_DriverIRQHandler
MRT0_DriverIRQHandler
CTIMER0_DriverIRQHandler
CTIMER1_DriverIRQHandler
SCT0_DriverIRQHandler
CTIMER3_DriverIRQHandler
FLEXCOMM0_DriverIRQHandler
FLEXCOMM1_DriverIRQHandler
FLEXCOMM2_DriverIRQHandler
FLEXCOMM3_DriverIRQHandler
FLEXCOMM4_DriverIRQHandler
FLEXCOMM5_DriverIRQHandler
FLEXCOMM6_DriverIRQHandler
FLEXCOMM7_DriverIRQHandler
ADC0_SEQA_DriverIRQHandler
ADC0_SEQB_DriverIRQHandler
ADC0_THCMP_DriverIRQHandler
DMIC0_DriverIRQHandler
HWVAD0_DriverIRQHandler
USB0_NEEDCLK_DriverIRQHandler
USB0_DriverIRQHandler
RTC_DriverIRQHandler
Reserved46_DriverIRQHandler
Reserved47_DriverIRQHandler
PIN_INT4_DriverIRQHandler
PIN_INT5_DriverIRQHandler
PIN_INT6_DriverIRQHandler
PIN_INT7_DriverIRQHandler
CTIMER2_DriverIRQHandler
CTIMER4_DriverIRQHandler
RIT_DriverIRQHandler
SPIFI0_DriverIRQHandler
FLEXCOMM8_DriverIRQHandler
FLEXCOMM9_DriverIRQHandler
SDIO_DriverIRQHandler
CAN0_IRQ0_DriverIRQHandler
CAN0_IRQ1_DriverIRQHandler
CAN1_IRQ0_DriverIRQHandler
CAN1_IRQ1_DriverIRQHandler
USB1_DriverIRQHandler
USB1_NEEDCLK_DriverIRQHandler
ETHERNET_DriverIRQHandler
ETHERNET_PMT_DriverIRQHandler
ETHERNET_MACLP_DriverIRQHandler
EEPROM_DriverIRQHandler
LCD_DriverIRQHandler
SHA_DriverIRQHandler
SMARTCARD0_DriverIRQHandler
SMARTCARD1_DriverIRQHandler
*/



