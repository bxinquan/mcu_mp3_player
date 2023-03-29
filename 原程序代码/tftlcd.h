

/*
*GYTF018LB35BOM
*128*160 color TFT LCD 1.77英寸
*16位真彩色
*Controller:ST7735
*/


/*
*544177215@qq.com
*2011.11.4
*普中HC6800V3.2
*卞新权
*/

/*
*典型使用步骤
*tftWriteByte tftReadByte tftReset为基本函数
*tftSetWindow tftRGB tftClear tftShowChar tftShowPic为用户可使用函数
*使用前请先使用tftInit进行初始化
*显示汉字时 请先设置好参数（TFT_CHAR_WINDOW_FORECOLOR TFT_CHAR_WINDOW_BACKCOLOR...）
*/

#ifndef __tftlcd_h__
#define __tftlcd_h__


#include<reg52.h>
#include<intrins.h>
#include"delay.h"


//接口定义
#define TFTDB P0//并行数据口
sbit TFTCS=P2^6;//片选 0有效
sbit TFTRST=P2^7;//重置 0有效
sbit TFTRS=P2^5;//数据或命令选择端 0-命令 1-数据
sbit TFTWR=P3^3;//写 0有效
sbit TFTRD=P3^2;//读 0有效



//定义TFT屏的像素尺寸
#define TFT_WIDTH 128
#define TFT_HEIGHT 160





//ST7735功能命令表1宏定义
//参考ST7735_V1[1].0_2001127.pdf
#define TFT_NOP			0x00	//空操作
#define TFT_SWRESET		0x01	//软件复位
#define TFT_RDDID		0x04	//读显示器ID
#define TFT_RDDST		0x09	//读显示器状态
#define TFT_RDDPM		0x0a	//读显示器电源
#define TFT_RDDMADCTL  	0x0b	//读显示器扫描方式
#define TFT_RDDCOLMOD	0x0c	//读显示器像素
#define TFT_RDDIM		0x0d	//读显示器图像
#define TFT_RDDSM		0x0e	//读显示器信号
#define TFT_SLPIN		0x10	//进入睡眠
#define TFT_SLPOUT		0x11	//离开睡眠
#define TFT_PTLON		0x12	//打开部分模式
#define TFT_NORON		0x13	//关闭部分模式(普通)
#define TFT_INVOFF		0x20	//显示器颜色取反关
#define TFT_INVON		0x21	//显示器颜色取反开
#define TFT_GAMSET		0x26	//伽玛曲线选择
#define TFT_DISPOFF		0x28	//显示器关
#define TFT_DISPON		0x29	//显示器开
#define TFT_CASET		0x2a	//列地址设置
#define TFT_RASET		0x2b	//行地址设置
#define TFT_RAMWR		0x2c	//写数据
#define TFT_RAMRD		0x2e	//读数据
#define TFT_PTLAR		0x30	//部分模式起始/结束地址设置
#define TFT_TEOFF		0x34	//撕效果线关闭
#define TFT_TEON		0x35	//撕效果线开启
#define TFT_MADCTL		0x36	//显示存储器访问控制(扫描方式)
#define TFT_IDMOFF		0x38	//空闲模式关闭
#define TFT_IDMON		0x39	//空闲模式开启
#define TFT_COLMOD		0x3a	//接口像素格式
#define TFT_RDID1		0xda	//读ID1
#define TFT_RDID2		0xdb	//读ID2
#define TFT_RDID3		0xdc	//读ID3



/********************************************************************************/
//函数名:tftWriteByte
//功  能:向TFT中写入一个字节的命令或数据
//参  数:rs=0写命令 rs=1写数据 x为待写的命令或数据		
//返回值:
//备  注:内部
//更  新:2012.3.6									
/********************************************************************************/
void tftWriteByte(bit rs,unsigned char x)
{
	TFTRS=rs;
	TFTDB=x;
	TFTCS=0;
	TFTWR=0;
	TFTWR=1;
	TFTCS=1;
	TFTRS=1;
}


/********************************************************************************/
//函数名:tftReadByte
//功  能:从TFT中读取一个字节的命令或数据
//参  数:rs=0读命令 rs=1读数据		
//返回值:已读取的命令或数据
//备  注:内部
//更  新:2012.3.6									
/********************************************************************************/
unsigned char tftReadByte(bit rs)
{
	unsigned char res;
	TFTRS=rs;
	TFTDB=0xff;
	TFTCS=0;
	TFTRD=0;
	TFTRD=1;
	res=TFTDB;
	TFTCS=1;
	TFTRS=1;
	return res;
}


/********************************************************************************/
//函数名:tftReset
//功  能:TFT 复位
//参  数:		
//返回值:
//备  注:外部
//更  新:2012.3.6									
/********************************************************************************/
void tftReset(void)
{
	TFTCS=0;
	TFTRS=0;
	TFTWR=0;
	TFTRD=0;
	delay1ms(50);
	TFTRST=0;
	delay1ms(20);
	TFTRST=1;
	delay1ms(20);
	TFTCS=1;
	TFTRS=1;
	TFTWR=1;
	TFTRD=1;
}



/********************************************************************************/
//函数名:tftInit
//功  能:TFT初始化及相关配置
//参  数:		
//返回值:
//备  注:外部
//更  新:2012.3.6									
/********************************************************************************/
void tftInit(void)
{
	tftReset();

	tftWriteByte(0,TFT_SWRESET);	    //software reset软件复位
	delay1ms(100);
	tftWriteByte(0,TFT_SLPOUT);	    //sleep out/power on(SLPOUT) 退出睡眠
	delay1ms(50);
	tftWriteByte(0,TFT_INVOFF);		//display inversion off 关闭取反
	delay1ms(50);
	tftWriteByte(0,TFT_IDMOFF);	    //ID mode off (IDMOFF)
	delay1ms(50);
	tftWriteByte(0,TFT_NORON);	    //Normal display mode on (Partial mode off)
	
	tftWriteByte(0,TFT_COLMOD);	    //color mode Interface pixel format  (COLMOD)
	tftWriteByte(1,0x05);		//16-bit/pixel , 1-times data transfer
	
	tftWriteByte(0,0xc0);		//power control 1      (PWCTR1)
	tftWriteByte(1,0x03);		//default value  LCM=0 set the GVDD voltage=4.65
	
	tftWriteByte(0,0xc5);	   
	tftWriteByte(1,0xc8);	   //VCOMH voltage set 4.300V
	
	tftWriteByte(0,0xc6);
	tftWriteByte(1,0x1f);	   //VCOMAC voltage set 5.550V ;
	delay1ms(10);
	
	tftWriteByte(0,0xfe);
	tftWriteByte(1,0x00);
	tftWriteByte(1,0x04);
	
	tftWriteByte(0,TFT_GAMSET);	   //gamma sel
	tftWriteByte(1,0x04);
	
	
	tftWriteByte(0,TFT_CASET);	   //column address set (CASET)
	tftWriteByte(1,0x00);	   //XS15-XS8
	tftWriteByte(1,0x00);	   //XS7-XS0    (X start address)
	tftWriteByte(1,0x00);	   //XE15-XE8
	tftWriteByte(1,0x7f);	   //XE7-XE0    (X end address)

	tftWriteByte(0,TFT_RASET);	   //row address set (RASET)
	tftWriteByte(1,0x00);	   //YS15-YS8
	tftWriteByte(1,0x00);	   //YS7-YS0    (Y start address)
	tftWriteByte(1,0x00);	   //YE15-YE8
	tftWriteByte(1,0x9f);	   //YE7-YE0    (Y end address)

	tftWriteByte(0,TFT_MADCTL);	   //memory data access control  (MADCTR)
	tftWriteByte(1,0xc0);		//由于本模块的液晶是倒置的 所以MX=1 MY=1
	
	tftWriteByte(0,TFT_DISPON);	  // display on

}


/********************************************************************************/
//函数名:tftSetWindow
//功  能:设置显示窗口
//参  数:StartX为窗口的横坐标 StartY为窗口的纵坐标 width为窗口的宽度 height为窗口的高度		
//返回值:
//备  注:内部 最大坐标为(127,159) 最大尺寸为128*160
//更  新:2012.3.6									
/********************************************************************************/
void tftSetWindow(unsigned char StartX,unsigned char StartY,unsigned char width,unsigned char height)
{

	tftWriteByte(0,TFT_CASET);
	tftWriteByte(1,0);
	tftWriteByte(1,StartX);
	tftWriteByte(1,0);
	tftWriteByte(1,StartX+width-1);

	tftWriteByte(0,TFT_RASET);
	tftWriteByte(1,0);
	tftWriteByte(1,StartY);
	tftWriteByte(1,0);
	tftWriteByte(1,StartY+height-1);

	tftWriteByte(0,TFT_RAMWR);	//下面可以写数据了
}


/********************************************************************************/
//函数名:tftSetMadctl
//功  能:设置TFT窗口的扫描方式
//参  数:mode为已定义好的扫描方式的按位相或	
//返回值:
//备  注:外部 使用了TFT_MADCTL命令
//更  新:2012.3.21									
/********************************************************************************/
#define TFT_MADCTL_MY	0x80	//有此项Y地址反向
#define TFT_MADCTL_MX	0x40  	//有此项X地址反向
#define TFT_MADCTL_MV	0x20	//有此项X地址与Y地址交换
#define TFT_MADCTL_ML	0x10	//有此项垂直显示刷新时从底到顶
#define TFT_MADCTL_RGB	0x08	//有此项输入的颜色格式为BGR(无此项时为RGB)
#define TFT_MADCTL_MH	0x04	//有此项水平显示刷新时从右到左
void tftSetMadctl(unsigned char mode)
{
	tftWriteByte(0,TFT_MADCTL);
	tftWriteByte(1,mode);				
}




/********************************************************************************/
//函数名:tftRGB
//功  能:将RGB方式表示的颜色转换为16位真彩色
//参  数:r与b取值范围为0~31 g取值范围为0~63		
//返回值:转换后16位真彩色
//备  注:外部
//更  新:2012.3.6									
/********************************************************************************/
unsigned short tftRGB(unsigned char r,unsigned char g,unsigned char b)
{
	unsigned short res=0;
	r=r&0x1f;
	g=g&0x3f;
	b=b&0x1f;
	res|=r<<11;
	res|=g<<5;
	res|=b;
	return res;
}



/********************************************************************************/
//函数名:tftClear
//功  能:用颜色color清除从(StartX,StartY)开始的大小为width*height的区域
//参  数:StartX为区域的横坐标 StartY为区域的纵坐标 width为区域的宽度 height为区域的高度	
//返回值:
//备  注:外部
//更  新:2012.3.6									
/********************************************************************************/
void tftClear(unsigned char StartX,unsigned char StartY,unsigned char width,unsigned char height,unsigned short color)
{
	unsigned short size;
	tftSetWindow(StartX,StartY,width,height);
	size=width*height;
	while(size--)
	{
		tftWriteByte(1,color>>8);
		tftWriteByte(1,color);
	}
}





/******************************************************************************/
//"字符显示窗口"即在TFT上开辟出一个像素区域,此区域仅供显示字符字模码之用
//			显示字符字模码时码,仅显示在窗口之内,而不出超出此区域
//			所有字符(字模码)的显示都应先为其设置一个"字符显示窗口"
/******************************************************************************/


/********************************************************************************/
//函数名:字符显示窗口公共变量
//功  能:控制字符显示窗口
//参  数:
//返回值:
//备  注:  所有字符(字模码)的显示都应先为其设置一个"字符显示窗口"
//更  新:2012.3.7									
/********************************************************************************/
unsigned short TFT_CHAR_WINDOW_BACKCOLOR=0xffff;	//字符背景色 打字模时有点(即有1)的地方所使用的填充色
unsigned short TFT_CHAR_WINDOW_FORECOLOR=0x0000;	//字符前景色 打字模时无点(即有0)的地方所使用的填充色
unsigned char TFT_CHAR_WINDOW_LEFT=0;				//"字符显示窗口"距TFT左边界的像素数(0~127)
unsigned char TFT_CHAR_WINDOW_TOP=0;				//"字符显示窗口"距TFT顶边界的像素数(0~159)
unsigned char TFT_CHAR_WINDOW_WIDTH=TFT_WIDTH;		//字符显示窗口宽度(1~128)
unsigned char TFT_CHAR_WINDOW_HEIGHT=TFT_HEIGHT;	//字符显示窗口高度(1~160)
unsigned char TFT_CHAR_X=0;					//待打印字符距"字符显示窗口"左边界像素数
unsigned char TFT_CHAR_Y=0;					//待打印字符距"字符显示窗口"顶边界像素数
unsigned char TFT_CHAR_TYPE=1;				//打字符时是否填充背景色


/********************************************************************************/
//函数名:tftSetCharWindow
//功  能:设置字符显示窗口
//参  数:left为窗口距TFT左边界的像素数,top为窗口距TFT顶边界的像素数,width为窗口的宽度,height为窗口的高度
//返回值:
//备  注:left范围[0,127],top范围[0,159],width范围[1,128],height范围[1,160],left+width<=128,top+height<=160
//更  新:2012.3.7									
/********************************************************************************/
void tftSetCharWindow(unsigned char left,unsigned char top,unsigned char width,unsigned char height)
{

	//设置字符显示窗口公共变量
	TFT_CHAR_WINDOW_LEFT=left;
	TFT_CHAR_WINDOW_TOP=top;
	TFT_CHAR_WINDOW_WIDTH=width;
	TFT_CHAR_WINDOW_HEIGHT=height;
	TFT_CHAR_X=0;
	TFT_CHAR_Y=0;
}


/********************************************************************************/
//函数名:tftSetCharXY
//功  能:设置字符在"字符显示窗口"的当前坐标
//参  数:x为横坐标,y为纵坐标
//返回值:
//备  注:外部 
//			x范围[0,TFT_CHAR_WINDOW_WIDTH-1] 
//			y范围[0,TFT_CHAR_WINDOW_HEIGHT-1]
//更  新:2012.3.7									
/********************************************************************************/
void tftSetCharXY(unsigned char x,unsigned char y)
{
	TFT_CHAR_X=x;
	TFT_CHAR_Y=y;
}

/********************************************************************************/
//函数名:tftShowChar
//功  能:用字符的字模码来显示该字符(包括汉字等一切字模码)
//参  数:width为字模宽度 height为字模高度,zimo为字模数组 字模宽度*字模高度必须为8的整数倍	
//返回值:
//备  注:外部
//			横向取模左高位,数据排列:从左到右从上到下 
//			本函数会自动修改变量TFT_CHAR_X及TFT_CHAR_Y的值,使用其值为下一个字符位置
//			本函数使用TFT_CHAR_WINDOW_FORECOLOR作为字符前景色,TFT_CHAR_WINDOW_BACKCOLOR作为字符背景色
//			可修改TFT_CHAR_TYPE来决定是否为字符填充背景色
//			暂支持8*16 16*12 16*16 24*24
//			建议使用LcmZimo.exe取字模软件
//更  新:2012.3.7									
/********************************************************************************/
void tftShowChar(unsigned char width,unsigned char height,unsigned char *zimo)
{
	unsigned char j,tmp;
	unsigned short size,i;
	size=width * height / 8; //计算字模码字节数

	//超边界处理
	if(TFT_CHAR_X+width>TFT_CHAR_WINDOW_WIDTH)
	{
		TFT_CHAR_X=0;
		TFT_CHAR_Y+=height;
	}
	if(TFT_CHAR_Y+height>TFT_CHAR_WINDOW_HEIGHT)
	{
		TFT_CHAR_Y=0;
	}
		
	tftSetWindow(TFT_CHAR_WINDOW_LEFT+TFT_CHAR_X,TFT_CHAR_WINDOW_TOP+TFT_CHAR_Y,width,height); //在TFT上开窗
	
	for(i=0;i<size;i++)//打字模
	{
		tmp=zimo[i]; //取一个字节的字模
		for(j=8;j>0;j--)	  //打一个字节的字模
		{
			if(tmp&0x80)	  //取高位
			{  //是1   打前景色
				tftWriteByte(1,TFT_CHAR_WINDOW_FORECOLOR>>8);
				tftWriteByte(1,TFT_CHAR_WINDOW_FORECOLOR);
			}
			else if(TFT_CHAR_TYPE)
			{  //不是1 但填充背景色
				tftWriteByte(1,TFT_CHAR_WINDOW_BACKCOLOR>>8);
				tftWriteByte(1,TFT_CHAR_WINDOW_BACKCOLOR);
			}
			else
			{	 //不是1 不填充背景色 但要后移一个像素的位置(利用读移位)
				tftReadByte(1);
				tftReadByte(1);
			}
			tmp<<=1;
		}	
	}

	TFT_CHAR_X+=width; //一个字符打完后 右移一个字符的位置
	//超边界处理
	if(TFT_CHAR_X+width>TFT_CHAR_WINDOW_WIDTH)
	{
		TFT_CHAR_X=0;
		TFT_CHAR_Y+=height;
	}
	if(TFT_CHAR_Y+height>TFT_CHAR_WINDOW_HEIGHT)
	{
		TFT_CHAR_Y=0;
	}
}  



/********************************************************************************/
//函数名:tftShowPic
//功  能:显示16位真彩色图片
//参  数:在(StartX,StartY)处开始显示,图片大小为width*height,图片取模表为array	
//返回值:
//备  注:外部
//			图片取模软件建议使用Image2Lcd
//			图片取模方向为水平扫描
//			扫描结果以低字节在前
//更  新:2012.3.6									
/********************************************************************************/
void tftShowPic(unsigned short StartX,unsigned short StartY,unsigned char width,unsigned char height,unsigned char *array)
{
	unsigned short i,max=width*height;
	tftSetWindow(StartX,StartY,width,height);
	for(i=0;i<max;i++)
	{
		tftWriteByte(1,array[2*i+1]);//由于高字节在后 所以高字节为奇数字节
		tftWriteByte(1,array[2*i]);	 //由于低字节在前 所以低字节为偶数字节
	}
}

/********************************************************************************/
//函数名:tftDrawRect
//功  能:在TFT上画出矩形
//参  数:(StartX,StartY)为左上角坐标,(EndX,EndY)右下角坐标,width为线宽,color为线颜色
//返回值:
//备  注:外部
//更  新:2012.3.7									
/********************************************************************************/
void tftDrawRect(unsigned short StartX,
				unsigned short StartY,
				unsigned char EndX,
				unsigned char EndY,
				unsigned char width,
				unsigned short color)
{
	unsigned short size;
	//画上线
	size=(EndX-StartX+1)*width;
	tftSetWindow(StartX,StartY,EndX-StartX+1,width);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//画下线
	size=(EndX-StartX+1)*width;
	tftSetWindow(StartX,EndY,EndX-StartX+1,width);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//画左线
	size=(EndY-StartY+1)*width;
	tftSetWindow(StartX,StartY,width,EndY-StartY+1);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//画右线
	size=(EndY-StartY+1)*width;
	tftSetWindow(EndX,StartY,width,EndY-StartY+1);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
}


/********************************************************************************/
//函数名:tftInvertColor
//功  能:将设定的区域中的颜色取反显示(color=~color)
//参  数:StartX为区域的横坐标 StartY为区域的纵坐标 width为区域的宽度 height为区域的高度
//返回值:
//备  注:外部
//更  新:2012.3.7									
/********************************************************************************/
/*void tftInvertColor(unsigned char StartX,unsigned char StartY,unsigned char width,unsigned char height)
{
	
} */


#endif
