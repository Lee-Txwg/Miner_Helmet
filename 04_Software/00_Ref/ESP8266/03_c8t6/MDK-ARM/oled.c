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
		OLED_WR_CMD(0xb0+i);    //����ҳ��ַ��0~7��
		OLED_WR_CMD(0x00);      //������ʾλ�á��е͵�ַ
		OLED_WR_CMD(0x10);      //������ʾλ�á��иߵ�ַ
		for(n=0;n<128;n++)
			OLED_WR_DATA(fill_Data);
	}
}
void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD((x&0x0f)|0x01);
}
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)//��ʾ�ַ�
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

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) //��ʾ����
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
	OLED_WR_CMD(0x2e);//ֹͣ����
	OLED_WR_CMD(direction);//���ù�������
	OLED_WR_CMD(0x00);//�����ֽ����ã�Ĭ��Ϊ0x00
	OLED_WR_CMD(0x00);//���ÿ�ʼҳ��ַ
	OLED_WR_CMD(0x06);//����ÿ����������֮���ʱ������֡Ƶ
    //  0x00-5֡�� 0x01-64֡�� 0x02-128֡�� 0x03-256֡�� 0x04-3֡�� 0x05-4֡�� 0x06-25֡�� 0x07-2֡��
	OLED_WR_CMD(0x07);//���ý���ҳ��ַ
	OLED_WR_CMD(0x00);//�����ֽ����ã�Ĭ��Ϊ0x00
	OLED_WR_CMD(0xff);//�����ֽ����ã�Ĭ��Ϊ0xff
	OLED_WR_CMD(0x2f);//��������-0x2f�����ù���-0x2e��������Ҫ��д����
}
char* itoa(int num,char* str,int radix)
{
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//������
	unsigned unum;//���Ҫת���������ľ���ֵ,ת�������������Ǹ���
	int i=0,j,k;//i����ָʾ�����ַ�����Ӧλ��ת��֮��i��ʵ�����ַ����ĳ��ȣ�ת����˳��������ģ��������������k����ָʾ����˳��Ŀ�ʼλ��;j����ָʾ����˳��ʱ�Ľ�����
 
	//��ȡҪת���������ľ���ֵ
	if(radix==10&&num<0)//Ҫת����ʮ�����������Ǹ���
	{
		unum=(unsigned)-num;//��num�ľ���ֵ����unum
		str[i++]='-';//���ַ�����ǰ������Ϊ'-'�ţ�����������1
	}
	else unum=(unsigned)num;//����numΪ����ֱ�Ӹ�ֵ��unum
 
	//ת�����֣�ע��ת�����������
	do
	{
		str[i++]=index[unum%(unsigned)radix];//ȡunum�����һλ��������Ϊstr��Ӧλ��ָʾ������1
		unum/=radix;//unumȥ�����һλ
 
	}while(unum);//ֱ��unumΪ0�˳�ѭ��
 
	str[i]='\0';//���ַ���������'\0'�ַ���c�����ַ�����'\0'������
 
	//��˳���������
	if(str[0]=='-') k=1;//����Ǹ��������Ų��õ������ӷ��ź��濪ʼ����
	else k=0;//���Ǹ�����ȫ����Ҫ����
 
	char temp;//��ʱ��������������ֵʱ�õ�
	for(j=k;j<=(i-1)/2;j++)//ͷβһһ�Գƽ�����i��ʵ�����ַ����ĳ��ȣ��������ֵ�ȳ�����1
	{
		temp=str[j];//ͷ����ֵ����ʱ����
		str[j]=str[i-1+k-j];//β����ֵ��ͷ��
		str[i-1+k-j]=temp;//����ʱ������ֵ(��ʵ����֮ǰ��ͷ��ֵ)����β��
	}
 
	return str;//����ת������ַ���
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
    //��������ת��������Ȼ����ʾ
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
