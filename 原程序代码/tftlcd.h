

/*
*GYTF018LB35BOM
*128*160 color TFT LCD 1.77Ӣ��
*16λ���ɫ
*Controller:ST7735
*/


/*
*544177215@qq.com
*2011.11.4
*����HC6800V3.2
*����Ȩ
*/

/*
*����ʹ�ò���
*tftWriteByte tftReadByte tftResetΪ��������
*tftSetWindow tftRGB tftClear tftShowChar tftShowPicΪ�û���ʹ�ú���
*ʹ��ǰ����ʹ��tftInit���г�ʼ��
*��ʾ����ʱ �������úò�����TFT_CHAR_WINDOW_FORECOLOR TFT_CHAR_WINDOW_BACKCOLOR...��
*/

#ifndef __tftlcd_h__
#define __tftlcd_h__


#include<reg52.h>
#include<intrins.h>
#include"delay.h"


//�ӿڶ���
#define TFTDB P0//�������ݿ�
sbit TFTCS=P2^6;//Ƭѡ 0��Ч
sbit TFTRST=P2^7;//���� 0��Ч
sbit TFTRS=P2^5;//���ݻ�����ѡ��� 0-���� 1-����
sbit TFTWR=P3^3;//д 0��Ч
sbit TFTRD=P3^2;//�� 0��Ч



//����TFT�������سߴ�
#define TFT_WIDTH 128
#define TFT_HEIGHT 160





//ST7735���������1�궨��
//�ο�ST7735_V1[1].0_2001127.pdf
#define TFT_NOP			0x00	//�ղ���
#define TFT_SWRESET		0x01	//�����λ
#define TFT_RDDID		0x04	//����ʾ��ID
#define TFT_RDDST		0x09	//����ʾ��״̬
#define TFT_RDDPM		0x0a	//����ʾ����Դ
#define TFT_RDDMADCTL  	0x0b	//����ʾ��ɨ�跽ʽ
#define TFT_RDDCOLMOD	0x0c	//����ʾ������
#define TFT_RDDIM		0x0d	//����ʾ��ͼ��
#define TFT_RDDSM		0x0e	//����ʾ���ź�
#define TFT_SLPIN		0x10	//����˯��
#define TFT_SLPOUT		0x11	//�뿪˯��
#define TFT_PTLON		0x12	//�򿪲���ģʽ
#define TFT_NORON		0x13	//�رղ���ģʽ(��ͨ)
#define TFT_INVOFF		0x20	//��ʾ����ɫȡ����
#define TFT_INVON		0x21	//��ʾ����ɫȡ����
#define TFT_GAMSET		0x26	//٤������ѡ��
#define TFT_DISPOFF		0x28	//��ʾ����
#define TFT_DISPON		0x29	//��ʾ����
#define TFT_CASET		0x2a	//�е�ַ����
#define TFT_RASET		0x2b	//�е�ַ����
#define TFT_RAMWR		0x2c	//д����
#define TFT_RAMRD		0x2e	//������
#define TFT_PTLAR		0x30	//����ģʽ��ʼ/������ַ����
#define TFT_TEOFF		0x34	//˺Ч���߹ر�
#define TFT_TEON		0x35	//˺Ч���߿���
#define TFT_MADCTL		0x36	//��ʾ�洢�����ʿ���(ɨ�跽ʽ)
#define TFT_IDMOFF		0x38	//����ģʽ�ر�
#define TFT_IDMON		0x39	//����ģʽ����
#define TFT_COLMOD		0x3a	//�ӿ����ظ�ʽ
#define TFT_RDID1		0xda	//��ID1
#define TFT_RDID2		0xdb	//��ID2
#define TFT_RDID3		0xdc	//��ID3



/********************************************************************************/
//������:tftWriteByte
//��  ��:��TFT��д��һ���ֽڵ����������
//��  ��:rs=0д���� rs=1д���� xΪ��д�����������		
//����ֵ:
//��  ע:�ڲ�
//��  ��:2012.3.6									
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
//������:tftReadByte
//��  ��:��TFT�ж�ȡһ���ֽڵ����������
//��  ��:rs=0������ rs=1������		
//����ֵ:�Ѷ�ȡ�����������
//��  ע:�ڲ�
//��  ��:2012.3.6									
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
//������:tftReset
//��  ��:TFT ��λ
//��  ��:		
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.6									
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
//������:tftInit
//��  ��:TFT��ʼ�����������
//��  ��:		
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.6									
/********************************************************************************/
void tftInit(void)
{
	tftReset();

	tftWriteByte(0,TFT_SWRESET);	    //software reset�����λ
	delay1ms(100);
	tftWriteByte(0,TFT_SLPOUT);	    //sleep out/power on(SLPOUT) �˳�˯��
	delay1ms(50);
	tftWriteByte(0,TFT_INVOFF);		//display inversion off �ر�ȡ��
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
	tftWriteByte(1,0xc0);		//���ڱ�ģ���Һ���ǵ��õ� ����MX=1 MY=1
	
	tftWriteByte(0,TFT_DISPON);	  // display on

}


/********************************************************************************/
//������:tftSetWindow
//��  ��:������ʾ����
//��  ��:StartXΪ���ڵĺ����� StartYΪ���ڵ������� widthΪ���ڵĿ�� heightΪ���ڵĸ߶�		
//����ֵ:
//��  ע:�ڲ� �������Ϊ(127,159) ���ߴ�Ϊ128*160
//��  ��:2012.3.6									
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

	tftWriteByte(0,TFT_RAMWR);	//�������д������
}


/********************************************************************************/
//������:tftSetMadctl
//��  ��:����TFT���ڵ�ɨ�跽ʽ
//��  ��:modeΪ�Ѷ���õ�ɨ�跽ʽ�İ�λ���	
//����ֵ:
//��  ע:�ⲿ ʹ����TFT_MADCTL����
//��  ��:2012.3.21									
/********************************************************************************/
#define TFT_MADCTL_MY	0x80	//�д���Y��ַ����
#define TFT_MADCTL_MX	0x40  	//�д���X��ַ����
#define TFT_MADCTL_MV	0x20	//�д���X��ַ��Y��ַ����
#define TFT_MADCTL_ML	0x10	//�д��ֱ��ʾˢ��ʱ�ӵ׵���
#define TFT_MADCTL_RGB	0x08	//�д����������ɫ��ʽΪBGR(�޴���ʱΪRGB)
#define TFT_MADCTL_MH	0x04	//�д���ˮƽ��ʾˢ��ʱ���ҵ���
void tftSetMadctl(unsigned char mode)
{
	tftWriteByte(0,TFT_MADCTL);
	tftWriteByte(1,mode);				
}




/********************************************************************************/
//������:tftRGB
//��  ��:��RGB��ʽ��ʾ����ɫת��Ϊ16λ���ɫ
//��  ��:r��bȡֵ��ΧΪ0~31 gȡֵ��ΧΪ0~63		
//����ֵ:ת����16λ���ɫ
//��  ע:�ⲿ
//��  ��:2012.3.6									
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
//������:tftClear
//��  ��:����ɫcolor�����(StartX,StartY)��ʼ�Ĵ�СΪwidth*height������
//��  ��:StartXΪ����ĺ����� StartYΪ����������� widthΪ����Ŀ�� heightΪ����ĸ߶�	
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.6									
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
//"�ַ���ʾ����"����TFT�Ͽ��ٳ�һ����������,�����������ʾ�ַ���ģ��֮��
//			��ʾ�ַ���ģ��ʱ��,����ʾ�ڴ���֮��,����������������
//			�����ַ�(��ģ��)����ʾ��Ӧ��Ϊ������һ��"�ַ���ʾ����"
/******************************************************************************/


/********************************************************************************/
//������:�ַ���ʾ���ڹ�������
//��  ��:�����ַ���ʾ����
//��  ��:
//����ֵ:
//��  ע:  �����ַ�(��ģ��)����ʾ��Ӧ��Ϊ������һ��"�ַ���ʾ����"
//��  ��:2012.3.7									
/********************************************************************************/
unsigned short TFT_CHAR_WINDOW_BACKCOLOR=0xffff;	//�ַ�����ɫ ����ģʱ�е�(����1)�ĵط���ʹ�õ����ɫ
unsigned short TFT_CHAR_WINDOW_FORECOLOR=0x0000;	//�ַ�ǰ��ɫ ����ģʱ�޵�(����0)�ĵط���ʹ�õ����ɫ
unsigned char TFT_CHAR_WINDOW_LEFT=0;				//"�ַ���ʾ����"��TFT��߽��������(0~127)
unsigned char TFT_CHAR_WINDOW_TOP=0;				//"�ַ���ʾ����"��TFT���߽��������(0~159)
unsigned char TFT_CHAR_WINDOW_WIDTH=TFT_WIDTH;		//�ַ���ʾ���ڿ��(1~128)
unsigned char TFT_CHAR_WINDOW_HEIGHT=TFT_HEIGHT;	//�ַ���ʾ���ڸ߶�(1~160)
unsigned char TFT_CHAR_X=0;					//����ӡ�ַ���"�ַ���ʾ����"��߽�������
unsigned char TFT_CHAR_Y=0;					//����ӡ�ַ���"�ַ���ʾ����"���߽�������
unsigned char TFT_CHAR_TYPE=1;				//���ַ�ʱ�Ƿ���䱳��ɫ


/********************************************************************************/
//������:tftSetCharWindow
//��  ��:�����ַ���ʾ����
//��  ��:leftΪ���ھ�TFT��߽��������,topΪ���ھ�TFT���߽��������,widthΪ���ڵĿ��,heightΪ���ڵĸ߶�
//����ֵ:
//��  ע:left��Χ[0,127],top��Χ[0,159],width��Χ[1,128],height��Χ[1,160],left+width<=128,top+height<=160
//��  ��:2012.3.7									
/********************************************************************************/
void tftSetCharWindow(unsigned char left,unsigned char top,unsigned char width,unsigned char height)
{

	//�����ַ���ʾ���ڹ�������
	TFT_CHAR_WINDOW_LEFT=left;
	TFT_CHAR_WINDOW_TOP=top;
	TFT_CHAR_WINDOW_WIDTH=width;
	TFT_CHAR_WINDOW_HEIGHT=height;
	TFT_CHAR_X=0;
	TFT_CHAR_Y=0;
}


/********************************************************************************/
//������:tftSetCharXY
//��  ��:�����ַ���"�ַ���ʾ����"�ĵ�ǰ����
//��  ��:xΪ������,yΪ������
//����ֵ:
//��  ע:�ⲿ 
//			x��Χ[0,TFT_CHAR_WINDOW_WIDTH-1] 
//			y��Χ[0,TFT_CHAR_WINDOW_HEIGHT-1]
//��  ��:2012.3.7									
/********************************************************************************/
void tftSetCharXY(unsigned char x,unsigned char y)
{
	TFT_CHAR_X=x;
	TFT_CHAR_Y=y;
}

/********************************************************************************/
//������:tftShowChar
//��  ��:���ַ�����ģ������ʾ���ַ�(�������ֵ�һ����ģ��)
//��  ��:widthΪ��ģ��� heightΪ��ģ�߶�,zimoΪ��ģ���� ��ģ���*��ģ�߶ȱ���Ϊ8��������	
//����ֵ:
//��  ע:�ⲿ
//			����ȡģ���λ,��������:�����Ҵ��ϵ��� 
//			���������Զ��޸ı���TFT_CHAR_X��TFT_CHAR_Y��ֵ,ʹ����ֵΪ��һ���ַ�λ��
//			������ʹ��TFT_CHAR_WINDOW_FORECOLOR��Ϊ�ַ�ǰ��ɫ,TFT_CHAR_WINDOW_BACKCOLOR��Ϊ�ַ�����ɫ
//			���޸�TFT_CHAR_TYPE�������Ƿ�Ϊ�ַ���䱳��ɫ
//			��֧��8*16 16*12 16*16 24*24
//			����ʹ��LcmZimo.exeȡ��ģ���
//��  ��:2012.3.7									
/********************************************************************************/
void tftShowChar(unsigned char width,unsigned char height,unsigned char *zimo)
{
	unsigned char j,tmp;
	unsigned short size,i;
	size=width * height / 8; //������ģ���ֽ���

	//���߽紦��
	if(TFT_CHAR_X+width>TFT_CHAR_WINDOW_WIDTH)
	{
		TFT_CHAR_X=0;
		TFT_CHAR_Y+=height;
	}
	if(TFT_CHAR_Y+height>TFT_CHAR_WINDOW_HEIGHT)
	{
		TFT_CHAR_Y=0;
	}
		
	tftSetWindow(TFT_CHAR_WINDOW_LEFT+TFT_CHAR_X,TFT_CHAR_WINDOW_TOP+TFT_CHAR_Y,width,height); //��TFT�Ͽ���
	
	for(i=0;i<size;i++)//����ģ
	{
		tmp=zimo[i]; //ȡһ���ֽڵ���ģ
		for(j=8;j>0;j--)	  //��һ���ֽڵ���ģ
		{
			if(tmp&0x80)	  //ȡ��λ
			{  //��1   ��ǰ��ɫ
				tftWriteByte(1,TFT_CHAR_WINDOW_FORECOLOR>>8);
				tftWriteByte(1,TFT_CHAR_WINDOW_FORECOLOR);
			}
			else if(TFT_CHAR_TYPE)
			{  //����1 ����䱳��ɫ
				tftWriteByte(1,TFT_CHAR_WINDOW_BACKCOLOR>>8);
				tftWriteByte(1,TFT_CHAR_WINDOW_BACKCOLOR);
			}
			else
			{	 //����1 ����䱳��ɫ ��Ҫ����һ�����ص�λ��(���ö���λ)
				tftReadByte(1);
				tftReadByte(1);
			}
			tmp<<=1;
		}	
	}

	TFT_CHAR_X+=width; //һ���ַ������ ����һ���ַ���λ��
	//���߽紦��
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
//������:tftShowPic
//��  ��:��ʾ16λ���ɫͼƬ
//��  ��:��(StartX,StartY)����ʼ��ʾ,ͼƬ��СΪwidth*height,ͼƬȡģ��Ϊarray	
//����ֵ:
//��  ע:�ⲿ
//			ͼƬȡģ�������ʹ��Image2Lcd
//			ͼƬȡģ����Ϊˮƽɨ��
//			ɨ�����Ե��ֽ���ǰ
//��  ��:2012.3.6									
/********************************************************************************/
void tftShowPic(unsigned short StartX,unsigned short StartY,unsigned char width,unsigned char height,unsigned char *array)
{
	unsigned short i,max=width*height;
	tftSetWindow(StartX,StartY,width,height);
	for(i=0;i<max;i++)
	{
		tftWriteByte(1,array[2*i+1]);//���ڸ��ֽ��ں� ���Ը��ֽ�Ϊ�����ֽ�
		tftWriteByte(1,array[2*i]);	 //���ڵ��ֽ���ǰ ���Ե��ֽ�Ϊż���ֽ�
	}
}

/********************************************************************************/
//������:tftDrawRect
//��  ��:��TFT�ϻ�������
//��  ��:(StartX,StartY)Ϊ���Ͻ�����,(EndX,EndY)���½�����,widthΪ�߿�,colorΪ����ɫ
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.7									
/********************************************************************************/
void tftDrawRect(unsigned short StartX,
				unsigned short StartY,
				unsigned char EndX,
				unsigned char EndY,
				unsigned char width,
				unsigned short color)
{
	unsigned short size;
	//������
	size=(EndX-StartX+1)*width;
	tftSetWindow(StartX,StartY,EndX-StartX+1,width);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//������
	size=(EndX-StartX+1)*width;
	tftSetWindow(StartX,EndY,EndX-StartX+1,width);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//������
	size=(EndY-StartY+1)*width;
	tftSetWindow(StartX,StartY,width,EndY-StartY+1);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
	//������
	size=(EndY-StartY+1)*width;
	tftSetWindow(EndX,StartY,width,EndY-StartY+1);
	while(size--)
		tftWriteByte(1,color>>8),
		tftWriteByte(1,color);
}


/********************************************************************************/
//������:tftInvertColor
//��  ��:���趨�������е���ɫȡ����ʾ(color=~color)
//��  ��:StartXΪ����ĺ����� StartYΪ����������� widthΪ����Ŀ�� heightΪ����ĸ߶�
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.7									
/********************************************************************************/
/*void tftInvertColor(unsigned char StartX,unsigned char StartY,unsigned char width,unsigned char height)
{
	
} */


#endif
