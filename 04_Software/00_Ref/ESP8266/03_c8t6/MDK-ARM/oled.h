#ifndef OLED_TASK_H
#define OLED_TASK_H
#include "main.h"


void oled_show(void const *pvParameters);
void OLED_WR_CMD(uint8_t cmd);
void OLED_Init(void);
void OLED_WR_DATA(uint8_t data);
void OLED_On(unsigned char fill_Data);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_HorizontalShift(uint8_t direction);
char* itoa(int num,char* str,int radix);
void OLED_ShowFNum(unsigned char Line, unsigned char Column, float Number, uint8_t Length,uint8_t Flength);
void curve(float str)		;
void function_1(void)		;
#endif