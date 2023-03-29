#include<reg52.h>
#include"vs1003.h"
#include"vs1003_spectrum.h"
#include"sd.h"
#include"fat.h"
#include"tftlcd.h"
#include"tftchar.h"
#include"hzk16.h"
#include"chinese.h"
#include"key.h"
#include"zimo.c"
#include"ir.h"


//��������(���ļ������еĺ�����ͬά��)
unsigned char play_stat;//����״̬ 1-���ŵ��ļ�δ���� 2-�����ļ������� 3-��ͣ���ļ�δ���� 4-��ͣ���ļ�������
unsigned char play_style;//���ŷ�ʽ(����ѭ���ȵ�){1-��������,2-����ѭ��,3-˳�򲥷�,4-˳��ѭ������}
bit surround_sound;//��������� 1-�� 0-�� ��ͬ
bit low_sound;//�������ر��
bit high_sound;//������ǿ���
unsigned char vol;//����[0,20]





//��vol����ת��ΪVS1003����
unsigned char code vol_tab[21]=
{0,80,110,140,160,180,190,200,205,210,215,220,225,230,235,238,241,245,248,251,254};

//���ɺ���ļ��̡�
bit key(unsigned char *x,unsigned char *y)
{
	if(key1(x,y))  //�����ϼ���
		return 1;
	if(irRead(x,y))	  //���������
	{	delay1ms(100);//���������LCD��Ӱ��
		switch(*y)
		{
			case 0x46:
			case 0x0c://��Ӧ0
				*x=0;*y=0;
				break;
			case 0x09:
			case 0x18://��Ӧ1
				*x=0;*y=1;
				break; 
			case 0x47:
			case 0x5e://��Ӧ2
				*x=0;*y=2;
				break;
			case 0x40:
			case 0x08://��Ӧ4
				*x=1;*y=0;
				break;
			case 0x44:
			case 0x1c://��Ӧ5
				*x=1;*y=1;
				break;
			case 0x43:
			case 0x5a://��Ӧ6
				*x=1,*y=2;
				break;
			case 0x15:
			case 0x52://��Ӧ9
				*x=2;*y=1;
				break;
		}
		return 1;
	}
	return 0;

}


//������
void Error(unsigned char i)
{
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,0,31);
	tftSetCharWindow((128-10*8)/2,32,10*8,16);
	tftShowStr("Error");
	tftShowULong(i);
	while(1);
}



/********************************************************************************/
//������:get_long_file_name_from_fct
//��  ��:���ļ���������Ϸ����ִ�
//��  ��:fctΪ�ļ���,nameΪ����ļ�������(27B)
//����ֵ:
//��  ע:�ⲿ
//��  ��:2012.3.2									
/********************************************************************************/
void get_long_file_name_from_fct(FAT_DIR_ENTRY*fct,unsigned char *name)
{
	unsigned char i,*arr;
	arr=(unsigned char*)fct;
	for(i=0;i<10;i++)
		name[i]=arr[1+i];
	for(i=0;i<12;i++)
		name[10+i]=arr[14+i];
	for(i=0;i<4;i++)
		name[22+i]=arr[28+i];
}


//�������������ʾ
void frame_show(void)
{
	tftClear(0,0,128,160,tftRGB(0,31,31));

	//Music Player

	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	tftSetCharWindow((128-12*8)/2,3,12*8,16);
	tftShowStr("Music Player");

	//������
	tftDrawRect(6,17,6+114+1,19+64+1,1,tftRGB(10,21,10));
	//�����ʿ�
	tftDrawRect(8,87,8+5*8,87+16,1,tftRGB(10,21,10));
	//�����ʿ�
	tftDrawRect(64,87,64+7*8,87+16,1,tftRGB(10,21,10));
	//�ļ����Ϳ�
	tftDrawRect(96,106,96+8*3,106+16,1,tftRGB(10,21,10));
	//������
	tftDrawRect(24,134,24+40+2+2,134+3+1,1,tftRGB(10,21,10));
	//���ȿ�
	tftDrawRect(23,149,23+60+1,149+4+1,1,tftRGB(10,21,10));

}

//״̬��ʾ
void set_play_stat(unsigned char stat)
{
	if(stat<0 || stat>4)
		return;
	play_stat=stat;//�޸ı���

	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(7,144,16,16);
	switch(play_stat)
	{
		case 1:
		case 2:
			tftShowChar(16,16,play_ico);
			break;
		case 3:
		case 4:
			tftShowChar(16,16,break_ico);
			break;					
	}
}


//�������ò���ʾ
void set_vol(unsigned char v)
{
	if(v>20)
		return;
	vol=v;										//�޸�����
	vs1003_set_volume(vol_tab[vol],vol_tab[vol]);	  //��������


	//��ʾ������
	tftClear(24+2+v*2,134+2,40-v*2,1,tftRGB(10,21,10));	//�����ǰ������Ӧ�����Ժ������
	tftClear(24+2,134+2,v*2,1,tftRGB(30,0,0));//һ���������������ر�ʾ������v*2

	//��ʾ����ͼ��
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(8,128,16,16);
	if(v)
		tftShowChar(16,16,vol_yes_ico);
	else
		tftShowChar(16,16,vol_no_ico);
}

//������ʾ
//nameΪҪ��ʾ�������ļ���(uni=1UNICODE�� uni=0ASCII)
void name_show(unsigned char *name,bit uni)
{
	tftClear(7,18,112+2,64+2,tftRGB(20,40,20));
	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	tftSetCharWindow(8,19,112,64);
	if(uni)
		tftShowUNI(name);
	else
		tftShowGB(name);
}

//��������ʾ
void samprate_show(unsigned short rate)
{
	rate/=1000;

	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(20,40,20);
	tftSetCharWindow(9,88,40,16);
	if(rate<10)
		tftShowStr(" ");
	tftShowULong(rate);
	tftShowStr("KHz");
}
//��������ʾ
void bitrate_show(unsigned short rate)
{

	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(20,40,20);
	tftSetCharWindow(65,88,56,16);
	if(rate<10)
		tftShowStr("  ");
	else if(rate<100)
		tftShowStr(" ");
	tftShowULong(rate);
	tftShowStr("Kbps");
}


//���ŷ�ʽ���ò���ʾ
void set_play_style(unsigned char n)
{
	if(n<1 || n>4)
		return;
	play_style=n;	//�޸Ĳ��ŷ�ʽ

//��ʾ���ŷ�ʽ
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(8,108,16*3,16);
	switch(play_style)
	{
		case 1:
			tftShowChar(16*3,16,style1_ico);
			break;
		case 2:
			tftShowChar(16*3,16,style2_ico);
			break;
		case 3:
			tftShowChar(16*3,16,style3_ico);
			break;
		case 4:
			tftShowChar(16*3,16,style4_ico);
			break;
	}
}
//�ļ�������ʾ
void filetype_show(unsigned char *type)
{
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(20,40,20);
	tftSetCharWindow(97,107,24,16);
	tftShowStr(type);	
}

//�ļ���ʱ����ʾ
void time_show(unsigned short time)
{
	tftClear(88,144,40,16,tftRGB(0,31,31));

	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	tftSetCharWindow(88,144,40,16);
	tftShowULong(time/60);
	tftShowStr(":");
	time%=60;
	if(time<10)
		tftShowStr("0");
	tftShowULong(time);	
}


//���������ò�ͼ����ʾ  (1-���ԡ�0-����)
void set_surround_sound(bit x)
{

	surround_sound=x;//�޸ı���

	vs1003_set_surround(surround_sound);  //����

	//��ʾ
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(72,128,16,16);
	if(surround_sound)
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,10);
	else
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,26,28);
	tftShowChar(16,16,surround_sound_ico);		

}

//�ߵ��������ò��������������Ч����
//�������ò�ͼ����ʾ (1-���� 0-����)
void set_low_sound(bit x)
{
	low_sound=x;//�޸ı���
	if(low_sound)	//����
	{
		vs1003_set_bass(15,15);	 //��150Hz����ǿ�ֱ� ��Ƶ��150Hz�ϼӷֱ��Ƚ�����
		if(high_sound==0)		   //���������� ���͸���
			vs1003_set_treble(-8,10);				//��10KHz�Ͻ��ͷֱ�
	}	
	else
	{
		vs1003_set_bass(0,0);
		if(high_sound==0)		//���������� �رո���
			vs1003_set_treble(0,0);		
	}


//��ʾ
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(88,128,16,16);
	if(low_sound)
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,10);
	else
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,26,28);
	tftShowChar(16,16,low_sound_ico);

}

//�������ò�ͼ����ʾ (1-���� 0-����)
void set_high_sound(bit x)
{
	high_sound=x;//�޸ı���
	if(high_sound)			//����
	{
		vs1003_set_treble(7,10);   //��Ƶ��10KHz�ϼӷֱ��Ƚ�����
	}		
	else
	{
		if(low_sound)			 //�������� ��������
			vs1003_set_treble(-8,10);
		else
			vs1003_set_treble(0,0);
	}

//��ʾ
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(104,128,16,16);
	if(high_sound)
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,10);
	else
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,26,28);
	tftShowChar(16,16,high_sound_ico);
}



/****************************************************************/
//������:step_show
//��  ��:�������ƽ�
//��  ��:lengthΪ�������ĳ��� resetΪ1��ʾ�ӿ�ʼλ��������ʾ���������� resetΪ0��ʾ����ʾ���ӵĲ���
/****************************************************************/
void step_show(unsigned char length,bit reset)
{
	static unsigned char fre_len;//����������ʾ�ĳ��� (���Ϊ0���ӿ�ʼ�㻭������ ,���ÿ��ʾһ�ξͽ���������ǰ���ȸ����������´β��ٴ�ͷ��ʾ������ֱ��reset��λ)
	unsigned short color=tftRGB(0x5f>>3,0x3c>>4,0x23>>3);//��������ɫ
	if(length>60)
		return;
	if(reset||length<fre_len)  //�ӿ�ʼ��������ʾ
	{
		fre_len=0;
		tftClear(24,150,60,4,tftRGB(0,31,31));//��λ�����������
	}															 
	if(length==fre_len)//���ϴεĽ�����������ͬ
		return;

	tftClear(24+fre_len,150,length-fre_len,4,color);
	fre_len=length;//ˢ������ʾ�ĳ���
}



//��ʾƵ��
void spec_show(bit reset)
{

	unsigned short color1=tftRGB(31,0,0);	  //ǰ��ɫ
	unsigned short color2=tftRGB(0,31,31); 		//����ɫ
	static unsigned short spec1[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned short spec2[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//ptr1���ڴ洢��Ҫ��ȡ��Ƶ�� ptr2���ڴ洢�ϴ�����ʾ��Ƶ��ֵ(ͨ���Ƚ����ε�ֵֻ��ʾ����ֵ���������ֵ �����˷�ʱ��)	
	static unsigned short *ptr1=spec1,*ptr2=spec2;	
	unsigned short *tmp;
	unsigned char i,val1,val2,x;

	//��ʾ������Ƶ��			
	if(reset)
	{
		for(i=0;i<14;i++)
			ptr2[i]=0;
	}

	x=60;//��״ͼX����
	vs1003_get_spec(ptr1);
	for(i=0;i<14;i++) //��ʾ��״ͼ
	{
		val1=ptr1[i]&0x000f;	//ȡ��4λЧ������
		val2=ptr2[i]&0x000f;   	//ȡ��4λЧ������
		if(val1>val2)			//��ֵ�Ⱦ�ֵ��
		{
			tftClear(x,108+16-val1,1,val1-val2,color1);//����ʾ����
		}else if(val1<val2)	  //��ֵ�Ⱦ�ֵ��
		{
			tftClear(x,108+16-val2,1,val2-val1,color2);//���������
		}
		//����ȱ��β�����ʾ
		x+=2;	
	}
	tmp=ptr1;
	ptr1=ptr2;
	ptr2=tmp;
}




/**********************************************************************/
//������:setup
//��  ��:��ʾ���ô��ڹ��û����á�ȷ�������������á�
/**********************************************************************/
void setup(void)
{
	bit surround,low,high;//��Ч���ñ���
	unsigned char style;   //���ŷ�ʽ���ñ���
	unsigned char key_stat;//��ǰѡ��״̬0~6��7��ѡ��
	unsigned char code option_pos[7]={2*16,3*16,4*16,6*16,7*16,8*16,9*16};//��ѡ���Y����
	unsigned short color1,color2;//����ǰ��ɫ������ɫ
	unsigned char x,y;

	tftClear(0,0,128,160,tftRGB(0,31,31)); //ˢ��

	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
//����
	tftSetCharWindow((128-2*16)/2,0,2*16,16);
	tftShowGB("����");
//��Ч
	tftSetCharWindow(0,1*16,2*16,16);
	tftShowGB("��Ч");
//������
	tftSetCharWindow(32,2*16,3*16,16);
	tftShowGB("������");
//��������
	tftSetCharWindow(32,3*16,4*16,16);
	tftShowGB("��������");
//������ǿ
	tftSetCharWindow(32,4*16,4*16,16);
	tftShowGB("������ǿ");
//���ŷ�ʽ
	tftSetCharWindow(0,5*16,4*16,16);
	tftShowGB("���ŷ�ʽ");
//��������
	tftSetCharWindow(32,6*16,4*16,16);
	tftShowGB("��������");
//����ѭ��
	tftSetCharWindow(32,7*16,4*16,16);
	tftShowGB("����ѭ��");
//˳�򲥷�
	tftSetCharWindow(32,8*16,4*16,16);
	tftShowGB("˳�򲥷�");
//˳��ѭ��
	tftSetCharWindow(32,9*16,4*16,16);
	tftShowGB("˳��ѭ��");

	
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
//��������ѡ��
	tftSetCharWindow(16,2*16,16,16);
	if(surround_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//�������ظ�ѡ��
	tftSetCharWindow(16,3*16,16,16);
	if(low_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//������ǿ��ѡ��
	tftSetCharWindow(16,4*16,16,16);
	if(high_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//����������ѡ��
	tftSetCharWindow(16,6*16,16,16);
	if(play_style==1)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//����ѭ����ѡ��
	tftSetCharWindow(16,7*16,16,16);
	if(play_style==2)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//˳�򲥷���ѡ��
	tftSetCharWindow(16,8*16,16,16);
	if(play_style==3)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//˳��ѭ����ѡ��
	tftSetCharWindow(16,9*16,16,16);
	if(play_style==4)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);


	surround=surround_sound;
	low=low_sound;
	high=high_sound;
	style=play_style;
	key_stat=0;
	color1=tftRGB(31,0,0);
	color2=tftRGB(0,31,31);
	tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);
	while(1)
	{
		if(key(&x,&y))//�а���
		{
			delay1ms(50);//�������̶�LCD��Ӱ��
			switch(x*4+y)
			{
				case 1://��
					if(key_stat==0)
						break;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color2);//��ɫ
					key_stat--;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//��ɫ
					break;
				case 9://��
					if(key_stat==6)
						break;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color2);//��ɫ
					key_stat++;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//��ɫ
					break;
				case 5://ѡ��
					switch(key_stat)
					{
						case 0://������
							surround=~surround;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(surround)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 1://����
							low=~low;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(low)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 2://����
							high=~high;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(high)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 3://��������
						case 4://����ѭ��
						case 5://˳�򲥷�
						case 6://˳��ѭ��
							tftSetCharWindow(16,option_pos[style+2],16,16);
							tftShowChar(16,16,option_no_ico);
							style=key_stat-2;
							tftSetCharWindow(16,option_pos[key_stat],16,16);
							tftShowChar(16,16,option_yes_ico);
							break;
					}
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//��ɫ
					break;
				case 0://ȷ��
					surround_sound=surround;
					low_sound=low;
					high_sound=high;
					play_style=style;
					return;					
				case 2://ȡ��
					return;
			}
		}				
	}

}



/**********************************************************************/
//������:record
//��  ��:��ʾ¼�������� ���û����� ����ȷ����ʼ¼�� ¼�����ݱ�����SD����
/**********************************************************************/
void recorder(void)
{
	unsigned char option_stat;//��ǰ������ 0-�ļ������ 1-ʱ 2-�� 3-��
	unsigned char x,y;
	bit isLock;//����ѡ�� ֻ�����������ܸ���ֵ
	unsigned short lockColor=tftRGB(31,0,0);//ѡ�����ʱ��ǰ��ɫ
	unsigned short unlockColor=tftRGB(5,5,5);//ѡ��δ������ʱ��ǰ��ɫ
	unsigned char code option_pos[][2]=
	{
		{76,16*2},//�ļ����ѡ��λ��
		{16,16*4},//ʱѡ��λ��
		{48,16*4},//��ѡ��λ��
		{80,16*4}	//��λ��
	};
	unsigned char code option_max[4]={99,99,59,59};  //��ѡ������������ֵ
	unsigned char option_val[4]={0,0,1,0};//��ѡ��ĵ�ǰֵ

	FAT_FILE file;
	unsigned char file_name[11]={'R','E','C','O','R','D',' ',' ','M','P','3'};
	unsigned long file_size;
	RiffHeader *head=FAT_BUF;

	tftClear(0,0,128,160,tftRGB(0,31,31)); //ˢ��

	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);

	//¼����
	tftSetCharWindow((128-3*16)/2,0,3*16,16);
	tftShowGB("¼����");
	//�����ļ���Ϊ:
	tftSetCharWindow(0,16,7*16,16);
	tftShowGB("�����ļ���Ϊ:");
	//RECORD
	tftSetCharWindow((128-9*8)/2,16*2,6*8,16);
	tftShowGB("RECORD");
	//���¼��ʱ��Ϊ:
	tftSetCharWindow(0,16*3,8*16,16);
	tftShowGB("���¼��ʱ��Ϊ:");
	//  ʱ  ��  ��
	tftSetCharWindow((128-6*16)/2,16*4,6*16,16);
	tftShowGB("  ʱ  ��  ��");

	option_stat=0; //��ǰѡ��Ϊ�ļ����
	isLock=0;	   //δ����״̬

	tftSetCharWindow(option_pos[0][0],option_pos[0][1],16,16);
	tftShowULong(option_val[0]);   //��ʾ�ļ����
	tftSetCharWindow(option_pos[1][0],option_pos[1][1],16,16);
	tftShowULong(option_val[1]);   //��ʾʱ
	tftSetCharWindow(option_pos[2][0],option_pos[2][1],16,16);
	tftShowULong(option_val[2]);   //��ʾ��
	tftSetCharWindow(option_pos[3][0],option_pos[3][1],16,16);
	tftShowULong(option_val[3]);   //��ʾ��
	tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));
	while(1)
	{
		if(key(&x,&y))
		{
			delay1ms(50);//�������̶�LCD��Ӱ��
			switch(x*4+y)
			{
				case 0://�û�ȷ���� ��ʼ׼���ռ� ¼��
//					tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(0,31,31));//����
					TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
					tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
					tftShowULong(option_val[option_stat]);	//���Ե�ǰֵ
					if(option_val[option_stat]<10)
					tftShowStr(" ");//�ڶ�λ����

					if(option_val[0]<10)
					{
						file_name[6]='0'+option_val[0];
						file_name[7]=' ';
					}
					else
					{
						file_name[6]='0'+option_val[0]/10;
						file_name[7]='0'+option_val[0]%10;
					}

					file_size=((unsigned long)option_val[1]*60+option_val[2])*60+option_val[3];//��������
					if(file_size==0)
					{//����Ϊ0
						TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
						tftSetCharWindow(0,5*16,128,32);
						tftShowGB("¼��ʱ��̫��!");
						delay10ms(200);
						tftClear(0,5*16,128,32,tftRGB(0,31,31));
						option_stat=3;//��ǰѡ����Ϊ��
						isLock=1;//��������
						TFT_CHAR_WINDOW_FORECOLOR=lockColor;
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//���Ե�ǰֵ
						if(option_val[option_stat]<10)
						tftShowStr(" ");//��ֵʱ ���ܴ���λ��Ϊһλ ���Եڶ�λ����					
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��																								
						break;
					}
					file_size*=8110ul;	// 8110B/s
					file_size+=512;//�����ļ���С


					if(fat_open_file(file_name,1,&file)==0)
					{	//���ļ��Ѵ��� ��ʾ�û�
						TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
						tftSetCharWindow(0,5*16,128,32);
						tftShowGB("�ļ��Ѵ���,�Ƿ񸲸�?");
						option_stat=1;	 //���ñ���
						while(option_stat)
						{
							if(key(&x,&y))
							{
								delay1ms(50);//�������̶�LCD��Ӱ��	
								switch(x*4+y)
								{
									case 0://����ԭ�ļ�
										isLock=1;//���ñ���
										option_stat=0;
										break;
									case 2://������ԭ�ļ�
										isLock=0;//���ñ���
										option_stat=0;
										break;
								}
							}
						}
						tftClear(0,5*16,128,32,tftRGB(0,31,31));
						if(isLock==0)
						{//�û�Ҫ�󲻸���
							option_stat=0;//��ǰѡ����Ϊ�ļ����
							isLock=1;//��������
							TFT_CHAR_WINDOW_FORECOLOR=lockColor;
							tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
							tftShowULong(option_val[option_stat]);	//���Ե�ǰֵ
							if(option_val[option_stat]<10)
								tftShowStr(" ");//�ڶ�λ����					
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��																								
							break;						
						}
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
						tftSetCharWindow(0,5*16,128,16);
						tftShowGB("׼���ռ���...");
						if(fat_file_resize(file_size,&file))
							Error(30);
						tftShowGB("��");	
					}else
					{
						//�����ļ�
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
						tftSetCharWindow(0,5*16,128,16);
						tftShowGB("׼���ռ���...");
						if(fat_open_file(file_name,0,&file))
							Error(31);
						if(fat_file_resize(file_size,&file))
							Error(32);
						tftShowGB("��");
					}
					//����׼�������Ѿ���
					tftSetCharWindow(0,6*16,128,16);
					tftShowGB("���������ʼ");
					if(fat_file_write(&file))//д��ͷģ��
						Error(33);
					tftSetCharWindow(0,7*16,128,16);
					while(key(&x,&y)==0);  //�ȴ��û�����
					delay1ms(50);//�������̶�LCD��Ӱ��
					vs1003_start_adpcm(0,1);//����˷� ������ͨ�˲�	����¼��ģʽ
					tftShowGB("¼����...");
					tftSetCharWindow(0,8*16,128,16);
					tftShowGB("�������ֹͣ¼��");
					file_size-=512;
					while(1)
					{
						vs1003_get_adpcm(FAT_BUF);	 //��ȡ¼������
						vs1003_get_adpcm(FAT_BUF+256);
						if(fat_file_write(&file))	//д���ļ���
							Error(34);
						if(file_size<512)	 //�ͺ�һ���������������ݽ�����
						{
							file_size=0;
							break;
						}
						file_size-=512;
						if(key(&x,&y))//�û������� �뿪
						{
							delay1ms(50);//�������̶�LCD��Ӱ��
							break;
						}
					}
					vs1003_stop_adpcm();//�ر�¼��ģʽ
					if(fat_file_resize(file.file_size - file_size,&file))  //�����ļ���С
						Error(35);
					if(fat_file_set_offset(0,&file))
						Error(36);
					for(lockColor=0;lockColor<512;lockColor++)	   //�����ļ�ͷģ��
						FAT_BUF[lockColor]=RIFF_header[lockColor];
					head->ChunkSize=file.file_size-8;//����ļ�����
					fat_l2be((U8*)&(head->ChunkSize),4);
					head->SubChunk3Size = file.file_size-512;//������ݳ���
					fat_l2be((U8*)&(head->SubChunk3Size),4);
					head->NumOfSamples=(file.file_size-512)/256 * 505ul;	//��������
					fat_l2be((U8*)&(head->NumOfSamples),4);
					if(fat_file_write(&file)) //д��������
						Error(37);				
					if(fat_close_file(&file))  //�ر��ļ�
						Error(38);
					tftSetCharWindow((128-4*16)/2,9*16,4*16,16);
					tftShowGB("¼�����");
					delay10ms(200);
					return ;
				case 1://�û������ϼ�
					if(isLock)
					{//��������ֵ
						option_val[option_stat] = (option_val[option_stat]+1) % (option_max[option_stat]+1); //��ֵ
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//���Ե�ǰֵ
						if(option_val[option_stat]<10)
						tftShowStr(" ");//��ֵʱ ���ܴ���λ��Ϊһλ ���Եڶ�λ����					
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��											
					}else
					{//��������һ��ѡ��
						if(option_stat>0)
						{
							option_stat--;
							tftDrawRect(option_pos[option_stat+1][0],option_pos[option_stat+1][1],option_pos[option_stat+1][0]+15,option_pos[option_stat+1][1]+15,1,tftRGB(0,31,31));//����
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��
						}					
					}
					break;
				case 2://�û�ȡ���˱���¼������
					return ;
				case 5://�û���������/��������
					if(isLock)
					{//������
						isLock=0;//δ�������
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
					}else
					{//������ǰ��
						isLock=1;//�������
						TFT_CHAR_WINDOW_FORECOLOR=lockColor;
					}
					tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
					tftShowULong(option_val[option_stat]);						
					tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));
					break;
				case 9://�û������¼�
					if(isLock)
					{//�����ｵֵ
						if(option_val[option_stat]==0)
							option_val[option_stat]=option_max[option_stat];
						else
							option_val[option_stat]--; //��ֵ
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//���Ե�ǰֵ
						if(option_val[option_stat]<10)
						tftShowStr(" ");//��ֵʱ ���ܴ���λ��Ϊһλ ���Եڶ�λ����				
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��										
					}else
					{//��������һ��ѡ��
						if(option_stat<3)
						{
							option_stat++;
							tftDrawRect(option_pos[option_stat-1][0],option_pos[option_stat-1][1],option_pos[option_stat-1][0]+15,option_pos[option_stat-1][1]+15,1,tftRGB(0,31,31));//����
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//��ʾ��
						}					
					}
					break;
			}	
		}		
	}
}



//���ֲ�����������
void main()
{
	unsigned char *buf=FAT_BUF;//���û�����
	unsigned short file_total;//��Ŀ¼�������ļ�����
	unsigned short file_id;//��ǰ�ļ���(��1��ʼ,���file_total)
	unsigned char file_name[106];//���ڱ����ļ���(���ɴ��52��UNICODE��)
	bit file_name_type;			//�ļ�������1-UNICODE 0-ASCII��
	unsigned long remain_size;//�ȴ����ŵ�����ʣ���ֽ���(��ʼ���������ļ���С)


	FAT_FILE mp3File;

	FAT_DIR_ENTRY *fct=buf;
	unsigned long seq;
	unsigned char i,x,y;
	unsigned short time;
	RiffHeader *wavHead=FAT_BUF;


	tftInit();

	TFT_CHAR_TYPE=0;
	tftClear(0,0,128,160,tftRGB(0,31,31));

	//LOGO
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,0);
	tftSetCharWindow(4,16,120,94);
	tftShowChar(120,94,music_logo);

	//���ֲ�����
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,5,0);
	tftSetCharWindow((128-16*5)/2,2,16*5,16);
	for(i=0;i<5;i++)
		tftShowChar(16,16,yybfq+i*32);

	//��ʼ��...
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,21,10);
	tftSetCharWindow(8,128,15*8,16);
	for(i=0;i<3;i++)
		tftShowChar(16,16,csh+i*32);
	tftShowStr("...");

	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
	//��ʼ��Ӳ��
	if(sd_init())
	{
		tftShowStr("SD Err");
		while(1);
	}

	if(fat_init(0))
	{
		tftShowStr("FatErr");
		while(1);
	}

	if(hzk16_init())
	{
		tftShowStr("HzkErr");
		while(1);
	}
	if(hzk16_uni_init())
	{
		tftShowStr("UniErr");
		while(1);
	}

	vs1003_sine_test();	  //���Ҳ���
	vs1003_init();		   //VS1003��ʼ��
	vs1003_soft_reset();	//VS1003�����λ
//	vs1003_load_patch();	//��VS1003��Ƶ�׹��ܲ�������
	irInit();			 //�����ʼ��

	file_id=1;
	//�����м���SD���й��ж���������
	file_total=0;
	while(fat_open_file("????????MP3",file_total,&mp3File)==0)file_total++;
	file_total--;
				   
															 
	play_style=3;														   //Ĭ�ϲ��ŷ�ʽ
	play_stat=1;													   //��������
	surround_sound=0;													//�޻���		
	low_sound=0;													   //�޵���
	high_sound=0;														//�޸���
	vol=15;															   //Ĭ������


	//��ʼ���ɹ�
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,63,0);
	tftShowStr("[OK]");



	delay10ms(100);//�ȴ��������������


////////////////////////////////////////////////////////////////////////////
//����Ϊ����������
////////////////////////////////////////////////////////////////////////////

	frame_show();					//��ʾ���
	set_play_style(play_style);	  //���ò���ʾĬ�ϲ��ŷ�ʽ
	set_vol(vol);				   //���ò���ʾĬ������
	set_surround_sound(surround_sound);   //���ò���ʾĬ�ϻ�����
	set_low_sound(low_sound);	   //���ò���ʾĬ�ϵ���
	set_high_sound(high_sound);		   //���ò���ʾĬ�ϸ���
	set_play_stat(play_stat);				   //��ʾ��ͣ(ÿ�׸貥��ʱ���Զ��ı䲥��״̬)


	if(file_total==0)
	{
		name_show("SD����û�������ļ�!",0);
		while(1);
	}

	while(1)
	{
		if(play_stat==1)
		{	//���������ļ�
			if(fat_open_file("????????MP3",file_id,&mp3File))
				Error(2);
			remain_size=mp3File.file_size;
			i=mp3File.dir_entry_offset;
			sd_read_sector(mp3File.dir_entry_sector,buf);
			if(fat_has_long_file_name(fct+i))
			{	//��ʾ���ļ���
				for(x=0;x<4;x++)//���ȡ4�����ļ���
				{
					if(i==0)
					{//��ǰ�ļ����������ײ� ���ǰһ������ȡ���ļ���
						if(sd_read_sector(mp3File.dir_entry_sector-1,buf))	//��ǰһ������	(������һ��BUG��ǰһ�������Ų���һ���ǵ�ǰ������-1)
							Error(6);
						i=512/32-1;//ǰһ���������һ���ļ���
					}else{
						i--;										  //ǰ��һ���ļ���
					}
					if(fat_dir_entry_type(fct+i)==FAT_DIR_ENTRY_LONG_FILE)
					{//�ǳ��ļ���
						get_long_file_name_from_fct(fct+i,file_name+x*26);	
					}else
					{	//���ǳ��ļ�����
						break;
					}
				}
				file_name[x*26]=file_name[x*26+1]=0;	//���������0 ������Ч��UNICODE��
				file_name_type=1; //���ļ���
			}else
			{	//��ʾ���ļ���
				for(x=0;x<8;x++)
					file_name[x]=fct[i].file_name[x];
				file_name[8]='.';
				for(x=9;x<12;x++)
					file_name[x]=fct[i].extend_name[x-9];
				file_name[x]=0;
				file_name_type=0;  //���ļ���
			}
			name_show(file_name,file_name_type);				//��ʾ����
			samprate_show(0);	 //δ�����Ч������ǰ��ʾΪ0  ��ͬ
			bitrate_show(0);		
			filetype_show("NUL");
			seq=15;//��play_stat=2������ʱ��ʱ��ȡ�����ʵȵ�	 
			set_play_stat(2);	 //��ʾ����״̬	 //״̬��Ϊ�����ļ�������
			step_show(0,1);	//��ս�����
			time_show(0);
			vs1003_soft_reset();	//����оƬ��λ ׼������
			vs1003_load_patch();	//��������λ�����´򲹶�
		}
		else if(play_stat==2)
		{	//����������
			x=fat_file_read(&mp3File);
			if(x!=0 && x!=2)
				Error(4);
			if(remain_size<=512)
			{	//�ļ���������
				vs1003_sdi_write(buf,remain_size);
				remain_size=0;	//��������0 ��ֹ"setup"����ʱ���¼������
				step_show(60,0);//����󲻻�ִ�к���Ľ��������� ���������ﲹȫ
				switch(play_style)
				{
					case 1:	  //�������� ����� �ļ��Ų��� ��ͣ�ļ�δ����
						set_play_stat(3);
						break;
					case 2:	//����ѭ�� ����� �ļ��Ų��� �������ļ�δ����
						set_play_stat(1);
						break;
					case 3:	//˳�򲥷� ����� �ļ���+1 �����������ͣ�ļ�δ���� ���򲥷��ļ�δ���� 
						if(file_id==file_total)
							set_play_stat(3);
						else{
							file_id=file_id%file_total+1;
							set_play_stat(1);
						}
						break;
					case 4:	//˳��ѭ�� ����� �ļ���+1 �����ļ�δ����
						file_id=file_id%file_total+1;
						set_play_stat(1);
						break;
				}
				spec_show(0);//��ʾƵ�� ��Ӧ��д�����ݺ�������ȡƵ��(�����ڻ�ȡƵ��ʱ��ȴ�,�˷�ʱ��)
				continue;
			}
			else{ //������512�ֽ�ȫ������
				vs1003_sdi_write(buf,512);
				remain_size-=512;
				//�ƽ�����
				step_show((unsigned char)((mp3File.file_size-remain_size)*60/mp3File.file_size),0);		//������(total-remain)*60/total
				spec_show(0);//��ʾƵ�� ��Ӧ��д�����ݺ�������ȡƵ��(�����ڻ�ȡƵ��ʱ��ȴ�,�˷�ʱ��)
			}

			if(seq>1)	 //ÿ��һ��ʹseq-1��������ǰ����������Ϊ1
				seq--;
		}

		if(seq==1)		  //����ǰ���������� ���Ի�����ֲ�����
		{//��ʱ��ʾ����״̬
			time=vs1003_get_bitrate();		  //��ñ�����
			bitrate_show(time);				  //��ʾ������
			if(time!=0)
				time=mp3File.file_size/(time>>3)/1000;	   //���㲥��ʱ��
			samprate_show(vs1003_get_samprate());		 //��ʾ������

			switch(vs1003_get_file_type())	  //����ļ����Ͳ���ʾ
			{
				case VS1003_MP3:
					filetype_show("MP3");
					break;
				case VS1003_WMA:
					filetype_show("WMA");
					break;
				case VS1003_WAV:
					filetype_show("WAV");
					break;
				case VS1003_MID:
					filetype_show("MID");
					break;
				case VS1003_NUL:
					filetype_show("NUL");
					break;	
			}

			time_show(time);			   //��ʾ����ʱ��
			seq=0;						   //seq��0 һ�׸����ʾһ�����ֲ���
		}
		if(key(&x,&y))
		{	//�����������
//			delay1ms(50);//�������̶�LCD��Ӱ��
			switch(x*4+y)
			{
				case 0://����
					setup();		   //�������ô��ڡ����غ�ˢ�²�����(�����޸�surround_sound,low_sound,high_sournd,play_style����ֵ)
					frame_show();	  //ˢ��
					name_show(file_name,file_name_type);	//��ʾ�ļ���(�ѱ����)	
					samprate_show(vs1003_get_samprate());		 //��ʾ������
					bitrate_show(vs1003_get_bitrate());
					set_play_style(play_style);	  //���ò���ʾĬ�ϲ��ŷ�ʽ
					switch(vs1003_get_file_type())	  //����ļ����Ͳ���ʾ
					{
						case VS1003_MP3:
							filetype_show("MP3");
							break;
						case VS1003_WMA:
							filetype_show("WMA");
							break;
						case VS1003_WAV:
							filetype_show("WAV");
							break;
						case VS1003_MID:
							filetype_show("MID");
							break;
						case VS1003_NUL:
							filetype_show("NUL");
							break;	
					}
					set_vol(vol);				   //���ò���ʾĬ������
					set_surround_sound(surround_sound);   //���ò���ʾĬ�ϻ�����
					set_low_sound(low_sound);	   //���ò���ʾĬ�ϵ���
					set_high_sound(high_sound);		   //���ò���ʾĬ����
					set_play_stat(play_stat);		//���ò���ʾ����״̬
					spec_show(1);//������ʾƵ��
					step_show((unsigned char)((mp3File.file_size-remain_size)*60/mp3File.file_size),1);		//������ʾ������
					time_show(time);					   //��ʾʱ��
					break;
				case 1://��������
					set_vol(vol+1);
					break;
				case 2://¼��ģʽ
					recorder();
					//�����м���SD���й��ж���������
					file_total=0;
					while(fat_open_file("????????MP3",file_total,&mp3File)==0)file_total++;
					file_total--;
					frame_show();	  //ˢ��
					set_play_style(play_style);	  //���ò���ʾĬ�ϲ��ŷ�ʽ
					set_vol(vol);				   //���ò���ʾĬ������
					set_surround_sound(surround_sound);   //���ò���ʾĬ�ϻ�����
					set_low_sound(low_sound);	   //���ò���ʾĬ�ϵ���
					set_high_sound(high_sound);		   //���ò���ʾĬ����
					set_play_stat(1);	 //��ʾ����״̬	 //״̬��Ϊ�����ļ�δ����				
					break;
				case 4://��һ��
					if(file_id<=1)
						file_id=file_total;
					else
						file_id--;
					play_stat=1;
					vs1003_flush();
					break;
				case 5://����/��ͣ
					switch(play_stat)
					{
						case 1:
							vs1003_set_pause(1);
							set_play_stat(3);
							break;
						case 2:
							vs1003_set_pause(1);
							set_play_stat(4);
							break;
						case 3:
							vs1003_set_pause(0);
							set_play_stat(1);
							break;
						case 4:
							vs1003_set_pause(0);
							set_play_stat(2);
							break;
					}
					break;
				case 6://��һ��
					if(file_id>=file_total)
						file_id=1;
					else
						file_id++;
					play_stat=1;
					vs1003_flush();
					break;
				case 9://��������
					set_vol(vol-1);
					break;  
			}
		}		
	}
}


