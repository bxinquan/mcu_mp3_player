

/*
*SD�洢����д(ʹ��SPIģʽ)
*������ʹ����xdata�����ڴ� ������ʹ����xdata������512bytes��MCU
*��д����ʱ������д���� �ɳ������³�ʼ������д �����ֶ�����³�ʼ������
	дʧ�� ����Ҫ����������
	�������׳���,����ʱ�ɳ��Զ������
	11.0592MHz��ƽ��6.9KB/s(10MB��֤) дƽ��6.5KB/s(10MB��֤)
	33.1776MHz��ƽ��160kbps����(1MB��֤)
*��������:2012.3.21
*����Ȩ
*/



#ifndef __sd_h__ 
#define __sd_h__


#include<reg52.h>



//�ӿڶ���(SPIģʽ)
sbit SD_SPI_CS   = P2^0;//Ƭѡ(�͵�ƽ��Ч)
sbit SD_SPI_CLK  = P2^1;//ʱ��
sbit SD_SPI_MISO = P2^2;//����(����/�ӳ�)
sbit SD_SPI_MOSI = P2^3;//���(����/����)



//SPIͨ��ʱ���ֽڻ�����
//ʹ��λѰַ������ٶ�
unsigned char bdata SD_SPI_BUF;	 
sbit SD_SPI_BUF_0 = SD_SPI_BUF^0;
sbit SD_SPI_BUF_1 = SD_SPI_BUF^1;
sbit SD_SPI_BUF_2 = SD_SPI_BUF^2;
sbit SD_SPI_BUF_3 = SD_SPI_BUF^3;
sbit SD_SPI_BUF_4 = SD_SPI_BUF^4;
sbit SD_SPI_BUF_5 = SD_SPI_BUF^5;
sbit SD_SPI_BUF_6 = SD_SPI_BUF^6;
sbit SD_SPI_BUF_7 = SD_SPI_BUF^7; 



//���ڿ��Ƴ�ʱ�Ĳ���
//���������Ӳ�����ܼ�Ƶ�ʽ����޸�
//�����ò��Խ����¸�λ��ʼ��ʧ�ܼ���д����
#define SD_SLOW_SPEED_DELAY 50				//��ʼ������λ�ڼ� ����ģʽ�µ�ʱ��ȴ�����	 ���ʹ������߸�λ����ʼ���ٶ�
#define SD_INIT_CMD_REPEAT_TIMES 200		//��ʼ������λ�ڼ� ������д����Ĵ���	���ʹ������߸�λ����ʼ���ٶ�
#define SD_WRITED_BUSY_DELAY 10000			//д������ �ȴ���ɵĴ��� 	 ����õ���6000
#define SD_CMD_RESPONSE_DELAY 30			//д����� �ȴ���Ӧ�Ĵ���	 ����õ���15
#define SD_WAIT_0XFE_DELAY 100				//��������ȡ��д������� ������ʼ��־0XFE�ĵȴ�����	���ʹ�������д�����ٶ� ����õ���100


/********************************************************************************/
//������:sd_spi_init
//��  ��:SPI���߸�λ
//��  ��:		
//����ֵ:
//��  ע:�ڲ�
//��  ��:2012.3.21									
/********************************************************************************/
void sd_spi_reset(void)
{
	SD_SPI_CS = 1;
	SD_SPI_CLK = 0;
	SD_SPI_MISO = 1;
	SD_SPI_MOSI = 1;
}


/********************************************************************************/
//������:sd_spi_write
//��  ��:��SDд��һ���ֽڵ�����	(SPIģʽ)
//��  ��:��д����ֽ�		
//����ֵ:
//��  ע:�ڲ�
//��  ��:2012.3.21									
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
//������:sd_spi_write_slow
//��  ��:��SDд��һ���ֽڵ�����	(����SPIģʽ)
//��  ��:��д����ֽ�		
//����ֵ:
//��  ע:�ڲ� �ڸ�λ����ʼ���ڼ�ʹ��
//��  ��:2012.3.21									
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
//������:sd_spi_read
//��  ��:��SD��ȡһ���ֽڵ�����	(SPIģʽ)
//��  ��:		
//����ֵ:�������ֽ�
//��  ע:�ڲ�
//��  ��:2012.3.21									
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
//������:sd_spi_read_slow
//��  ��:��SD��ȡһ���ֽڵ�����	(����SPIģʽ)
//��  ��:		
//����ֵ:�������ֽ�
//��  ע:�ڲ� �ڸ�λ����ʼ���ڼ�ʹ��
//��  ��:2012.3.21									
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
//������:sd_write_cmd
//��  ��:д��һ�������ֲ����ػ�ӦR1
//��  ��:��������		
//����ֵ:��ӦR1,������0xff
//��  ע:�ڲ�
//��  ��:2012.3.21									
/********************************************************************************/
unsigned char sd_write_cmd(unsigned char* cmd)
{
	unsigned char r1;
	unsigned short times;

	for(times=0;times<6;times++)//д������
		sd_spi_write(cmd[times]);
	for(times=SD_CMD_RESPONSE_DELAY ; times>0 ; times--)//�ȴ���Ӧ
		if((r1=sd_spi_read())!=0xff)
			break;

	if(times == 0)	//��ʱ
		return 0xff;
	else
		return r1;	
}


/********************************************************************************/
//������:sd_write_cmd_slow
//��  ��:����д��һ�������ֲ����ػ�ӦR1
//��  ��:��������		
//����ֵ:��ӦR1,������0xff
//��  ע:�ڲ� ��λ����ʼ�ڼ�ʹ��
//��  ��:2012.3.21									
/********************************************************************************/
unsigned char sd_write_cmd_slow(unsigned char* cmd)
{
	unsigned char r1;
	unsigned short times;

	for(times=0;times<6;times++)//д������
		sd_spi_write_slow(cmd[times]);
	for(times=SD_CMD_RESPONSE_DELAY ; times>0 ; times--)//�ȴ���Ӧ
		if((r1=sd_spi_read_slow())!=0xff)
			break;

	if(times == 0)	//��ʱ
		return 0xff;
	else
		return r1;	
}



/********************************************************************************/
//������:sd_reset
//��  ��:SD�洢����λ
//��  ��:		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_reset(void)
{
	unsigned char code cmd0[]={0x40,0,0,0,0,0x95};//��λ��ת����SPIģʽ����cmd0 
	unsigned short times;

	sd_spi_reset();	  //�ӿڸ�λ

	for(times=15 ; times>0 ; times--)  //����ʱ��ͼ ��λ����֮ǰ��������74��ʱ�� ����SPIģʽ
		sd_spi_write_slow(0xff);	

	SD_SPI_CS = 0;	//��Ƭѡ	
	for(times=SD_INIT_CMD_REPEAT_TIMES ; times>0 ; times--)//��д�ȴ�ֱ����ȷ��Ӧ��ʱ
		if(sd_write_cmd_slow(cmd0)==0x01)
			break;
	SD_SPI_CS = 1;//�ر�Ƭѡ

	sd_spi_write_slow(0xff);//����ʱ�򲹳�8��ʱ��

	if(times == 0) //��ʱ
		return 1;
	//��λ���
	return 0;
}



/********************************************************************************/
//������:sd_init
//��  ��:SD�洢����λ����ʼ��
//��  ��:		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_init(void)
{
	unsigned char code cmd1[]={0x41,0,0,0,0,0xff};//��ʼ��cmd1
	unsigned short times;

	if(sd_reset())//��λSD��
		return 1;

	sd_spi_write_slow(0xff);//��߼�����

	SD_SPI_CS = 0; 	//��Ƭѡ
	for(times=SD_INIT_CMD_REPEAT_TIMES ; times>0 ; times--)//��д�ȴ�ֱ����ȷ��Ӧ��ʱ
		if(sd_write_cmd_slow(cmd1)==0x00)
			break;
	SD_SPI_CS = 1;  //�ر�Ƭѡ

	sd_spi_write_slow(0xff);//����ʱ�򲹳�8��ʱ��

	if(times == 0) //��ʱ
		return 1;
	//��ʼ�����
	return 0;
}



/********************************************************************************/
//������:sd_read_CID
//��  ��:��SD�洢��CID�Ĵ���
//��  ��:���ڱ���CID��Ϣ��16�ֽ�����		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ڲ�
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_read_CID(unsigned char* cid)
{
	unsigned char code cmd10[]={0x40+10,0,0,0,0,0xff};//CMD10
	unsigned short times;

	sd_spi_write(0xff);//��߼�����

	SD_SPI_CS = 0;	//��Ƭѡ
	if(sd_write_cmd(cmd10) != 0)//����Ӧ�����
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//��ʱ
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);	//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=0;times<16;times++)//������
		cid[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//�ر�Ƭѡ
	sd_spi_write(0xff);	  //����ʱ�򲹳�8��ʱ��
	return 0;
}



/********************************************************************************/
//������:sd_read_CSD
//��  ��:��SD�洢��CSD�Ĵ���
//��  ��:���ڱ���CSD��Ϣ��16�ֽ�����		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ڲ�
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_read_CSD(unsigned char* csd)
{
	unsigned char code cmd9[]={0x40+9,0,0,0,0,0xff};//CMD9
	unsigned short times;

	sd_spi_write(0xff);//��߼�����

	SD_SPI_CS = 0;	//��Ƭѡ
	if(sd_write_cmd(cmd9))
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//��ʱ
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);	//����ʱ�򲹳�8��ʱ��
		return 1;
	}
	
	for(times=0;times<16;times++)//������
		csd[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//�ر�Ƭѡ
	sd_spi_write(0xff);	  //����ʱ�򲹳�8��ʱ��
	return 0;	
}


//SD�洢����Ϣ�ṹ
typedef struct
{
	unsigned long id;//��Ʒ���к�
	unsigned short block_len;//���С ��λByte
	unsigned long block_count;//�ܿ���	
}SD_INFO;


/********************************************************************************/
//������:sd_read_info
//��  ��:��SD�洢����Ϣ����
//��  ��:���ڱ���洢����Ϣ���ݵ�ָ��		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ
//��  ��:2012.3.21									
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
//������:sd_read_sector
//��  ��:��SD�洢��һ����������(512�ֽ�)
//��  ��:sectorΪҪ��ȡ��������,buf���ڴ洢�������ڴ�ָ��		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ ,�Ѳ�� �����������ݳ���10MB
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_read_sector(unsigned long sector,unsigned char* buf)
{
	unsigned char cmd17[]={0x40+17,0,0,0,0,0xff};//cmd17
	unsigned short times;

	sector<<=9;//ת��Ϊ��ַ(*512)
	cmd17[1]=(sector&0xff000000)>>24;
	cmd17[2]=(sector&0x00ff0000)>>16;
	cmd17[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//��߼�����

	SD_SPI_CS = 0;	//��Ƭѡ
	if(sd_write_cmd(cmd17) != 0)//����Ӧ�����
	{
		sd_spi_read(); //��ȡ�����ʶ
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//��ʱ
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);	//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=0;times<512;times++)//����������
		buf[times]=sd_spi_read();

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//�ر�Ƭѡ

	sd_spi_write(0xff);	  //����ʱ�򲹳�8��ʱ��

	return 0;
}




/********************************************************************************/
//������:sd_write_sector
//��  ��:��SD�洢��д��һ����������(512�ֽ�)
//��  ��:sectorΪҪд���������,buf���ڴ洢�������ڴ�ָ��		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ ,�Ѳ�� ������д����10MB(�����ø��� ���Լӳ�æ�ȴ�)
//��  ��:2012.3.21									
/********************************************************************************/
bit sd_write_sector(unsigned long sector,unsigned char* buf)
{
	unsigned char cmd24[]={0x40+24,0,0,0,0,0xff};//cmd24
	unsigned short times;

	sector<<=9;//ת��Ϊ��ַ(*512)
	cmd24[1]=(sector&0xff000000)>>24;
	cmd24[2]=(sector&0x00ff0000)>>16;
	cmd24[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//��߼�����

	SD_SPI_CS = 0;	//��Ƭѡ
	if(sd_write_cmd(cmd24) != 0)
	{ //����Ӧ�����
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0;times--) //�ȴ�(Nwr)
		sd_spi_write(0xff);

	sd_spi_write(0xfe);//д������ʼ��־
	for(times=0;times<512;times++)			//����д��
		sd_spi_write(buf[times]);

	sd_spi_write(0xff);//crc
	sd_spi_write(0xff);//crc

	if((sd_spi_read()&0x1f)==0x05)//����Ӧ����ȷ
	{
		for(times=SD_WRITED_BUSY_DELAY;times>0 && sd_spi_read()!=0xff;times--);//æ�ȴ�
		if(!times)//��ʱ
		{
			SD_SPI_CS = 1;	//�ر�Ƭѡ
			sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
			return 1;
		}
	}else{//����Ӧ�����
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	SD_SPI_CS = 1;	//�ر�Ƭѡ
	sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
	return 0;
}


/********************************************************************************/
//������:sd_read_sector_N
//��  ��:��SD�洢��һ�������е���������
//��  ��:sectorΪҪ��ȡ��������,buf���ڴ洢�������ڴ�ָ��,startΪ��ʼ��ȡλ��(0ָ������ͷ��ʼ),lengthΪ��ȡ����		
//����ֵ:��ȷ����0,���󷵻�1
//��  ע:�ⲿ ,�Ѳ�� �����������ݳ���10MB
//��  ��:2012.3.6									
/********************************************************************************/
bit sd_read_sector_N(unsigned long sector,unsigned char* buf,unsigned short start,unsigned short length)
{
	unsigned char cmd[]={0x40+17,0,0,0,0,0xff};//cmd17
	unsigned short times;

	if(start>512)start=512;//��֤λ����ʼλ�ò�����512
	if(start+length>512)  //��֤��ȡ��������һ������֮��
		length=512-start;

	sector<<=9;//ת��Ϊ��ַ(*512)
	cmd[1]=(sector&0xff000000)>>24;
	cmd[2]=(sector&0x00ff0000)>>16;
	cmd[3]=(sector&0x0000ff00)>>8;

	sd_spi_write(0xff);//��߼�����

	SD_SPI_CS = 0;	//��Ƭѡ
	if(sd_write_cmd(cmd) != 0)
	{ //����Ӧ�����
		sd_spi_read(); //��ȡ�����ʶ
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	for(times=SD_WAIT_0XFE_DELAY;times>0 && sd_spi_read()!=0xfe;times--);
	if(!times)//��ʱ
	{
		SD_SPI_CS = 1;	//�ر�Ƭѡ
		sd_spi_write(0xff);	//����ʱ�򲹳�8��ʱ��
		return 1;
	}

	times=start;
	while(times--)
		sd_spi_read();//����ǰ����ֽ�
	for(times=0;times<length;times++)//��ȡ��������
		buf[times]=sd_spi_read();
	times=512-start-length;//����ʣ���ֽ���
	while(times--)
		sd_spi_read();//����ʣ���ֽ�

	sd_spi_read();//crc
	sd_spi_read();//crc

	SD_SPI_CS = 1;	//�ر�Ƭѡ

	sd_spi_write(0xff);	  //����ʱ�򲹳�8��ʱ��

	return 0;
}




#endif
