    #include<reg51.h>
    #include<intrins.h>  //其中有nop函数的定义

    /*对于延时很短的，要求在us级的，采用“_nop_”函数，这个函数相当汇编NOP指令，延时几微秒。NOP指令为单周期指令，可由晶振频率算出延时时间，对于12M晶振，延时1uS。*/

    #define uchar unsigned char
    #define uint unsigned int

    sbit RS=P2^6;  //定义数据命令选择端
    sbit RW=P2^5;  //定义读写选择端
    sbit E=P2^7;   //定义使能端
    //注意：具体为哪一个IO口由你自己的接线来定，并不是一定要这样写！

    uchar text1[]="Welcome to";   //第一行要显示的字符
    uchar text2[]="Lime House";   //第二行要显示的字符

    void delay_ms(uint z)  //毫秒级延时函数
    {
       uint i,j;
       for(i=z;i>0;i--)
          for(j=110;j>0;j--);
    }

    //为什么要判忙？
    /*因为LCD1602控制芯片也是一个CPU,它在处理上位机发来的某些指令的时候，耗时较长。如复位命令，全屏清除命令等。另外，LCD1602的CPU速度较慢，上位机较快时，LCD1602CPU来不及完成上条命令，会对后续命令不予理睬，这样就造成了命令序列不能正确完整地到达LCD1602CPU，以至于显示不正常。*/

    uchar Lcd1602_ReadBusy()   //判断lcd1602是否处于忙的状态，即读忙
    {
       uchar temp;
       RS=0;
       RW=1;
       _nop_();
       P0=0xff;     //读某IO口数据前，先将该口置为1            

    /*原因：电路中存在的一个普遍的现象：高电平很容易被低电平拉低，而低电平一般不可能被高电平拉高。所以在读数据之前将单片机IO口拉高才不会影响原来数据线上的数据！*/

       _nop_();
       E=1;
       _nop_();
       temp=P0;   //读取此时lcd1602的状态字
       _nop_();
       E=0;
       return (temp&0x80);  //如果忙

       /*状态字为temp（8位2进制数）的最高位，最高位为1表示禁止读写，为0表示允许读写，即temp&0x80得1表示忙，得0表示不忙*/

    }

    /*在lcd1602的资料中，读写操作都有对应的时序图，想要理解lcd1602的具体工作流程还得认真研究时序图！*/

    void Lcd1602_WriteCom(uchar com)  //写命令
    {
       while(Lcd1602_ReadBusy());  //判忙
       RS=0;   //命令
       RW=0;   //写
       _nop_();
       P0=com;   //准备发送命令
       _nop_();
       E=1;      //由时序图知，使能端为高电平时才允许数据交换
       _nop_();
       _nop_();
       E=0;      //由时序图知，使能端在完成数据交换后要拉低
       _nop_();
       _nop_();   
    }

    void Lcd1602_WriteData(uchar dat)  //写数据
    {
       while(Lcd1602_ReadBusy());   //判忙
       RS=1;   //数据
       RW=0;   //写
       _nop_();
       P0=dat;
       _nop_();
       E=1;
       _nop_();
       _nop_();
       E=0;
       _nop_();
       _nop_();   
    }

    void Lcd1602_init()   //初始化函数
    {
       delay_ms(15);
       write_com(0x38);   //显示模式设置
       delay_ms(5);
       write_com(0x38);
       delay_ms(5);
       write_com(0x38);
       write_com(0x0c);  //显示开
       write_com(0x01);  //显示清屏
       write_com(0x06);  //显示光标
    }

    void main()
    {
       uchar i,j;
       Lcd1602_init();
       Lcd1602_WriteCom(0x80);   //0x80是第一行的第一个字符的地址
       for(i=0;i<10;i++)  
          Lcd1602_WriteData(text1[i]);   
       Lcd1602_WriteCom(0x80+0x40);   //0xc0是第二行的第一个字符的地址

       /*表示的方法有很多，可以用0xc0,0x80+0x40,0x80|0x40等等*/

       for(j=0;j<10;j++)
          Lcd1602_WriteData(text2[j]);
    }
