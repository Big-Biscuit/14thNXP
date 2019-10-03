#include "System.h"

uint8 datbuf = 0;//uart数据
uint8 chaoshengbo_flag = 0;
uint8 highbuf = 0;//超声波高位
uint8 lowbuf = 0;//超声波低位

uint16 adc1_result = 0;
uint16 adc2_result = 0;
uint16 adc3_result = 0;
uint16 adc4_result = 0;

//拨码开关状态变量
uint8 sw1_status;
uint8 sw2_status;

//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;

//开关标志位
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

//激光测距状态变量
uint8 jig_status = 1;

void System_Init(void)
{
  	/*********************** 舵机 初始化 ***********************/  
	//舵机居中   1.5ms/20ms * 10000（10000是PWM的满占空比时候的值）
	//初始化定时器4  使用A6引脚  初始化频率为50hz 舵机居中
    ctimer_pwm_init(TIMER4_PWMCH0_A6, 50, S_D5_MID);

	/************************ 电机 初始化  ***********************/
	//占空比精度可以在LPC546XX_ctimer.h文件内找到TIMER1_PWM_DUTY_MAX和TIMER2_PWM_DUTY_MAX宏定义进行设置
    ctimer_pwm_init(TIMER1_PWMCH0_A18, 17000, 0); //初始化定时器1  使用A18引脚  初始化频率为17Khz l
    ctimer_pwm_init(TIMER1_PWMCH1_A20, 17000, 0); //初始化定时器1  使用A20引脚  初始化频率为17Khz
	ctimer_pwm_init(TIMER2_PWMCH1_B4, 17000, 0); //初始化定时器1  使用B4引脚  初始化频率为17Khz   r
    ctimer_pwm_init(TIMER2_PWMCH0_B5, 17000, 0); //初始化定时器1  使用B5引脚  初始化频率为17Khz
	
	/************************** 6050初始化  ***************************/
	mpu6050_init_hardware();//硬件iic
	
//	simiic_init();
//	mpu6050_init();//软件iic
	
	/************************** 速度初始化  ***************************/
	Speed_Control();	//速度初始化

  	/************************ LCD 液晶屏 初始化  ***********************/
	//显示模式设置为0  竖屏模式 
    //显示模式在tft的h文件内的TFT_DISPLAY_DIR宏定义设置
    lcd_init();     //初始化TFT屏幕

	/************************ 摄像头 初始化  ***********************/
//	ov7725_init();
	camera_init();  //初始化摄像头

	/************************ 编码器 初始化  ***********************/   
	//初始化定时器0  1通道 使用引脚A2,4  为计数模式
	ctimer_count_init(SPEED_PIN_l);
	ctimer_count_init(SPEED_PIN_r);
	//初始化方向引脚IO 且上拉A3,5
    gpio_init(SPEED_DIR_l,GPI,0,PULLUP);
	gpio_init(SPEED_DIR_r,GPI,0,PULLUP);

	/******************** 定时器 初始化  *********************/
	//初始化周期定时器  时间为10ms
    //中断函数在isr.c文件  函数名为RIT_DriverIRQHandler
	pit_init_ms(10);//电机控制等
	enable_irq(RIT_IRQn);

	/************************** 串口 初始化  ***********************/
//	uart_init(USART_0,9600,UART0_TX_A25,UART0_RX_A24);       // 初始化串口0 波特率9600 发送引脚使用P025 接收引脚使用P024
//	uart_rx_irq(USART_0,1);       // 打开串口0接收中断
	//中断函数在isr.c文件内 函数名为FLEXCOMM0_DriverIRQHandler
//	uart_init(USART_1,9600,UART1_TX_A10,UART1_RX_B10);       // 初始化串口1 波特率9600 发送引脚使用P010 接收引脚使用P110
//	uart_rx_irq(USART_1,1);       // 打开串口1接收中断
    //中断函数在isr.c文件内 函数名为FLEXCOMM1_DriverIRQHandler
  
	/**************************按键初始化***********************/
    gpio_init(KEY3,GPI,0,PULLUP);
    gpio_init(KEY4,GPI,0,PULLUP);
	
	/**********************初始化蜂鸣器引脚***********************/
	gpio_init(B18,GPO,0,NOPULL);
	
	/********************** ADC初始化 ***********************/
//	adc_init(ADC1);
	adc_init(ADC2);//l
	adc_init(ADC3);//r
//	adc_init(ADC4);

	/********************** PID参数初始化 ***********************/
	PlacePID_Init(&S_D5_PID);//舵机PID参数初始化   参数还未调节
	IncPID_Init(&MOTOR_PID);	//电机PID参数初始化
	
	/********************** NRF初始化 ***********************/
	nrf_init();
	
	/********************** 激光测距初始化 ***********************/
	gpio_init(JIG,GPI,0,PULLUP);
}

