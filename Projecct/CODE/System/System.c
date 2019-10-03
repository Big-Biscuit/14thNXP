#include "System.h"

uint8 datbuf = 0;//uart����
uint8 chaoshengbo_flag = 0;
uint8 highbuf = 0;//��������λ
uint8 lowbuf = 0;//��������λ

uint16 adc1_result = 0;
uint16 adc2_result = 0;
uint16 adc3_result = 0;
uint16 adc4_result = 0;

//���뿪��״̬����
uint8 sw1_status;
uint8 sw2_status;

//����״̬����
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//��һ�ο���״̬����
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

//���ر�־λ
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

//������״̬����
uint8 jig_status = 1;

void System_Init(void)
{
  	/*********************** ��� ��ʼ�� ***********************/  
	//�������   1.5ms/20ms * 10000��10000��PWM����ռ�ձ�ʱ���ֵ��
	//��ʼ����ʱ��4  ʹ��A6����  ��ʼ��Ƶ��Ϊ50hz �������
    ctimer_pwm_init(TIMER4_PWMCH0_A6, 50, S_D5_MID);

	/************************ ��� ��ʼ��  ***********************/
	//ռ�ձȾ��ȿ�����LPC546XX_ctimer.h�ļ����ҵ�TIMER1_PWM_DUTY_MAX��TIMER2_PWM_DUTY_MAX�궨���������
    ctimer_pwm_init(TIMER1_PWMCH0_A18, 17000, 0); //��ʼ����ʱ��1  ʹ��A18����  ��ʼ��Ƶ��Ϊ17Khz l
    ctimer_pwm_init(TIMER1_PWMCH1_A20, 17000, 0); //��ʼ����ʱ��1  ʹ��A20����  ��ʼ��Ƶ��Ϊ17Khz
	ctimer_pwm_init(TIMER2_PWMCH1_B4, 17000, 0); //��ʼ����ʱ��1  ʹ��B4����  ��ʼ��Ƶ��Ϊ17Khz   r
    ctimer_pwm_init(TIMER2_PWMCH0_B5, 17000, 0); //��ʼ����ʱ��1  ʹ��B5����  ��ʼ��Ƶ��Ϊ17Khz
	
	/************************** 6050��ʼ��  ***************************/
	mpu6050_init_hardware();//Ӳ��iic
	
//	simiic_init();
//	mpu6050_init();//���iic
	
	/************************** �ٶȳ�ʼ��  ***************************/
	Speed_Control();	//�ٶȳ�ʼ��

  	/************************ LCD Һ���� ��ʼ��  ***********************/
	//��ʾģʽ����Ϊ0  ����ģʽ 
    //��ʾģʽ��tft��h�ļ��ڵ�TFT_DISPLAY_DIR�궨������
    lcd_init();     //��ʼ��TFT��Ļ

	/************************ ����ͷ ��ʼ��  ***********************/
//	ov7725_init();
	camera_init();  //��ʼ������ͷ

	/************************ ������ ��ʼ��  ***********************/   
	//��ʼ����ʱ��0  1ͨ�� ʹ������A2,4  Ϊ����ģʽ
	ctimer_count_init(SPEED_PIN_l);
	ctimer_count_init(SPEED_PIN_r);
	//��ʼ����������IO ������A3,5
    gpio_init(SPEED_DIR_l,GPI,0,PULLUP);
	gpio_init(SPEED_DIR_r,GPI,0,PULLUP);

	/******************** ��ʱ�� ��ʼ��  *********************/
	//��ʼ�����ڶ�ʱ��  ʱ��Ϊ10ms
    //�жϺ�����isr.c�ļ�  ������ΪRIT_DriverIRQHandler
	pit_init_ms(10);//������Ƶ�
	enable_irq(RIT_IRQn);

	/************************** ���� ��ʼ��  ***********************/
//	uart_init(USART_0,9600,UART0_TX_A25,UART0_RX_A24);       // ��ʼ������0 ������9600 ��������ʹ��P025 ��������ʹ��P024
//	uart_rx_irq(USART_0,1);       // �򿪴���0�����ж�
	//�жϺ�����isr.c�ļ��� ������ΪFLEXCOMM0_DriverIRQHandler
//	uart_init(USART_1,9600,UART1_TX_A10,UART1_RX_B10);       // ��ʼ������1 ������9600 ��������ʹ��P010 ��������ʹ��P110
//	uart_rx_irq(USART_1,1);       // �򿪴���1�����ж�
    //�жϺ�����isr.c�ļ��� ������ΪFLEXCOMM1_DriverIRQHandler
  
	/**************************������ʼ��***********************/
    gpio_init(KEY3,GPI,0,PULLUP);
    gpio_init(KEY4,GPI,0,PULLUP);
	
	/**********************��ʼ������������***********************/
	gpio_init(B18,GPO,0,NOPULL);
	
	/********************** ADC��ʼ�� ***********************/
//	adc_init(ADC1);
	adc_init(ADC2);//l
	adc_init(ADC3);//r
//	adc_init(ADC4);

	/********************** PID������ʼ�� ***********************/
	PlacePID_Init(&S_D5_PID);//���PID������ʼ��   ������δ����
	IncPID_Init(&MOTOR_PID);	//���PID������ʼ��
	
	/********************** NRF��ʼ�� ***********************/
	nrf_init();
	
	/********************** �������ʼ�� ***********************/
	gpio_init(JIG,GPI,0,PULLUP);
}

