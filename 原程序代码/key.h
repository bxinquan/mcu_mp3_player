/*
*key1指矩阵键盘 
*key2指独立键盘
*/


/*
*544177215@qq.com
*2011.11.3
*普中HC6800V3.2	
*卞新权
*/

#ifndef __key_h__
#define __key_h__

#include<reg52.h>
#include"delay.h"


//定义JP4接法
#define JP4 P1

//定义JP5接法
#define JP5 P3


unsigned char code key1_o_tab[]={0xfd,0xfb,0xf7};
unsigned char code key1_i_tab[]={0xe0,0xd0,0xb0,0x70};
unsigned char code key2_i_tab[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};


//扫描矩阵键盘
//有输入返回1
bit key1(unsigned char *x_ptr,unsigned char *y_ptr)
{
	unsigned char x,y;
	for(x=0;x<3;x++)
	{
		JP4=key1_o_tab[x];
		if((JP4&0xf0)==0xf0)continue;
		for(y=0;y<4;y++)
		{
			if((JP4&0xf0)!=key1_i_tab[y])continue;
			delay1ms(5);
			if((JP4&0xf0)==key1_i_tab[y])
			{
				while((JP4&0xf0)!=0xf0);
				*x_ptr=x;*y_ptr=y;
				return 1;
			}
		}
	}
	return 0;
}


//扫描独立键盘
//有输入返回1
bit key2(unsigned char *k_ptr)
{
	unsigned char k;
	JP5=0xff;
	if(JP5==0xff)return 0;
	for(k=0;k<8;k++)
	{
		if(JP5!=key2_i_tab[k])continue;
		delay1ms(5);
		if(JP5==key2_i_tab[k])
		{
			while(JP5!=0xff);
			*k_ptr=k;
			return 1;
		}
	}
	return 0;
}

#endif