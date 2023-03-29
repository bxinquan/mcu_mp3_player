/*software delay time*/
/*CLK is 33.1776MHz*/

/*
*544177215@qq.com
*2012.3.1
*∆’÷–HC6800V3.2
*/

#ifndef __delay_h__
#define __delay_h__

void delay1ms(unsigned int x)//delay 0.001 seconds
{
	unsigned int t;
	while(x--)
		for(t=113*3;t;t--);
}

void delay10ms(unsigned int x)//delay 0.01 seconds
{
	unsigned int t;
	while(x--)
		for(t=1149*3;t;t--);
}

#endif