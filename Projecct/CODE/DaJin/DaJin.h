#ifndef _DAJIN_H
#define _DAJIN_H

#include "common.h"
#include "SEEKFREE_MT9V032.h"

extern uint8 HistGram[256];//�Ҷ�ֱ��ͼ

void GetHistGram(uint8 *Image);//��ȡͼ��ĻҶ���Ϣ
uint8 OSTUThreshold(void);//��򷨻�ȡͼ����ֵ

#endif
