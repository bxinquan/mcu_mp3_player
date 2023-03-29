

/*
*SD存储卡读写(使用SPI模式)
*本程序使用了xdata区的内存 所以请使用用xdata不低于512bytes的MCU
*在写扇区时若遇见写错误 可尝试重新初始化后再写 若出现多次重新初始化后仍
	写失败 则需要跳过该扇区
	读不容易出错,出错时可尝试多读几次
	11.0592MHz读平均6.9KB/s(10MB验证) 写平均6.5KB/s(10MB验证)
	33.1776MHz读平均160kbps以上(1MB验证)
*更新日期:2012.3.21
*卞新权
*/



#ifndef __sd_h__ 
#define __sd_h__


#include<reg52.h>



//接口定义(SPI模式)
sbit SD_SPI_CS   = P2^0;//片选(低电平有效)
sbit SD_SPI_CLK  = P2^1;//时钟
sbit SD_SPI_MISO = P2^2;//输入(主入/从出)
sbit SD_SPI_MOSI = P2^3;//输出(主出/从入)



//SPI通信时的字节缓冲器
//使用位寻址可提高速度
unsigned char bdata SD_SPI_BUF;	 
sbit SD_SPI_BUF_0 = SD_SPI_BUF^0;
sbit SD_SPI_BUF_1 = SD_SPI_BUF^1;
sbit SD_SPI_BUF_2 = SD_SPI_BUF^2;
sbit SD_SPI_BUF_3 = SD_SPI_BUF^3;
sbit SD_SPI_BUF_4 = SD_SPI_BUF^4;
sbit SD_SPI_BUF_5 = SD_SPI_BUF^5;
sbit SD_SPI_BUF_6 = SD_SPI_BUF^6;
sbit SD_SPI_BUF_7 = SD_SPI_BUF^7; 



//用于控制超时的参数
//此项需根据硬件性能及频率进行修改
//若设置不对将导致复位初始化失败及读写错误
#define SD_SLOW_SPEED_DELAY 50				//初始化及复位期间 低速模式下的时序等待次数	 降低此项可提高复位及初始化速度
#define SD_INIT_CMD_REPEAT_TIMES 200		//初始化及复位期间 尝试重写命令的次数	降低此项可提高复位及初始化速度
#define SD_WRITED_BUSY_DELAY 10000			//写扇区后 等待完成的次数 	 此项不得低于6000
#define SD_CMD_RESPONSE_DELAY 30			//写命令后 等待回应的次数	 此项不得低于15
#define SD_WAIT_0XFE_DELAY 100				//当发出读取或写入命令后 数据起始标志0XFE的等待次数	降低此项可提高写扇区速度 此项不得低于100


/********************************************************************************/
//函数名:sd_spi_init
//功  能:SPI总线复位
//参  数:		
//返回值:
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
void sd_spi_reset(void)
{
	SD_SPI_CS = 1;
	SD_SPI_CLK = 0;
	SD_SPI_MISO = 1;
	SD_SPI_MOSI = 1;
}


/********************************************************************************/
//函数名:sd_spi_write
//功  能:向SD写入一个字节的数据	(SPI模式)
//参  数:待写入的字节		
//返回值:
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
void sd_spi_write(unsigned char x)
{
	SD_SPI_BUF = x;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_7;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_6;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_5;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_4;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_3;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_2;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_1;
	SD_SPI_CLK = 1;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_0;
	SD_SPI_CLK = 1;	
}


/********************************************************************************/
//函数名:sd_spi_write_slow
//功  能:向SD写入一个字节的数据	(慢速SPI模式)
//参  数:待写入的字节		
//返回值:
//备  注:内部 在复位及初始化期间使用
//更  新:2012.3.21									
/********************************************************************************/
void sd_spi_write_slow(unsigned char x)
{
	unsigned short times;
	SD_SPI_BUF = x;
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_7;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_6;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_5;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_4;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_3;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_2;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	 
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	
	SD_SPI_CLK = 0;
	SD_SPI_MOSI = SD_SPI_BUF_0;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
}

/********************************************************************************/
//函数名:sd_spi_read
//功  能:从SD读取一个字节的数据	(SPI模式)
//参  数:		
//返回值:读出的字节
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
unsigned char sd_spi_read(void)
{
	SD_SPI_MISO = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_7 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_6 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_5 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_4 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_3 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_2 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_1 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_0 = SD_SPI_MISO;
	SD_SPI_CLK = 1;

	return SD_SPI_BUF;
}


/********************************************************************************/
//函数名:sd_spi_read_slow
//功  能:从SD读取一个字节的数据	(慢速SPI模式)
//参  数:		
//返回值:读出的字节
//备  注:内部 在复位及初始化期间使用
//更  新:2012.3.21									
/********************************************************************************/
unsigned char sd_spi_read_slow(void)
{
	unsigned short times;
	SD_SPI_MISO = 1;

	SD_SPI_CLK = 0;
	SD_SPI_BUF_7 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_6 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	  
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_5 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	  
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_4 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_3 = SD_SPI_MISO;	 
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_2 = SD_SPI_MISO;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_1 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	  
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	SD_SPI_CLK = 0;
	SD_SPI_BUF_0 = SD_SPI_MISO;
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);
	SD_SPI_CLK = 1;	 
	for(times=SD_SLOW_SPEED_DELAY ; times>0 ; times--);

	return SD_SPI_BUF;
}




/********************************************************************************/
//函数名:sd_write_cmd
//功  能:写入一个命令字并返回回应R1
//参  数:命令数组		
//返回值:回应R1,出错返回0xff
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
unsigned char sd_write_cmd(unsigned char* cmd)
{
	unsigned char r1;
	unsigned short times;

	for(times=0;times<6;times++)//写命令字
		sd_spi_write(cmd[times]);
	for(times=SD_CMD_RESPONSE_DELAY ; times>0 ; times--)//等待回应
		if((r1=sd_spi_read())!=0xff)
			break;

	if(times == 0)	//超时
		return 0xff;
	else
		return r1;	
}


/********************************************************************************/
//函数名:sd_write_cmd_slow
//功  能:慢速写入一个命令字并返回回应R1
//参  数:命令数组		
//返回值:回应R1,出错返回0xff
//备  注:内部 复位及初始期间使用
//更  新:2012.3.21									
/********************************************************************************/
unsigned char sd_write_cmd_slow(unsigned char* cmd)
{
	unsigned char r1;
	unsigned short times;

	for(times=0;times<6;times++)//写命令字
		sd_spi_write_slow(cmd[times]);
	for(times=SD_CMD_RESPONSE_DELAY ; times>0 ; times--)//等待回应
		if((r1=sd_spi_read_slow())!=0xff)
			break;

	if(times == 0)	//超时
		return 0xff;
	else
		return r1;	
}



/********************************************************************************/
//函数名:sd_reset
//功  能:SD存储卡复位
//参  数:		
//返回值:正确返回0,错误返回1
//备  注:外部
//更  新:2012.3.21									
/********************************************************************************/
bit sd_reset(void)
{
	unsigned char code cmd0[]={0x40,0,0,0,0,0x95};//复位及转换到SPI模式命令cmd0 
	unsigned short times;

	sd_spi_reset();	  //接口复位

	for(times=15 ; times>0 ; times--)  //根据时序图 复位命令之前发送至少74个时钟 唤醒SPI模式
		sd_spi_write_slow(0xff);	

	SD_SPI_CS = 0;	//打开片选	
	for(times=SD_INIT_CMD_REPEAT_TIMES ; times>0 ; times--)//重写等待直到正确响应或超时
		if(sd_write_cmd_slow(cmd0)==0x01)
			break;
	SD_SPI_CS = 1;//关闭片选

	sd_spi_write_slow(0xff);//依照时序补充8个时钟

	if(times == 0) //超时
		return 1;
	//复位完成
	return 0;
}



/********************************************************************************/
//函数名:sd_init
//功  能:SD存储卡复位及初始化
//参  数:		
//返回值:正确返回0,错误返回1
//备  注:外部
//更  新:2012.3.21									
/********************************************************************************/
bit sd_init(void)
{
	unsigned char code cmd1[]={0x41,0,0,0,0,0xff};//初始化cmd1
	unsigned short times;

	if(sd_reset())//复位SD卡
		return 1;

	sd_spi_write_slow(0xff);//提高兼容性

	SD_SPI_CS = 0; 	//打开片选
	for(times=SD_INIT_CMD_REPEAT_TIMES ; times>0 ; times--)//重写等待直到正确响应或超时
		if(sd_write_cmd_slow(cmd1)==0x00)
			break;
	SD_SPI_CS = 1;  //关闭片选

	sd_spi_write_slow(0xff);//依照时序补充8个时钟

	if(times == 0) //超时
		return 1;
	//初始化完成
	return 0;
}



/********************************************************************************/
//函数名:sd_read_CID
//功  能:读SD存储卡CID寄存器
//参  数:用于保存CID信息的16字节数组		
//返回值:正确返回0,错误返回1
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
bit sd_read_CID(unsigned char* cid)
{
	unsigned char code cmd10[]={0x40+10,0,0,0,0,0xff};//CMD10
	unsigned short times;

	sd_spi_write(0xff);//提高兼容性

	SD_SPI_CS = 0;	//打开片选
	if(sd_write_cmd(cmd10) != 0)//命令应答错误
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);//依照时序补充8个时钟
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//超时
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);	//依照时序补充8个时钟
		return 1;
	}

	for(times=0;times<16;times++)//读数据
		cid[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//关闭片选
	sd_spi_write(0xff);	  //依照时序补充8个时钟
	return 0;
}



/********************************************************************************/
//函数名:sd_read_CSD
//功  能:读SD存储卡CSD寄存器
//参  数:用于保存CSD信息的16字节数组		
//返回值:正确返回0,错误返回1
//备  注:内部
//更  新:2012.3.21									
/********************************************************************************/
bit sd_read_CSD(unsigned char* csd)
{
	unsigned char code cmd9[]={0x40+9,0,0,0,0,0xff};//CMD9
	unsigned short times;

	sd_spi_write(0xff);//提高兼容性

	SD_SPI_CS = 0;	//打开片选
	if(sd_write_cmd(cmd9))
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//超时
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);	//依照时序补充8个时钟
		return 1;
	}
	
	for(times=0;times<16;times++)//读数据
		csd[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//关闭片选
	sd_spi_write(0xff);	  //依照时序补充8个时钟
	return 0;	
}


//SD存储卡信息结构
typedef struct
{
	unsigned long id;//产品序列号
	unsigned short block_len;//块大小 单位Byte
	unsigned long block_count;//总块数	
}SD_INFO;


/********************************************************************************/
//函数名:sd_read_info
//功  能:读SD存储卡信息数据
//参  数:用于保存存储卡信息数据的指针		
//返回值:正确返回0,错误返回1
//备  注:外部
//更  新:2012.3.21									
/********************************************************************************/
bit sd_read_info(SD_INFO* info)
{
	unsigned char buf[16],read_bl_len,c_size_mult;
	unsigned short c_size;

	if(sd_read_CID(buf))
		return 1;
	info->id=*(unsigned long *)(buf+9);

	if(sd_read_CSD(buf))
		return 1;

	read_bl_len=buf[5]&0x0f;
	c_size=(buf[6]&0x03)<<10;
	c_size+=buf[7]<<2;
	c_size+=(buf[8]&0xc0)>>6;
	c_size_mult=(buf[9]&0x03)<<1;
	c_size_mult+=(buf[10]&0x80)>>7;

	info->block_len=1<<read_bl_len;
	info->block_count=(unsigned long)(c_size+1)*(1<<(c_size_mult+2));
	return 0;
}




/********************************************************************************/
//函数名:sd_read_sector
//功  能:读SD存储卡一个扇区数据(512字节)
//参  数:sector为要读取的扇区号,buf用于存储扇区的内存指针		
//返回值:正确返回0,错误返回1
//备  注:外部 ,已测得 可连续读数据超过10MB
//更  新:2012.3.21									
/********************************************************************************/
bit sd_read_sector(unsigned long sector,unsigned char* buf)
{
	unsigned char cmd17[]={0x40+17,0,0,0,0,0xff};//cmd17
	unsigned short times;

	sector<<=9;//转换为地址(*512)
	cmd17[1]=(sector&0xff000000)>>24;
	cmd17[2]=(sector&0x00ff0000)>>16;
	cmd17[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//提高兼容性

	SD_SPI_CS = 0;	//打开片选
	if(sd_write_cmd(cmd17) != 0)//命令应答错误
	{
		sd_spi_read(); //读取错误标识
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);//依照时序补充8个时钟
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//超时
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);	//依照时序补充8个时钟
		return 1;
	}

	for(times=0;times<512;times++)//读扇区数据
		buf[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//关闭片选

	sd_spi_write(0xff);	  //依照时序补充8个时钟

	return 0;
}




/********************************************************************************/
//函数名:sd_write_sector
//功  能:向SD存储卡写入一个扇区数据(512字节)
//参  数:sector为要写入的扇区号,buf用于存储扇区的内存指针		
//返回值:正确返回0,错误返回1
//备  注:外部 ,已测得 可连续写超过10MB(若想获得更好 可以加长忙等待)
//更  新:2012.3.21									
/********************************************************************************/
bit sd_write_sector(unsigned long sector,unsigned char* buf)
{
	unsigned char cmd24[]={0x40+24,0,0,0,0,0xff};//cmd24
	unsigned short times;

	sector<<=9;//转换为地址(*512)
	cmd24[1]=(sector&0xff000000)>>24;
	cmd24[2]=(sector&0x00ff0000)>>16;
	cmd24[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//提高兼容性

	SD_SPI_CS = 0;	//打开片选
	if(sd_write_cmd(cmd24) != 0)
	{ //命令应答错误
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);//依照时序补充8个时钟
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0;times--) //等待(Nwr)
		sd_spi_write(0xff);

	sd_spi_write(0xfe);//写数据起始标志
	for(times=0;times<512;times++)			//数据写入
		sd_spi_write(buf[times]);

	sd_spi_write(0xff);//crc
	sd_spi_write(0xff);//crc

	if((sd_spi_read()&0x1f)==0x05)//数据应答正确
	{
		for(times=SD_WRITED_BUSY_DELAY;times>0 && sd_spi_read()!=0xff;times--);//忙等待
		if(!times)//超时
		{
			SD_SPI_CS = 1;	//关闭片选
			sd_spi_write(0xff);//依照时序补充8个时钟
			return 1;
		}
	}else{//数据应答错误
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);//依照时序补充8个时钟
		return 1;
	}

	SD_SPI_CS = 1;	//关闭片选
	sd_spi_write(0xff);//依照时序补充8个时钟
	return 0;
}


/********************************************************************************/
//函数名:sd_read_sector_N
//功  能:读SD存储卡一个扇区中的若干数据
//参  数:sector为要读取的扇区号,buf用于存储扇区的内存指针,start为开始截取位置(0指从扇区头开始),length为截取长度		
//返回值:正确返回0,错误返回1
//备  注:外部 ,已测得 可连续读数据超过10MB
//更  新:2012.3.6									
/********************************************************************************/
bit sd_read_sector_N(unsigned long sector,unsigned char* buf,unsigned short start,unsigned short length)
{
	unsigned char cmd[]={0x40+17,0,0,0,0,0xff};//cmd17
	unsigned short times;

	if(start>512)start=512;//保证位置起始位置不超过512
	if(start+length>512)  //保证截取的数据在一个扇区之内
		length=512-start;

	sector<<=9;//转换为地址(*512)
	cmd[1]=(sector&0xff000000)>>24;
	cmd[2]=(sector&0x00ff0000)>>16;
	cmd[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//提高兼容性

	SD_SPI_CS = 0;	//打开片选
	if(sd_write_cmd(cmd) != 0)
	{ //命令应答错误
		sd_spi_read(); //读取错误标识
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);//依照时序补充8个时钟
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//超时
	{
		SD_SPI_CS = 1;	//关闭片选
		sd_spi_write(0xff);	//依照时序补充8个时钟
		return 1;
	}

	times=start;
	while(times--)
		sd_spi_read();//跳过前面的字节
	for(times=0;times<length;times++)//截取扇区数据
		buf[times]=sd_spi_read();
	times=512-start-length;//计算剩余字节数
	while(times--)
		sd_spi_read();//跳过剩余字节

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//关闭片选

	sd_spi_write(0xff);	  //依照时序补充8个时钟

	return 0;
}




#endif
