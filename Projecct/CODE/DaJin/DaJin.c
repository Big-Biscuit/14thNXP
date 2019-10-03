#include "DaJin.h"

uint8 HistGram[256] = {0};//灰度直方图

/*************************************************************** 
* 函数名称：void GetHistGram(uint8 *Image) 
* 功能说明：获取图像的灰度信息 
* 参数说明： 
* 函数返回：void 
* 修改时间：2019年3月29日 
* 备 注： 
***************************************************************/ 
void GetHistGram(uint8 *Image) 
{ 
	int X; 
	for (X = 0; X < 256; X++) 
	{ 
		HistGram[X] = 0; //初始化灰度直方图 
	} 
	for (X = (MT9V032_H / 3) * MT9V032_W; X < MT9V032_H * MT9V032_W; X++) 
	{ 
		HistGram[*(Image+X)]++; //统计每个灰度值的个数信息 
	} 
} 
/*************************************************************** 
* 函数名称：uint8 OSTUThreshold() 
* 功能说明：大津法获取图像阈值 
* 参数说明： 
* 函数返回：uint8 阈值 
* 修改时间：2019年3月29日 
* 备 注： 
***************************************************************/ 
uint8 OSTUThreshold(void) 
{ 
	int16 Y; 
	uint32 Amount = 0;//像素总数 
	uint32 PixelBack = 0;//前景像素点数 
	uint32 PixelIntegralBack = 0;//前景灰度值 
	uint32 PixelIntegral = 0;//灰度值总数 
	int32 PixelIntegralFore = 0;//背景灰度值 
	int32 PixelFore = 0;//背景像素点数 
	double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差; 
	int16 MinValue, MaxValue; 
	uint8 Threshold = 0;
	
	for(MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);        //获取最小灰度的值
	for(MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--); //获取最大灰度的值

	if (MaxValue == MinValue) 
	{
		return MaxValue;          //图像中只有一个颜色    
	}
	if (MinValue + 1 == MaxValue) 
	{
		return MinValue;      //图像中只有二个颜色
	}

	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		Amount += HistGram[Y];        //像素总数
	}

	PixelIntegral = 0;
	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		PixelIntegral += HistGram[Y] * Y;//灰度值总数
	}
	SigmaB = -1;
	for (Y = MinValue; Y < MaxValue; Y++)
	{
		PixelBack = PixelBack + HistGram[Y];    //前景像素点数
		PixelFore = Amount - PixelBack;         //背景像素点数
		OmegaBack = (double)PixelBack / Amount;//前景像素百分比
		OmegaFore = (double)PixelFore / Amount;//背景像素百分比
		PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
		MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
		MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
		if (Sigma > SigmaB)//遍历最大的类间方差g
		{
			SigmaB = Sigma;
			Threshold = Y;
		}
	}
	return Threshold;
}
