/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		小钻风摄像头
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看LPC546XX_config.h文件内版本宏定义
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 * @note		
					接线定义：
					------------------------------------ 
						模块管脚            单片机管脚
						SDA    			    A9
						SCL         	    A8
						场中断(VSY)         A0
						行中断(HREF)		未使用，因此不接
						像素中断(PCLK)      A1        
						数据口(D0-D7)		B24-B31
					------------------------------------ 
	
					默认分辨率是            160*120
					默认FPS                 50帧
 ********************************************************************************************************************/


#include "common.h"
#include "LPC546XX_sct.h"
#include "LPC546XX_dma.h"
#include "LPC546XX_gpio.h"
#include "LPC546XX_uart.h"
#include "LPC546XX_systick.h"
#include "LPC546XX_pint.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_7725.h"



uint8 image_bin[OV7725_H][OV7725_W/8];                      //定义存储接收图像的数组
uint8 image_dec[OV7725_H][OV7725_W];

uint8 ov7725_idcode = 0;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头内部寄存器初始化(内部使用，用户无需调用)
//  @param      NULL
//  @return     uint8			返回0则出错，返回1则成功
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_reg_init(void)
{
    
    simiic_write_reg ( OV7725_DEV_ADD, OV7725_COM7, 0x80 );	//复位摄像头
    systick_delay_ms(50);
	ov7725_idcode = simiic_read_reg( OV7725_DEV_ADD, OV7725_VER ,SCCB);
    if( ov7725_idcode != OV7725_ID )    return 0;			//校验摄像头ID号

    //ID号确认无误   然后配置寄存器
    simiic_write_reg(OV7725_DEV_ADD, OV7725_COM4         , 0xC1);//  
    simiic_write_reg(OV7725_DEV_ADD, OV7725_CLKRC        , 0x00);//
    simiic_write_reg(OV7725_DEV_ADD, OV7725_COM2         , 0x03);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_COM3         , 0xD0);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_COM7         , 0x40);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_COM8         , 0xCE);   //0xCE:关闭自动曝光  0xCF：开启自动曝光
    simiic_write_reg(OV7725_DEV_ADD, OV7725_HSTART       , 0x3F);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_HSIZE        , 0x50);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_VSTRT        , 0x03);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_VSIZE        , 0x78);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_HREF         , 0x00);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_SCAL0        , 0x0A);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_AWB_Ctrl0    , 0xE0);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_DSPAuto      , 0xff);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl2    , 0x0C);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl3    , 0x00);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_DSP_Ctrl4    , 0x00);
    
    if(OV7725_W == 80)              simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize    , 0x14);
    else if(OV7725_W == 160)        simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize    , 0x28);
    else if(OV7725_W == 240)        simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize    , 0x3c);
    else if(OV7725_W == 320)        simiic_write_reg(OV7725_DEV_ADD, OV7725_HOutSize    , 0x50);
    
    if(OV7725_H == 60)              simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize    , 0x1E);
    else if(OV7725_H == 120)        simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize    , 0x3c);
    else if(OV7725_H == 180)        simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize    , 0x5a);
    else if(OV7725_H == 240)        simiic_write_reg(OV7725_DEV_ADD, OV7725_VOutSize    , 0x78);
    
    simiic_write_reg(OV7725_DEV_ADD, OV7725_REG28        , 0x01);//
    simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCH        , 0x10);//
    simiic_write_reg(OV7725_DEV_ADD, OV7725_EXHCL        , 0x1F);//
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM1         , 0x0c);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM2         , 0x16);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM3         , 0x2a);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM4         , 0x4e);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM5         , 0x61);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM6         , 0x6f);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM7         , 0x7b);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM8         , 0x86);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM9         , 0x8e);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM10        , 0x97);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM11        , 0xa4);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM12        , 0xaf);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM13        , 0xc5);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM14        , 0xd7);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_GAM15        , 0xe8);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_SLOP         , 0x20);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_RADI      , 0x00);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEF      , 0x13);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_XC        , 0x08);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFB     , 0x14);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_COEFR     , 0x17);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_LC_CTR       , 0x05);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_BDBase       , 0x99);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_BDMStep      , 0x03);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_SDE          , 0x04);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_BRIGHT       , 0x00);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_CNST         , 0x60);//阈值，大亮，小暗
    simiic_write_reg(OV7725_DEV_ADD, OV7725_SIGN         , 0x06);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ0       , 0x11);
    simiic_write_reg(OV7725_DEV_ADD, OV7725_UVADJ1       , 0x02);
    return 1;
}





//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头采集程序初始化(内部使用，用户无需调用)
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ov7725_port_init(void)
{
    //初始化SCT模块用于接收PCLK信号并触发DMA
    sct_camera_dma(OV7725_PCLK_SCT, OV7725_PCLK, FALLING);
    dam_init_linked(OV7725_DMA_CH, (void *)&OV7725_DATAPORT, (void *)image_bin[0], OV7725_DMA_NUM);
    pint_init(OV7725_VSYNC_PINT,OV7725_VSYNC_PIN,FALLING);
    
    set_irq_priority(OV7725_VSYNC_IRQN,0);  //设置场中断为最高优先级
    set_irq_priority(DMA0_IRQn,1);          //设置DMA中断为第二优先级
    
    enable_irq(OV7725_VSYNC_IRQN);
    enable_irq(DMA0_IRQn);
    sct_start();
    
	EnableInterrupts;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头初始化(调用之后设置好相关中断函数即可采集图像)
//  @param      NULL
//  @return     0
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 ov7725_init(void)
{
	simiic_init();
	ov7725_reg_init();                                          //摄像头寄存器配置
    ov7725_port_init();                                         //摄像头中断引脚及DMA配置
    return 0;
}




uint8   ov7725_finish_flag = 0;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头场中断
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:					在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ov7725_vsync(void)
{
    PINT_IST_FLAG_CLEAR(OV7725_VSYNC_PINT);    //清除标志位
    if(SCT_DMA0REQUEST_DRQ0_MASK & SCT0->DMA0REQUEST)   DMA_ABORT(OV7725_DMA_CH);
    dma_reload_linked(OV7725_DMA_CH, (void *)image_bin[0], OV7725_DMA_NUM);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头DMA完成中断
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:					在isr.c里面先创建对应的中断函数，然后调用该函数(之后别忘记清除中断标志位)
//-------------------------------------------------------------------------------------------------------------------
void ov7725_dma(void)
{
    CLEAR_DMA_FLAG(OV7725_DMA_CH);
    if(!DMA_STATUS(OV7725_DMA_CH))//检查当前的DMA链接传输是否完毕
	{
		ov7725_finish_flag = 1;		//采集完成标志位
	}

}




//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头数据解压函数
//  @param      *data1				源地址
//  @param      *data2				目的地址
//  @return     void
//  @since      v1.0
//  Sample usage:					Image_Decompression(da1,dat2[0]);//将一维数组dat1的内容解压到二维数组dat2里.
//-------------------------------------------------------------------------------------------------------------------
void image_decompression(uint8 *data1,uint8 *data2)
{
    uint8  temp[2] = {0,1};
    uint16 lenth = OV7725_SIZE;
    uint8  i = 8;

        
    while(lenth--)
    {
        i = 8;
        while(i--)
        {
            *data2++ = temp[(*data1 >> i) & 0x01];
        }
        data1++;
    }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      小钻风摄像头未解压图像发送至上位机查看图像
//  @param      *imgaddr			压缩图像数据地址
//  @param      *imgsize			图像大小(直接填写OV7725_SIZE)
//  @return     void
//  @since      v1.0
//  Sample usage:					调用该函数前请先初始化uart2
//-------------------------------------------------------------------------------------------------------------------
void seekfree_sendimg_7725(UARTN_enum uartn)
{
    uart_putchar(uartn, 0x00);uart_putchar(uartn, 0xff);uart_putchar(uartn, 0x01);uart_putchar(uartn, 0x01);//发送四个字节命令
    uart_putbuff(uartn, (uint8_t *)image_bin[0], OV7725_SIZE);   //再发送图像
}




