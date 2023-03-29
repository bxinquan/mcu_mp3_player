

/*
*红外遥控接收
*接P3^2口
*使用中断0以及定时器1
*运行于33.1776MHz MCU
*从低位到高位接收
*/

/*
*544177215@qq.com
*2011.11.3
*普中HC6800V3.2
*/


/*
*典型使用步骤
*先使用irInit初始化
*再使用irRead读取
*/

/*以下为遥控对应按键编码
0x45,0x46,0x47
0x44,0x40,0x43
0x07,0x15,0x09
0x16,0x19,0x0D
0x0C,0x18,0x5E
0x08,0x1C,0x5A
0x42,0x52,0x4A
*/


#ifndef __ir_h__
#define __ir_h__


#include<reg52.h>


//以下数据根据晶振修改
//前导码周期范围13.5ms(12442)
#define L_MIN_S 12200*6
#define L_MAX_S 12700*6

//信号0周期范围1.12ms(1032)
#define L_MIN_0 700*6
#define L_MAX_0 1100*6

//信号1周期范围2.24ms(2064)
#define L_MIN_1 1600*6
#define L_MAX_1 2200*6


//红外接收数组(用户码，用户码反码,操作码,操作码反码)
unsigned char IR_ARRAY[4]={0,0,0,0};

unsigned char IR_COUNT;//已成功接收的位数

bit IR_OK;//接收成功位 成功接收后置1 使用时应在取得有效数据后将它置0

bit IR_START_FLAG;//已开始标致位



void irInit()
{
	IR_OK=0;
	IR_START_FLAG=0;
	IR_COUNT=0;

	//设置中断
	IT0=1;//边沿触发
	EX0=1;
	EA=1;

	//设置定时器1
	TMOD=TMOD&0x0f;
	TMOD=TMOD|0x10;//方式1
	TH1=0;
	TL1=0;
	TR1=1;//定时启动
}

//红外接收中断
void irInt0() interrupt 0
{
	unsigned short count=TH1*256+TL1;//取周期范围
	TH1=TL1=0;//重置

	if(count>=L_MIN_S && count<=L_MAX_S)//是开始
	{
		IR_START_FLAG=1;
		IR_OK=0;
		IR_COUNT=0;
		return;
	}
	if(IR_START_FLAG)//是数据
	{
		if(count>=L_MIN_0 && count<=L_MAX_0)//是0
		{
			IR_ARRAY[IR_COUNT/8]=IR_ARRAY[IR_COUNT/8]>>1;
			IR_COUNT++;
		}

		if(count>=L_MIN_1 && count<=L_MAX_1)//是1
		{

			IR_ARRAY[IR_COUNT/8]=(IR_ARRAY[IR_COUNT/8]>>1)|0x80;
			IR_COUNT++;
		}
		if(IR_COUNT==32)//接收完成
		{
			IR_START_FLAG=0;
			//校检正确性
			if(IR_ARRAY[2]==~IR_ARRAY[3] && IR_ARRAY[0]==~IR_ARRAY[1])
				IR_OK=1;
		}
	}
}


//取出一个有效用户码和操作码
//有数据返回1
bit irRead(unsigned char *user,unsigned char *val)
{
	if(~IR_OK)
		return 0;
	*user=IR_ARRAY[0];
	*val=IR_ARRAY[2];
	IR_OK=0;
	return 1;
}

#endif


