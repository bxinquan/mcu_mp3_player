/*
*VS1003B(MP3/MIDI0/WMA)音乐解码芯片
*运行于33.1776MHz MCU之上
*SPI总线基本操作函数(SPI上升沿采样 下降沿输出)
*发送或接收都是高位在前
*HC6800 33.1776MHz最高可播放80kbps的MP3	当STC90C516开启"6T/双倍速"时,可流畅播放160kbps
*卞新权
*2012.3.8
*/


#ifndef __vs1003_h__ 
#define __vs1003_h__

#include<reg52.h>
#include"delay.h"


//名词解释
//SCI -- 串行命令接口
//SDI -- 串行数据接口
//ADPCM -- 自适应差分脉冲编码调制


//接口定义
sbit VS1003_XRST=P2^4;	//复位(低有效)
sbit VS1003_DREQ=P3^4;	//数据请求(高有效)
//接口定义(SPI)
sbit VS1003_XCS=P3^6;//SCI片选(低电平有效)
sbit VS1003_XDCS=P3^7;//SDI片选(低电平有效)
sbit VS1003_SCLK=P3^5;//时钟
sbit VS1003_MISO=P0^6;//输入(主入/从出)
sbit VS1003_MOSI=P0^7;//输出(主出/从入)



//vs1003相关宏定义 
#define VS1003_SCI_WRITE_CMD 	0x02   //SCI写命令
#define VS1003_SCI_READ_CMD		0x03   //SCI读命令
#define VS1003_SCI_MODE        0x0   //模式控制寄存器
#define VS1003_SM_DIFF         0x01  //MODE寄存器的相应位
#define VS1003_SM_JUMP         0x02   
#define VS1003_SM_RESET        0x04   //软件复位
#define VS1003_SM_OUTOFWAV     0x08   
#define VS1003_SM_PDOWN        0x10   //电源掉电
#define VS1003_SM_TESTS        0x20   //允许SDI测试
#define VS1003_SM_STREAM       0x40   
#define VS1003_SM_PLUSV        0x80    
#define VS1003_SM_DACT         0x100   	 //DCLK有效沿	0-上升沿 1-下降沿
#define VS1003_SM_SDIORD       0x200   	//SDI位序   0-高位在前 1-低位在前
#define VS1003_SM_SDISHARE     0x400   	//共享SPI片选　
#define VS1003_SM_SDINEW       0x800   	//新模式
#define VS1003_SM_ADPCM        0x1000   //PCM录音模式
#define VS1003_SM_ADPCM_HP     0x2000 	//PCM录音高通滤波允许
#define VS1003_SM_LINE_IN	   0x4000 	//PCM录音输入选择 0-麦克风 1-线路
#define VS1003_SCI_STATUS      0x1   //状态寄存器地址(以下为各功能寄存器地址)
#define VS1003_SCI_BASS        0x2   //内置高音/低音增益寄存器
#define VS1003_SCI_CLOCKF      0x3   //时钟频率设置寄存器
#define VS1003_SCI_DECODE_TIME 0x4   //每秒解码次数
#define VS1003_SCI_AUDATA      0x5   //采样率寄存器
#define VS1003_SCI_WRAM        0x6   //RAM写/读
#define VS1003_SCI_WRAMADDR    0x7   //RAM写读基地址
#define VS1003_SCI_HDAT0       0x8   //流头数据0
#define VS1003_SCI_HDAT1       0x9   //流头数据1
#define VS1003_SCI_AIADDR      0xa   //用户代码起始地址
#define VS1003_SCI_VOL         0xb   //音量控制
#define VS1003_SCI_AICTRL0     0xc   //应用控制寄存器0
#define VS1003_SCI_AICTRL1     0xd   //应用控制寄存器1
#define VS1003_SCI_AICTRL2     0xe   //应用控制寄存器2
#define VS1003_SCI_AICTRL3     0xf 	 //应用控制寄存器3
#define VS1003_SCI_DATA		   0x8	//录音模式的SCI_HDAT0的新功能 即录音缓存中的数据字
#define VS1003_SCI_BUF_FILL	   0x9	//录音模式的SCI_HDAT1的新功能 即录音缓存的状态


/********************************************************************************/
//函数名:vs1003_if_init						
//功  能:初始化VS1003接口				
//参  数:								
//返回值:								 
//备  注:内部
/********************************************************************************/
void vs1003_if_init(void)
{
	VS1003_XRST=1;
	VS1003_DREQ=1;
	VS1003_XDCS=1;
	VS1003_XCS=1;
	VS1003_SCLK=1;
	VS1003_MISO=1;
	VS1003_MOSI=1;
}


/********************************************************************************/
//函数名:vs1003_poweroff						
//功  能:全掉电模式VS1003仅消耗很小的功率		
//参  数:								
//返回值:							
//备  注:外部
/********************************************************************************/
/*void vs1003_poweroff(void)
{
	vs1003_if_init();
	VS1003_XRST=0;		//复位模式也是全掉电模式
} */


/********************************************************************************/
//函数名:vs1003_write_byte					
//功  能:向VS1003写入一个字节的数据(SPI方式)			
//参  数:								
//返回值:								
//备  注:内部
/********************************************************************************/
void vs1003_write_byte(unsigned char x)
{
	unsigned char i;
	for(i=8;i>0;i--)	//写入一个字节 高位在前 上升沿发送
	{
		VS1003_SCLK=0;
		VS1003_MOSI=x&0x80;
		VS1003_SCLK=1;
		x<<=1;
	}
}

/********************************************************************************/
//函数名:vs1003_read_byte			
//功  能:从VS1003读取一个字节的数据(SPI方式)		
//参  数:								
//返回值:							
//备  注:内部
/********************************************************************************/
unsigned char vs1003_read_byte(void)
{
	unsigned char i,res;
	VS1003_MISO=1;
	for(i=8;i>0;i--)	//读取一个字节　高字节在前　下降沿接收
	{
		res<<=1;
		VS1003_SCLK=0;
		res|=VS1003_MISO;
		VS1003_SCLK=1;
	}
	return res;
}




/********************************************************************************/
//函数名:vs1003_sci_write			
//功  能:通过SCI接口向VS1003功能寄存器写入数据		
//参  数:reg_addr为功能寄存器地址.reg_data为16位寄存器数据	
//返回值:无							
//备  注:内部
/********************************************************************************/
void vs1003_sci_write(unsigned char reg_addr,unsigned short reg_data)
{
	while(VS1003_DREQ==0);			//等待就绪
//	VS1003_XDCS=1; //xCS =0，xDCS = 1，写命令，写完后xCS =1；
	VS1003_XCS=0;
	vs1003_write_byte(VS1003_SCI_WRITE_CMD); //写WRITE命令
	vs1003_write_byte(reg_addr);			 //写寄存器地址
	vs1003_write_byte(reg_data>>8);			 //写高字节数据
	vs1003_write_byte(reg_data&0xff);			 //写低字节数据
	VS1003_XCS=1;
}



/********************************************************************************/
//函数名:vs1003_sci_read			
//功  能:通过SCI接口从VS1003功能寄存器读数据	
//参  数:reg_addr为功能寄存器地址		
//返回值:16位数据				
//备  注:内部
/********************************************************************************/
unsigned short vs1003_sci_read(unsigned char reg_addr)
{
	unsigned short res;
	while(VS1003_DREQ==0);	//等待就绪
//	VS1003_XDCS=1;	//xCS =0，xDCS = 1，写命令，写完后xCS =1；
	VS1003_XCS=0; 		
	vs1003_write_byte(VS1003_SCI_READ_CMD);	//写READ命令
	vs1003_write_byte(reg_addr);		   	//写寄存器地址
	res=vs1003_read_byte()<<8;			   	//读高字节数据
	res|=vs1003_read_byte();			   	//读低字节数据
	VS1003_XCS=1;
	return res;
}




/********************************************************************************/
//函数名:vs1003_sdi_write		
//功  能:通过SDI接口向VS1003写入待解码数据(包括MP3/MIDI0/WMA)
//参  数:data_ptr为指向数据的内存指针,length为数据字节数
//返回值:
//备  注:外部,直接将MP3,MIDI0或WMA文件的数据写入VS1003解码即可
/********************************************************************************/
void vs1003_sdi_write(unsigned char* data_ptr,unsigned short length)
{
//	VS1003_XCS=1; //xCS =1,xDCS =0，写数据，写完后xDCS =1；
	VS1003_XDCS=0;
	while(length--)
	{
		while(VS1003_DREQ==0);	//等待就绪	
		vs1003_write_byte(*data_ptr); //写字节数据
		data_ptr++;
	}
	VS1003_XDCS=1;
}


/********************************************************************************/
//函数名:vs1003_clockf
//功  能:换算VS1003时钟频率
//参  数:sc_mult为时钟倍频索引[0~7] sc_add为允许倍频的增量索引[0~3] sc_freq为输入频率因子[0 ~ 2^10-1]
//返回值:VS1003的CLOCKF寄存器的值	
//备  注:内部
/********************************************************************************/
/*---------------------------------------------*/
//			sc_mult			SC_MULT
//			0		 		1.0
//			1				1.5
//			2				2.0
//			3				2.5
//			4				3.0
//			5				3.5
//			6				4.0
//			7				4.5
/*---------------------------------------------*/
/*---------------------------------------------*/
//			sc_add			SC_ADD
//			0		 		0
//			1				0.5
//			2				1.0
//			3				1.5
/*---------------------------------------------*/
/********************************************************************************/
unsigned short vs1003_clockf(unsigned char sc_mult,unsigned char sc_add,unsigned short sc_freq)
{
	unsigned short clockf;
	sc_mult&=0x07;
	sc_add&=0x03;
	sc_freq&=0x03ff;
	clockf=sc_mult;
	clockf<<=2;
	clockf|=sc_add;
	clockf<<=11;
	clockf|=sc_freq;
	return clockf;
}
/********************************************************************************/
//关于时钟频率的名词
//XTALI:	晶振输入频率
//SC_MULTI:	倍频系数
//SC_ADD:	允许倍频增量
//CLKI:		内部工作时钟频率
//计算关系:
//		XTALI = sc_freq * 4000 + 8000000 (Hz) 注:当sc_freq=0时 则XTALI=12.288MHz 
//		CLKI = XTALI * SC_MULT 
//		最大CLKI = XTALI * (SC_MULT + SC_ADD)
//		实际频率范围在[CLKI ~ 最大CLKI]	  由硬件自动调整
//例:当sc_mult=4 , sc_add=3 , sc_freq=0
//	则:SC_MULT=3.0 , SC_ADD=1.5 , XTALI=12.288MHz
//	则:CLKI=12.288MHz*3.0=36.864MHz
//	则:最大CLKI=12.288MHz*(3.0+1.5)=55.296MHz
/********************************************************************************/



/********************************************************************************/
//函数名:vs1003_flush
//功  能:写2048个0清空VS1003数据缓冲区,为下一首音乐做准备
//参  数:
//返回值:	
//备  注:外部
/********************************************************************************/
void vs1003_flush(void)
{
	unsigned char code dat[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char count=128;
	while(count--)//写2048个0
		vs1003_sdi_write(dat,16);
}


/********************************************************************************/
//函数名:vs1003_set_volume		
//功  能:VS1003放音音量设置		
//参  数:leftVol为左声道音量,rightVol为右声道音量(0~254)
//返回值:					
//备  注:外部
/********************************************************************************/
void vs1003_set_volume(unsigned char leftVol,unsigned char rightVol)
{
	leftVol=254-leftVol%255;	//屏蔽255后取反数
	rightVol=254-rightVol%255;
	vs1003_sci_write(VS1003_SCI_VOL,(leftVol<<8)|(rightVol));//写入音量控制寄存器
}


/********************************************************************************/
//函数名:vs1003_set_surround
//功  能:VS1003虚拟环绕设置			
//参  数:x为0关闭虚拟环绕,x为1时开启虚拟环绕		
//返回值:							
//备  注:外部
/********************************************************************************/
void vs1003_set_surround(bit x)
{
	unsigned short sm;
	sm=vs1003_sci_read(VS1003_SCI_MODE);
	if(sm&0x0001==x)
		return;
	sm&=0xfffe;
	sm|=x;
	vs1003_sci_write(VS1003_SCI_MODE,sm);
}


/********************************************************************************/
//函数名:vs1003_set_bass
//功  能:VS1003重音控制
//参  数:sb_amplitude为振幅(低音加重,1dB步进,0~15,0关闭)即单位为1dB
//		 sb_freqlimit为频限(最低频限10Hz步进,2~15)即单位为10Hz
//返回值:
//备  注:外部								
/********************************************************************************/
void vs1003_set_bass(unsigned char sb_amplitude,unsigned char sb_freqlimit)
{
	unsigned short sci_bass=0,tmp;
	sb_amplitude&=0x0f;
	sb_amplitude<<=4;
	sb_freqlimit&=0x0f;
//	if(sb_freqlimit<2)
//		sb_freqlimit=2;
	sci_bass=sb_amplitude|sb_freqlimit;
	tmp=vs1003_sci_read(VS1003_SCI_BASS);
	if((tmp&0x00ff)==sci_bass)
		return;
	sci_bass|=tmp&0xff00;
	vs1003_sci_write(VS1003_SCI_BASS,sci_bass);
}

/********************************************************************************/
//函数名:vs1003_set_treble
//功  能:VS1003高音控制
//参  数:st_amplitude为振幅(高音控制,1.5dB步进,-8~7,0关闭)即单位为1.5dB
//		 st_freqlimit为频限(最低频限1000Hz步进,0~15)即单位为1KHz
//返回值:
//备  注:外部								
/********************************************************************************/
void vs1003_set_treble(char st_amplitude,unsigned char st_freqlimit)
{
	unsigned short sci_bass=0,tmp;
	st_amplitude&=0x0f;
	st_amplitude<<=4;
	st_freqlimit&=0x0f;
	sci_bass=(st_amplitude|st_freqlimit)<<8;
	tmp=vs1003_sci_read(VS1003_SCI_BASS);
	if((tmp&0xff00)==sci_bass)
		return;
	sci_bass|=tmp&0x00ff;
	vs1003_sci_write(VS1003_SCI_BASS,sci_bass);
}



/********************************************************************************/
//函数名:vs1003_set_pause
//功  能:VS1003暂停
//参  数:x为1时暂停,x为0时继续播放
//返回值:
//备  注:外部  使用软掉电暂停(VS1003_SM_PDOWN)能即时暂停且能即时继续播放								
/********************************************************************************/
void vs1003_set_pause(bit x)
{
	unsigned short sm;
	sm=vs1003_sci_read(VS1003_SCI_MODE);
	if(x)
		sm|=0x0010;
	else
		sm&=0xffef;
	vs1003_sci_write(VS1003_SCI_MODE,sm);	
}

/********************************************************************************/
//函数名:vs1003_get_file_type
//功  能:获取VS1003正在解码的文件类型
//参  数:
//返回值:正在解码的文件类型(0-未知,1-WAV,2-WMA,3-MIDI0,4-MP3)
//备  注:外部 ,通过读取SCI_HDAT1识别
//			根据文件类型不同,有些文件可能需经过文件前几百个字节后才能识别出
/********************************************************************************/
#define VS1003_NUL 0
#define VS1003_WAV 1
#define VS1003_WMA 2
#define VS1003_MID 3
#define VS1003_MP3 4
unsigned char vs1003_get_file_type(void)
{
	unsigned short hdat1=vs1003_sci_read(VS1003_SCI_HDAT1);
	hdat1&=0xffe0;
	switch(hdat1)
	{
		case 0x7660:
			return VS1003_WAV;
		case 0x5740:
			return VS1003_WMA;
		case 0x4d40:
			return VS1003_MID;
		case 0xffe0:
			return VS1003_MP3;
		default:
			break;
	}
	return VS1003_NUL;
}


/********************************************************************************/
//函数名:vs1003_get_samprate
//功  能:获取VS1003正在解码的采样率(KHz)
//参  数:
//返回值:正在解码的采样率
//备  注:外部 ,通过读取SCI_AUDATA前15位识别
//			根据文件类型不同,有些文件可能需经过文件前几百个字节后才能识别出
/********************************************************************************/
//unsigned short vs1003_get_samprate(void)
//{
//	return vs1003_sci_read(VS1003_SCI_AUDATA)&0xfffe;
//}
#define vs1003_get_samprate() (vs1003_sci_read(VS1003_SCI_AUDATA)&0xfffe) 



/********************************************************************************/
//函数名:vs1003_get_channel
//功  能:获取VS1003正在解码的声道数
//参  数:
//返回值:正在解码的声道数(0-单声道,1-立体声)
//备  注:外部 ,通过读取SCI_AUDATA最后1位识别
//			根据文件类型不同,有些文件可能需经过文件前几百个字节后才能识别出
/********************************************************************************/						  
#define VS1003_MONO 0			//单声道
#define VS1003_DIMENSIONAL 1 	//立体声
//bit vs1003_get_channel(void)
//{
//	return vs1003_sci_read(VS1003_SCI_AUDATA)&0x0001;
//}
#define vs1003_get_channel() (vs1003_sci_read(VS1003_SCI_AUDATA)&0x0001) 



/********************************************************************************/
//函数名:vs1003_get_bitrate
//功  能:获取VS1003正在解码的比特率(仅用于MP3与WMA)
//参  数:
//返回值:正在解码的比特率kbps(无法得到时返回0)
//备  注:外部 ,对于WMA通过读取SCI_HDAT0识别,对于MP3通过读取SCI_HDAT1及SCI_HDAT0识别
//			根据文件类型不同,有些文件可能需经过文件前几百个字节后才能识别出
/********************************************************************************/
unsigned short code VS1003_MP3_BITRATE[][2]=   //MP3比特率表
{
	{0   ,   0},
	{32  ,   8},
	{40  ,  16},
	{48  ,  24},
	{56  ,  32},
	{64  ,  40},
	{80  ,  48},
	{96  ,  56},
	{112 ,  64},
	{128 ,  80},
	{160 ,  96},
	{192 , 112},
	{224 , 128},
	{256 , 144},
	{320 , 160},
	{0   ,   0}
};
unsigned short vs1003_get_bitrate(void)
{
	unsigned char hdat1_id,hdat0_rate;
	switch(vs1003_get_file_type())
	{
		case VS1003_WMA:
			return (vs1003_sci_read(VS1003_SCI_HDAT0)<<3)/1000.0+0.5;//将byte/s转换成kbps
		case VS1003_MP3:
			hdat1_id=(vs1003_sci_read(VS1003_SCI_HDAT1)&0x0018)>>3;
			hdat1_id=(hdat1_id>2)?0:1;
			hdat0_rate=vs1003_sci_read(VS1003_SCI_HDAT0)>>12;
			return VS1003_MP3_BITRATE[hdat0_rate][hdat1_id];
		default:
			break;
	}
	return 0;	
}




/********************************************************************************/
//函数名:vs1003_sine_test				
//功  能:VS1003正弦测试	如果能持续听到一高一低的声音，证明测试通过			
//参  数:							
//返回值:						
//备  注:内部 用于测试VS1003是否正常
/********************************************************************************/
void vs1003_sine_test(void)
{
	unsigned char code test1[]={0x53,0xef,0x6e,0x24,0,0,0,0};
	unsigned char code test2[]={0x53,0xef,0x6e,0x44,0,0,0,0};
	unsigned char code end[]={0x45,0x78,0x69,0x74,0,0,0,0};
	vs1003_if_init();
	VS1003_XRST=0;
	delay1ms(100);//复位100ms
	VS1003_XRST=1;
	vs1003_sci_write(VS1003_SCI_MODE,VS1003_SM_SDINEW|VS1003_SM_TESTS);//开启测试模式
	vs1003_sdi_write(test1,8);//高音
	delay1ms(300);
	vs1003_sdi_write(end,8);  
	delay1ms(300);
	vs1003_sdi_write(test2,8); //低音
	delay1ms(300);
	vs1003_sdi_write(end,8);
}


/********************************************************************************/
//函数名:vs1003_soft_reset				
//功  能:VS1003软件复位				
//参  数:							
//返回值:						
//备  注:外部 软复位时不会丢失寄存器数据 建议每首歌放完后 进行一次软复位
/********************************************************************************/
void vs1003_soft_reset(void)
{
	vs1003_flush();
	vs1003_sci_write(VS1003_SCI_MODE,VS1003_SM_SDINEW|VS1003_SM_RESET);//新模式+软复位
	delay1ms(5);
//	vs1003_flush();
}


/********************************************************************************/
//函数名:vs1003_init					
//功  能:VS1003硬件复位及初始化					
//参  数:							
//返回值:						
//备  注:外部  初始化不成功将不返回
/********************************************************************************/
void vs1003_init(void)
{
	unsigned char code dat[]={0,0,0,0};
	vs1003_if_init();	//接口复位

	do{
		VS1003_XRST=0;	   	//硬复位
		delay1ms(100);	   	//延时100ms
		VS1003_XRST=1;		//硬复位结束
		vs1003_sci_write(VS1003_SCI_MODE,VS1003_SM_SDINEW);	//新模式 (基本参数)
		vs1003_sci_write(VS1003_SCI_CLOCKF,vs1003_clockf(4,3,0));	//设置vs1003的时钟,3倍频(基本参数)
	}while(vs1003_sci_read(VS1003_SCI_MODE)!=VS1003_SM_SDINEW);

//	vs1003_sci_write(VS1003_SCI_AUDATA,48000+1); 		//采样率48k，立体声
//	vs1003_set_treble(0,0);								//设置高音(典型值7,10)
//	vs1003_set_bass(0,0);								//设置重音(典型值15,6)
//	vs1003_set_volume(250,250);							//设置默认音量

	vs1003_sdi_write(dat,4);   //写4个0
}


/********************************************************************************/
//函数名:vs1003_start_adpcm					
//功  能:启动VS1003adpcm录音模式					
//参  数:select_in为0时从麦克风录音 select_in为1时从LINE录音
//			high_pass为0时不开启高通滤波 high_pass为1时开启高通滤波							
//返回值:						
//备  注:外部
//		ADPCM录音采样率Fs = CLKI / (256 * d) ;其中CLKI(=SCI_MULT*XTALI)是VS1003工作频率,d是写入SCI_AICTRL0的分频值
//		注:
//		   通道数:				1个
//		   每秒样本数(采样率):	16000Hz
//		   每秒字节数(字节速率):8110B/s			
//		   每样本数据位数:	 	4bit
//		   数据块大小:			256字节
//		   自动增益:			是 
/********************************************************************************/
void vs1003_start_adpcm(bit select_in , bit high_pass)
{
	unsigned short mode;
	mode = VS1003_SM_SDINEW | VS1003_SM_RESET | VS1003_SM_ADPCM ;  //新模式+软件复位+录音允许
	if(select_in)
		mode |= VS1003_SM_LINE_IN;	//从LINE中录音
	if(high_pass)
		mode |= VS1003_SM_ADPCM_HP;	   //启用高通滤波

	vs1003_sci_write(VS1003_SCI_CLOCKF , vs1003_clockf(2,3,0));	// 2.0x 12.288MHz
	vs1003_sci_write(VS1003_SCI_AICTRL0 , 6);	//12是写入SCI_AICTRL0的分频值 2*12288000/(256*6)=16000Hz  12=8kHz 8=12kHz 6=16kHz
	vs1003_sci_write(VS1003_SCI_AICTRL1 , 0); 	//开启自动增益
	vs1003_sci_write(VS1003_SCI_MODE , mode);		//激活录音模式
	delay1ms(10);
}


/********************************************************************************/
//函数名:vs1003_stop_adpcm					
//功  能:停止VS1003adpcm录音模式					
//参  数:						
//返回值:						
//备  注:外部
/********************************************************************************/
void vs1003_stop_adpcm(void)
{
	vs1003_sci_write(VS1003_SCI_MODE,VS1003_SM_SDINEW|VS1003_SM_RESET);//新模式+软复位
	delay1ms(5);
	vs1003_sci_write(VS1003_SCI_CLOCKF,vs1003_clockf(4,3,0));	//设置vs1003的时钟,3倍频(基本参数)
}



/********************************************************************************/
//函数名:vs1003_get_adpcm					
//功  能:获取一个VS1003录音ADPCM数据块(256字节)					
//参  数:pcm_buf是用来存放录音数据(256字节)				
//返回值:						
//备  注:外部
//		每128word为一个block，每block的第四个数据应该是0x00 即pcm_buf[3]==0x00
/********************************************************************************/
void vs1003_get_adpcm(unsigned char *pcm_buf)
{
	unsigned short dat,i;
	do{
		dat=vs1003_sci_read(VS1003_SCI_BUF_FILL);	  //获取PCM缓冲中的数据字个数
	}while(dat<128 || dat>=896);					//一个数据块为128个字大小(256个字节) 超过896后容易采样走样

	i=0;
	while(i<256)			  //取一个ADPCM数据块256字节
	{
		dat = vs1003_sci_read(VS1003_SCI_DATA);	  //获取PCM缓冲中的数据
		pcm_buf[i++] = dat >> 8;		 //先取高字节
		pcm_buf[i++] = dat & 0xff;		  //再取低字节
	}
}


//WAV文件头模板
//使用本模板后用户仅修改ChunkSize,NumOfSamples,SubChunk3Size域值
//RIFF头模板
//模板已设置:
//			NumOfChannels=1
//			SampleRate=16000
//			ByteRate=8110B/s
//			BlockAlign=256
//			BitsPerSample=4	
//			ExtraData=505
//本模板使用fact块进行了扩展(使文件头与扇区对齐)
unsigned char code RIFF_header[512] = {
0x52, 0x49, 0x46, 0x46, 0x1c, 0x10, 0x00, 0x00,
0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20, /*|RIFF....WAVEfmt |*/
0x14, 0x00, 0x00, 0x00, 0x11, 0x00,
/************下面2个字节为通道数(声道数1)**********/
0x01, 0x00,
/***********下面4个字节为采样率(16000Hz)***********/
0x80, 0x3e, 0x00, 0x00,
/***********下面4个字节为字节速率(8110字节每秒)*****************/
0xae, 0x1f, 0x00, 0x00, 						/*|........@......|*/
/**********下面2个字节为数据块的调整数(256B)********/
0x00, 0x01, 
/**********下面2个字节为每个样本的位数(4bit)********/
0x04, 0x00, 
0x02, 0x00, 0xf9, 0x01,
0x66, 0x61, 0x63, 0x74, 0xc8, 0x01, 0x00, 0x00, /*|.......fact....|*/
0x5c, 0x1f, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x64, 0x61, 0x74, 0x61, 0xe8, 0x0f, 0x00, 0x00
};


//RIFF/WAV文件头结构
typedef struct 	   
{ 
	unsigned char ChunkID[4];	 	//块标志"RIFF"
	unsigned long ChunkSize;		//块大小(文件大小-8) 
	unsigned char Format[4];		//格式"WAVE" 
	unsigned char SubChunk1ID[4];	//子块1标志"fmt " 
	unsigned long SubChunk1Size;	//子块1大小	0x14
	unsigned short AudioFormat;		//格式类别（11H为IMA_ADPCM形式的声音数据)
	unsigned short NumOfChannels;	//通道数，单声道为1，双声道为2 (0x01)
	unsigned long  SampleRate;	 //采样率（每秒样本数），表示每个通道的播放速度	 16000Hz
	unsigned long  ByteRate; 	//波形音频数据传送速率，其值为通道数×每秒样本数×每样本的数据位数／8。播放软件利用此值可以估计缓冲区的大小。	   8110B/s
	unsigned short BlockAlign ;  //数据块的调整数（按字节算的），播放软件需要一次处理多个该值大小的字节数据，以便将其值用于缓冲区的调整。  256
	unsigned short BitsPerSample; 	//每样本的数据位数，表示每个声道中各个样本的数据位数。如果有多个声道，对每个声道而言，样本大小都一样。	4
	unsigned short ByteExtraData;	//0x02
	unsigned short ExtraData;		//每块的采样数 505
	unsigned char  SubChunk2ID[4];	//子块2标志"fact"
	unsigned long  SubChunk2Size;	//子块2大小 4+452=456
	unsigned long  NumOfSamples;	//采样的个数
	unsigned char  fillZero[452];	//填充452个0使文件头独占一个扇区
	unsigned char  SubChunk3ID[4];	//子块3标志"data" 
	unsigned long  SubChunk3Size; 	//语音数据的长度 (文件长度-60)
}RiffHeader;	 //共512个字节
//设共有N个ADPCM数据块(每个块大小为256字节)
//ChunkSize = N * 256 + 52 (即FileSize - 8)
//SampleRate = 	Fs (ADPCM采样频率 在vs1003_start_adpcm中有设置8000Hz)
//ByteRate = (Fs *256)/505	
//NumOfSamples = N * 505
//SubChunk3Size = N * 256


#endif

