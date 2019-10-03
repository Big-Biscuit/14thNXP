#include "DaJin.h"

uint8 HistGram[256] = {0};//�Ҷ�ֱ��ͼ

/*************************************************************** 
* �������ƣ�void GetHistGram(uint8 *Image) 
* ����˵������ȡͼ��ĻҶ���Ϣ 
* ����˵���� 
* �������أ�void 
* �޸�ʱ�䣺2019��3��29�� 
* �� ע�� 
***************************************************************/ 
void GetHistGram(uint8 *Image) 
{ 
	int X; 
	for (X = 0; X < 256; X++) 
	{ 
		HistGram[X] = 0; //��ʼ���Ҷ�ֱ��ͼ 
	} 
	for (X = (MT9V032_H / 3) * MT9V032_W; X < MT9V032_H * MT9V032_W; X++) 
	{ 
		HistGram[*(Image+X)]++; //ͳ��ÿ���Ҷ�ֵ�ĸ�����Ϣ 
	} 
} 
/*************************************************************** 
* �������ƣ�uint8 OSTUThreshold() 
* ����˵������򷨻�ȡͼ����ֵ 
* ����˵���� 
* �������أ�uint8 ��ֵ 
* �޸�ʱ�䣺2019��3��29�� 
* �� ע�� 
***************************************************************/ 
uint8 OSTUThreshold(void) 
{ 
	int16 Y; 
	uint32 Amount = 0;//�������� 
	uint32 PixelBack = 0;//ǰ�����ص��� 
	uint32 PixelIntegralBack = 0;//ǰ���Ҷ�ֵ 
	uint32 PixelIntegral = 0;//�Ҷ�ֵ���� 
	int32 PixelIntegralFore = 0;//�����Ҷ�ֵ 
	int32 PixelFore = 0;//�������ص��� 
	double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // ��䷽��; 
	int16 MinValue, MaxValue; 
	uint8 Threshold = 0;
	
	for(MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);        //��ȡ��С�Ҷȵ�ֵ
	for(MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--); //��ȡ���Ҷȵ�ֵ

	if (MaxValue == MinValue) 
	{
		return MaxValue;          //ͼ����ֻ��һ����ɫ    
	}
	if (MinValue + 1 == MaxValue) 
	{
		return MinValue;      //ͼ����ֻ�ж�����ɫ
	}

	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		Amount += HistGram[Y];        //��������
	}

	PixelIntegral = 0;
	for (Y = MinValue; Y <= MaxValue; Y++)
	{
		PixelIntegral += HistGram[Y] * Y;//�Ҷ�ֵ����
	}
	SigmaB = -1;
	for (Y = MinValue; Y < MaxValue; Y++)
	{
		PixelBack = PixelBack + HistGram[Y];    //ǰ�����ص���
		PixelFore = Amount - PixelBack;         //�������ص���
		OmegaBack = (double)PixelBack / Amount;//ǰ�����ذٷֱ�
		OmegaFore = (double)PixelFore / Amount;//�������ذٷֱ�
		PixelIntegralBack += HistGram[Y] * Y;  //ǰ���Ҷ�ֵ
		PixelIntegralFore = PixelIntegral - PixelIntegralBack;//�����Ҷ�ֵ
		MicroBack = (double)PixelIntegralBack / PixelBack;//ǰ���ҶȰٷֱ�
		MicroFore = (double)PixelIntegralFore / PixelFore;//�����ҶȰٷֱ�
		Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
		if (Sigma > SigmaB)//����������䷽��g
		{
			SigmaB = Sigma;
			Threshold = Y;
		}
	}
	return Threshold;
}
