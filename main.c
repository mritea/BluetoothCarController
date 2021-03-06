/*
*本程序用定时器2作为波特率发生器，由模块接收数据传到单片机，同时可回传数据到控制主机，实现小车的控制功能
*通信协议：1	前进 1
					 2	后退 2
					 3	右转 3
					 4	左转 4
					 其它 停止 stop
*平台：STC89C52RC
*作者：W_Honor
*日期：2017/7/24
*/
#include <stc89xx.h>
#include "motor.h"

#define FOSC 12000000L      //System frequency
#define BAUD 9600   //设置波特率

bit flag = 0;

bit rec_flag = 0;//数据接受标志位

unsigned char RecData = 0;

char code str[] = "stop"; 


//串口初始化程序	`
void UART_Init(void)
{
 //PCON = 0x80;//波特率加倍，SOMD = 1
 SCON = 0x50; //SM0 SM1 SM2 REN TB8 RB8 TI RI 串行口工作方式1，10位异步收发
 RCAP2H = (65536-(FOSC/32/BAUD)) >> 8;
 RCAP2L = (65536-(FOSC/32/BAUD));
 TH2 = RCAP2H;
 TL2 = RCAP2L;
 T2CON = 0x34; //TF2 EXF2 RCLK TCLK EXEN2 TR2 C/T2 CP/RL2//定时器2用作波特率发生器
 T2MOD = 0;
 ES = 1;
 EA = 1;
	
/*	PCON = 0x80;
    SCON = 0x50;   // 
    TMOD = 0x20;   //T1
    TH1 = 0xF3;    //[
    TL1 = 0xF3;  
    TR1 = 1;                 	
    ES  = 1;       //
    EA  = 1;
	*/
}

//向上位机发送一个字节瘦信息
void senddata(char dat)
{
		if(rec_flag == 1)
		{
			ES=0;//
			SBUF=dat;
			while(!TI);
			TI=0;
			ES=1;//
		}
		rec_flag = 0;
}

//向上位机发送一个字符串
void sendstr(void)
{
	unsigned int i = 0;
	if(flag == 1)
	{
		flag = 0;
		while(str[i] != '\0')
	{
		ES = 0;
		SBUF = str[i];
		while(!TI);
		TI = 0;
		i++;
		ES = 1;
	}
	i = 0;
	}
}

//主函数
void main(void)
{
	UART_Init();
	P0 = 0;//屏蔽干扰
	while(1);
	}

//串口服务中断子程序�
void Uart_Timer2(void) interrupt 4//中断接收一个字节数据,串口中断何�4�
{
	unsigned char i = 0;
	if(RI == 1)//接收标志位//当硬件接收到数据时会置位
	{
		RI = 0;//关闭接收�	
		RecData = SBUF;
		rec_flag = 1;//接受完一个字符置1
		switch(RecData)
		{
			case '1'://前进
			  go();
				senddata('1');
			break;
			case '2'://后退
				back();
				senddata('2');
			break;
			case '3'://右转
				right();
			  senddata('3');
			break;
			case '4'://左转
				left();
				senddata('4');
			break;
			default:
				stop();
			  flag = 1;
				sendstr();
				break;
		}
	}
	else
	{
		TI = 0;
	}
}