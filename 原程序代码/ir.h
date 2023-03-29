

/*
*����ң�ؽ���
*��P3^2��
*ʹ���ж�0�Լ���ʱ��1
*������33.1776MHz MCU
*�ӵ�λ����λ����
*/

/*
*544177215@qq.com
*2011.11.3
*����HC6800V3.2
*/


/*
*����ʹ�ò���
*��ʹ��irInit��ʼ��
*��ʹ��irRead��ȡ
*/

/*����Ϊң�ض�Ӧ��������
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


//�������ݸ��ݾ����޸�
//ǰ�������ڷ�Χ13.5ms(12442)
#define L_MIN_S 12200*6
#define L_MAX_S 12700*6

//�ź�0���ڷ�Χ1.12ms(1032)
#define L_MIN_0 700*6
#define L_MAX_0 1100*6

//�ź�1���ڷ�Χ2.24ms(2064)
#define L_MIN_1 1600*6
#define L_MAX_1 2200*6


//�����������(�û��룬�û��뷴��,������,�����뷴��)
unsigned char IR_ARRAY[4]={0,0,0,0};

unsigned char IR_COUNT;//�ѳɹ����յ�λ��

bit IR_OK;//���ճɹ�λ �ɹ����պ���1 ʹ��ʱӦ��ȡ����Ч���ݺ�����0

bit IR_START_FLAG;//�ѿ�ʼ����λ



void irInit()
{
	IR_OK=0;
	IR_START_FLAG=0;
	IR_COUNT=0;

	//�����ж�
	IT0=1;//���ش���
	EX0=1;
	EA=1;

	//���ö�ʱ��1
	TMOD=TMOD&0x0f;
	TMOD=TMOD|0x10;//��ʽ1
	TH1=0;
	TL1=0;
	TR1=1;//��ʱ����
}

//��������ж�
void irInt0() interrupt 0
{
	unsigned short count=TH1*256+TL1;//ȡ���ڷ�Χ
	TH1=TL1=0;//����

	if(count>=L_MIN_S && count<=L_MAX_S)//�ǿ�ʼ
	{
		IR_START_FLAG=1;
		IR_OK=0;
		IR_COUNT=0;
		return;
	}
	if(IR_START_FLAG)//������
	{
		if(count>=L_MIN_0 && count<=L_MAX_0)//��0
		{
			IR_ARRAY[IR_COUNT/8]=IR_ARRAY[IR_COUNT/8]>>1;
			IR_COUNT++;
		}

		if(count>=L_MIN_1 && count<=L_MAX_1)//��1
		{

			IR_ARRAY[IR_COUNT/8]=(IR_ARRAY[IR_COUNT/8]>>1)|0x80;
			IR_COUNT++;
		}
		if(IR_COUNT==32)//�������
		{
			IR_START_FLAG=0;
			//У����ȷ��
			if(IR_ARRAY[2]==~IR_ARRAY[3] && IR_ARRAY[0]==~IR_ARRAY[1])
				IR_OK=1;
		}
	}
}


//ȡ��һ����Ч�û���Ͳ�����
//�����ݷ���1
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


