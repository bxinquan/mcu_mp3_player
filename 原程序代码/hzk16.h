


/*
*��SD����FAT32ϵͳ�и�Ŀ¼�µ�hzk16�ļ��л�ȡGB2312���ֵ�16*16��ģ��
*ʹ��ʱ���ȳ�ʼ��SD����FAT32ϵͳ
*֧��GB2312
*֧��UNICODE���ֲ���(��ѡ,���ʼ��hzk16_uni_init)
*2012.3.8
*����Ȩ
*/
#ifndef __hzk16_h__
#define __hzk16_h__


#include<reg52.h>
#include"sd.h"
#include"fat.h"


/***************************************************************************/
//������:hzk16_init
//��  ��:��ʼ����������HZK16_START_SECTOR(���ֿ��ļ����ڵ���ʼ����)
//��  ��:
//����ֵ:�ɹ�����0,���󷵻�1
//��  ע:���ֿ��ļ�(hzk16)�Ѵ���SD�ϵ�FAT32ϵͳ�ĸ�Ŀ¼��,��sd����FAT�ѳ�ʼ��
/**************************************************************************/
unsigned long HZK16_START_SECTOR=0;
bit hzk16_init(void)
{
	FAT_FILE hzk16;
	if(fat_open_file("HZK16      ",1,&hzk16))  //���Դ�HZK16�ļ�
		return 1;
	HZK16_START_SECTOR=hzk16.current_sector;
	return 0;
}




/***************************************************************************/
//������:hzk16_gbZiMo
//��  ��:���GB2312����(gb_h,gb_l)��16*16����ģ����zimo������(ģ������32�ֽ���ģ����)
//��  ��:gb_hΪ���ֵ�GB2312����ֽ�,gb_lΪ���ֵ�GB2312����ֽ�
//			zimoΪ�洢��ģ������
//����ֵ:�ɹ�����0,���󷵻�1
//��  ע:
/**************************************************************************/
bit hzk16_gbZiMo(unsigned char gb_h,unsigned char gb_l,unsigned char* zimo)
{
	unsigned long offset;//�洢����ƫ��
	if(gb_h<0xa0)
		return 1;//��Ч����
	if(gb_l<0xa0)
		return 1;//��Чλ��
	gb_h-=0xa0;//�������
	gb_l-=0xa0;//���λ��
	offset=(94ul*(gb_h-1)+(gb_l-1))*32ul;//���ƫ�� (����ul�ǳ���Ҫ ����ʡ��)
	if( sd_read_sector_N(HZK16_START_SECTOR+(offset>>9),zimo,offset%512,32) )
		return 1;//��SD����
	return 0;
}




/***************************************************************************/
//������:hzk16_unigb_init
//��  ��:��ʼ����������HZK16_UNIGB_START_SECTOR(UNICODE->GB2312	��Ӧ���ļ����ڵ���ʼ����)
//��  ��:
//����ֵ:�ɹ�����0,���󷵻�1
//��  ע:��Ӧ���ļ�(unigb)�Ѵ���SD�ϵ�FAT32ϵͳ�ĸ�Ŀ¼��,��sd����FAT�ѳ�ʼ��
/**************************************************************************/
unsigned long HZK16_UNIGB_START_SECTOR=0;
bit hzk16_uni_init(void)
{
	FAT_FILE unigb;
	if(fat_open_file("UNIGB      ",1,&unigb))  //���Դ�HZK16�ļ�
		return 1;
	HZK16_UNIGB_START_SECTOR=unigb.current_sector;
	return 0;
}



/***************************************************************************/
//������:hzk16_uniZiMo
//��  ��:���UNICODE����(uni_h,uni_l)��16*16����ģ����zimo������(ģ������32�ֽ���ģ����)
//��  ��:uni_hΪ���ֵ�UNICODE����ֽ�,uni_lΪ���ֵ�UNICODE����ֽ�
//			zimoΪ�洢��ģ������
//����ֵ:�ɹ�����0,���󷵻�1
//��  ע:
/**************************************************************************/
bit hzk16_uniZiMo(unsigned char uni_h,unsigned char uni_l,unsigned char* zimo)
{
	unsigned long offset;//�洢����ƫ��
	unsigned char gb[2];
	unsigned short unicode=((unsigned short)uni_h<<8)|uni_l;
	if(unicode<0x4e00)
		return 1;//��Ч����

	offset=(unicode-0x4e00)<<1;//���ƫ��
	if( sd_read_sector_N(HZK16_UNIGB_START_SECTOR+(offset>>9),gb,offset%512,2) )
		return 1;//��SD����
	return hzk16_gbZiMo(gb[0],gb[1],zimo);
}




#endif