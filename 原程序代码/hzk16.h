


/*
*从SD卡的FAT32系统中根目录下的hzk16文件中获取GB2312汉字的16*16字模码
*使用时请先初始化SD卡及FAT32系统
*支持GB2312
*支持UNICODE汉字部分(可选,需初始化hzk16_uni_init)
*2012.3.8
*卞新权
*/
#ifndef __hzk16_h__
#define __hzk16_h__


#include<reg52.h>
#include"sd.h"
#include"fat.h"


/***************************************************************************/
//函数名:hzk16_init
//功  能:初始化公共变量HZK16_START_SECTOR(汉字库文件所在的起始扇区)
//参  数:
//返回值:成功返回0,错误返回1
//备  注:汉字库文件(hzk16)已存入SD上的FAT32系统的根目录下,且sd卡与FAT已初始化
/**************************************************************************/
unsigned long HZK16_START_SECTOR=0;
bit hzk16_init(void)
{
	FAT_FILE hzk16;
	if(fat_open_file("HZK16      ",1,&hzk16))  //尝试打开HZK16文件
		return 1;
	HZK16_START_SECTOR=hzk16.current_sector;
	return 0;
}




/***************************************************************************/
//函数名:hzk16_gbZiMo
//功  能:获得GB2312汉字(gb_h,gb_l)的16*16的字模放在zimo数组中(模向逐行32字节字模数据)
//参  数:gb_h为汉字的GB2312码高字节,gb_l为汉字的GB2312码低字节
//			zimo为存储字模的数组
//返回值:成功返回0,错误返回1
//备  注:
/**************************************************************************/
bit hzk16_gbZiMo(unsigned char gb_h,unsigned char gb_l,unsigned char* zimo)
{
	unsigned long offset;//存储绝对偏移
	if(gb_h<0xa0)
		return 1;//无效区号
	if(gb_l<0xa0)
		return 1;//无效位号
	gb_h-=0xa0;//获得区号
	gb_l-=0xa0;//获得位号
	offset=(94ul*(gb_h-1)+(gb_l-1))*32ul;//获得偏移 (两个ul非常重要 不能省略)
	if( sd_read_sector_N(HZK16_START_SECTOR+(offset>>9),zimo,offset%512,32) )
		return 1;//读SD错误
	return 0;
}




/***************************************************************************/
//函数名:hzk16_unigb_init
//功  能:初始化公共变量HZK16_UNIGB_START_SECTOR(UNICODE->GB2312	对应表文件所在的起始扇区)
//参  数:
//返回值:成功返回0,错误返回1
//备  注:对应表文件(unigb)已存入SD上的FAT32系统的根目录下,且sd卡与FAT已初始化
/**************************************************************************/
unsigned long HZK16_UNIGB_START_SECTOR=0;
bit hzk16_uni_init(void)
{
	FAT_FILE unigb;
	if(fat_open_file("UNIGB      ",1,&unigb))  //尝试打开HZK16文件
		return 1;
	HZK16_UNIGB_START_SECTOR=unigb.current_sector;
	return 0;
}



/***************************************************************************/
//函数名:hzk16_uniZiMo
//功  能:获得UNICODE汉字(uni_h,uni_l)的16*16的字模放在zimo数组中(模向逐行32字节字模数据)
//参  数:uni_h为汉字的UNICODE码高字节,uni_l为汉字的UNICODE码低字节
//			zimo为存储字模的数组
//返回值:成功返回0,错误返回1
//备  注:
/**************************************************************************/
bit hzk16_uniZiMo(unsigned char uni_h,unsigned char uni_l,unsigned char* zimo)
{
	unsigned long offset;//存储绝对偏移
	unsigned char gb[2];
	unsigned short unicode=((unsigned short)uni_h<<8)|uni_l;
	if(unicode<0x4e00)
		return 1;//无效区号

	offset=(unicode-0x4e00)<<1;//获得偏移
	if( sd_read_sector_N(HZK16_UNIGB_START_SECTOR+(offset>>9),gb,offset%512,2) )
		return 1;//读SD错误
	return hzk16_gbZiMo(gb[0],gb[1],zimo);
}




#endif