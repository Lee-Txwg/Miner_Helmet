#include "oled.h"
#include "main.h"
#include "math.h"
#include "codetab.h" 
#include "tim.h"
extern I2C_HandleTypeDef hi2c1;
volatile float  val;
volatile  float  volt,res;
uint8_t CMD_Data[]={
0xAE, 0x20, 0x10, 0xb0, 0xc8, 0x00, 0x10, 0x40, 0x81, 0xff, 0xa1,

0xa6, 0xa8, 0x3F, 0xa4, 0xd3, 0x00, 0xd5, 0xf0, 0xd9, 0x22, 0xda,

0x12,0xdb,0x20,0x8d,0x14,0xaf};
unsigned char abc[] = "detetct: ";
unsigned char abc1[] = "real: ";
unsigned char abc2[] = "v";
unsigned char abc3[] = "Resistance:";
unsigned char cval[4]={0};
unsigned char cval1[]={0};
unsigned char cval2[]={0};
unsigned char cval3[]={0};
unsigned char cvolt[4]={0};

void oled_show(void const *pvParameters)
{
	   OLED_ShowCN(0+8, 1, 0);
		 OLED_ShowCN(18+8,1,1);
		 OLED_ShowCN(36+8,1,2);
		 OLED_ShowCN(54+8,1,3);
		 OLED_ShowCN(72+8,1,4);
		 OLED_ShowCN(90+8,1,5);
	   OLED_ShowCN(18,6,6);
		 OLED_ShowCN(36,6,7);
		 OLED_ShowCN(54,6,8);
	   OLED_ShowCN(72,6,9);
		 OLED_ShowCN(90,6,10);
		 OLED_ShowCN(108,6,11);
  	 OLED_On(0x00);
		  
		for( ; ; ){
	 
		
	  OLED_ShowStr(0,0,abc, 1);
		OLED_ShowStr(0,2,abc1, 1);	
		OLED_ShowStr(0,4,abc3, 1);		
    OLED_ShowStr(75,2,abc2, 1);
			
//			OLED_On(0x00);      
			
		}
			
			
		

}

void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
void OLED_Init(void)
{
	HAL_Delay(100);

	uint8_t i = 0;
	for(i=0; i<28; i++)
	{
		OLED_WR_CMD(CMD_Data[i]);
	}
	OLED_On (0x00);
}
void OLED_On(unsigned char fill_Data)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_CMD(0xb0+i);    //设置页地址（0~7）
		OLED_WR_CMD(0x00);      //设置显示位置—列低地址
		OLED_WR_CMD(0x10);      //设置显示位置—列高地址
		for(n=0;n<128;n++)
			OLED_WR_DATA(fill_Data);
	}
}
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD((x&0x0f)|0x01);
}
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)//显示字符
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					OLED_WR_DATA(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					OLED_WR_DATA(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					OLED_WR_DATA(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
		
	}
}

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) //显示汉字
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WR_DATA(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WR_DATA(F16x16[adder]);
		adder += 1;
	}
}
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_WR_DATA(BMP[j++]);
		}
	}
}
void OLED_HorizontalShift(uint8_t direction)//direction			LEFT	   0x27     	RIGHT  0x26

{
	OLED_WR_CMD(0x2e);//停止滚动
	OLED_WR_CMD(direction);//设置滚动方向
	OLED_WR_CMD(0x00);//虚拟字节设置，默认为0x00
	OLED_WR_CMD(0x00);//设置开始页地址
	OLED_WR_CMD(0x06);//设置每个滚动步骤之间的时间间隔的帧频
    //  0x00-5帧， 0x01-64帧， 0x02-128帧， 0x03-256帧， 0x04-3帧， 0x05-4帧， 0x06-25帧， 0x07-2帧，
	OLED_WR_CMD(0x07);//设置结束页地址
	OLED_WR_CMD(0x00);//虚拟字节设置，默认为0x00
	OLED_WR_CMD(0xff);//虚拟字节设置，默认为0xff
	OLED_WR_CMD(0x2f);//开启滚动-0x2f，禁用滚动-0x2e，禁用需要重写数据
}
char* itoa(int num,char* str,int radix)
{
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
	unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
	int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
	//获取要转换的整数的绝对值
	if(radix==10&&num<0)//要转换成十进制数并且是负数
	{
		unum=(unsigned)-num;//将num的绝对值赋给unum
		str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
	}
	else unum=(unsigned)num;//若是num为正，直接赋值给unum
 
	//转换部分，注意转换后是逆序的
	do
	{
		str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
		unum/=radix;//unum去掉最后一位
 
	}while(unum);//直至unum为0退出循环
 
	str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
	//将顺序调整过来
	if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
	else k=0;//不是负数，全部都要调整
 
	char temp;//临时变量，交换两个值时用到
	for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
	{
		temp=str[j];//头部赋值给临时变量
		str[j]=str[i-1+k-j];//尾部赋值给头部
		str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
	}
 
	return str;//返回转换后的字符串
}
void OLED_ShowFNum(unsigned char Line, unsigned char Column, float Number, uint8_t Length,uint8_t Flength)
{
    uint8_t i;
  
    float Number1;
    float Number2;
    char abc[10]={0};
		char abc4[]  =".";
		char abc5[]  ="0";
		Number2=Number;
if(Number>=1){		
    //将浮点数转换成整数然后显示
	for (i = 1; i <= Flength; i++)
	{
		Number2 *= 10;
	}
  Number2=(int)Number2;

    itoa(Number2,abc,10);
//    OLED_ShowStr(Line+50, Column , abc,1);
    for (i = 0; i < Length; i++)                            
    {   if(i < (Length - Flength-1))
			{
			OLED_ShowStr(Line+ (i*6) , Column , abc+i,1);
			
			
			
			}
			
        else if(i == (Length - Flength-1))
        {   OLED_ShowStr(Line+ (i*6), Column ,abc+i,1);
//            OLED_ShowStr(Line+ (i*6)+6,Column ,abc4,1);
         
           
        }
        else
        {
           OLED_ShowStr(Line+ (i*6)+6 , Column , abc+i,1);
        }
        
    }    
        
}
else{

	for (i = 1; i <= Flength; i++)
	{
		Number2 *= 10;
	}
  Number2=(int)Number2;

    itoa(Number2,abc,10);
//    OLED_ShowStr(Line+50, Column , abc,1);
    for (i = 0; i < Length; i++)                            
    {   if(i < (Length - Flength))
			{
			OLED_ShowStr(Line+ (i*6) , Column , abc5,1);
			
			
			
			}
			
        else if(i == (Length - Flength-1))
        {   OLED_ShowStr(Line+ (i*6), Column ,abc+i-(Length - Flength),1);
//            OLED_ShowStr(Line+ (i*6)+6,Column ,abc4,1);
         
           
        }
        else
        {
           OLED_ShowStr(Line+ (i*6)+6 , Column , abc+i-(Length - Flength),1);
        }
        
    }





}
}
