#ifndef _DAJIN_H
#define _DAJIN_H

#include "common.h"
#include "SEEKFREE_MT9V032.h"

extern uint8 HistGram[256];//灰度直方图

void GetHistGram(uint8 *Image);//获取图像的灰度信息
uint8 OSTUThreshold(void);//大津法获取图像阈值

#endif
