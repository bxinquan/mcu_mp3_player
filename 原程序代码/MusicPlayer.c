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


//公共变量(该文件中所有的函数共同维护)
unsigned char play_stat;//播放状态 1-播放但文件未载入 2-播放文件已载入 3-暂停且文件未载入 4-暂停且文件已载入
unsigned char play_style;//播放方式(单曲循环等等){1-单曲播放,2-单曲循环,3-顺序播放,4-顺序循环播放}
bit surround_sound;//环绕音标记 1-有 0-无 下同
bit low_sound;//低音加重标记
bit high_sound;//高音加强标记
unsigned char vol;//音量[0,20]





//将vol音量转换为VS1003音量
unsigned char code vol_tab[21]=
{0,80,110,140,160,180,190,200,205,210,215,220,225,230,235,238,241,245,248,251,254};

//集成红外的键盘　
bit key(unsigned char *x,unsigned char *y)
{
	if(key1(x,y))  //读板上键盘
		return 1;
	if(irRead(x,y))	  //读红外键盘
	{	delay1ms(100);//消弱红外对LCD的影响
		switch(*y)
		{
			case 0x46:
			case 0x0c://对应0
				*x=0;*y=0;
				break;
			case 0x09:
			case 0x18://对应1
				*x=0;*y=1;
				break; 
			case 0x47:
			case 0x5e://对应2
				*x=0;*y=2;
				break;
			case 0x40:
			case 0x08://对应4
				*x=1;*y=0;
				break;
			case 0x44:
			case 0x1c://对应5
				*x=1;*y=1;
				break;
			case 0x43:
			case 0x5a://对应6
				*x=1,*y=2;
				break;
			case 0x15:
			case 0x52://对应9
				*x=2;*y=1;
				break;
		}
		return 1;
	}
	return 0;

}


//错误处理
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
//函数名:get_long_file_name_from_fct
//功  能:从文件项中提出合法的字串
//参  数:fct为文件项,name为存放文件名数组(27B)
//返回值:
//备  注:外部
//更  新:2012.3.2									
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


//播放器主框架显示
void frame_show(void)
{
	tftClear(0,0,128,160,tftRGB(0,31,31));

	//Music Player

	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	tftSetCharWindow((128-12*8)/2,3,12*8,16);
	tftShowStr("Music Player");

	//歌名框
	tftDrawRect(6,17,6+114+1,19+64+1,1,tftRGB(10,21,10));
	//采样率框
	tftDrawRect(8,87,8+5*8,87+16,1,tftRGB(10,21,10));
	//比特率框
	tftDrawRect(64,87,64+7*8,87+16,1,tftRGB(10,21,10));
	//文件类型框
	tftDrawRect(96,106,96+8*3,106+16,1,tftRGB(10,21,10));
	//音量框
	tftDrawRect(24,134,24+40+2+2,134+3+1,1,tftRGB(10,21,10));
	//进度框
	tftDrawRect(23,149,23+60+1,149+4+1,1,tftRGB(10,21,10));

}

//状态显示
void set_play_stat(unsigned char stat)
{
	if(stat<0 || stat>4)
		return;
	play_stat=stat;//修改变量

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


//音量设置并显示
void set_vol(unsigned char v)
{
	if(v>20)
		return;
	vol=v;										//修改音量
	vs1003_set_volume(vol_tab[vol],vol_tab[vol]);	  //设置音量


	//显示音量条
	tftClear(24+2+v*2,134+2,40-v*2,1,tftRGB(10,21,10));	//清除当前音量对应像素以后的像素
	tftClear(24+2,134+2,v*2,1,tftRGB(30,0,0));//一个音量用两个像素表示　所以v*2

	//显示音量图标
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(8,128,16,16);
	if(v)
		tftShowChar(16,16,vol_yes_ico);
	else
		tftShowChar(16,16,vol_no_ico);
}

//歌名显示
//name为要显示的完整文件名(uni=1UNICODE码 uni=0ASCII)
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

//采样率显示
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
//比特率显示
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


//播放方式设置并显示
void set_play_style(unsigned char n)
{
	if(n<1 || n>4)
		return;
	play_style=n;	//修改播放方式

//显示播放方式
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
//文件类型显示
void filetype_show(unsigned char *type)
{
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(20,40,20);
	tftSetCharWindow(97,107,24,16);
	tftShowStr(type);	
}

//文件总时间显示
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


//环绕音设置并图标显示  (1-明显　0-暗显)
void set_surround_sound(bit x)
{

	surround_sound=x;//修改变量

	vs1003_set_surround(surround_sound);  //设置

	//显示
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(72,128,16,16);
	if(surround_sound)
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,10);
	else
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,26,28);
	tftShowChar(16,16,surround_sound_ico);		

}

//高低音的设置参照音乐软件的音效调节
//低音设置并图标显示 (1-明显 0-暗显)
void set_low_sound(bit x)
{
	low_sound=x;//修改变量
	if(low_sound)	//设置
	{
		vs1003_set_bass(15,15);	 //在150Hz上增强分贝 低频在150Hz上加分贝比较明显
		if(high_sound==0)		   //高音不存在 拉低高音
			vs1003_set_treble(-8,10);				//在10KHz上降低分贝
	}	
	else
	{
		vs1003_set_bass(0,0);
		if(high_sound==0)		//高音不存在 关闭高音
			vs1003_set_treble(0,0);		
	}


//显示
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
	tftSetCharWindow(88,128,16,16);
	if(low_sound)
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,10,10);
	else
		TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,26,28);
	tftShowChar(16,16,low_sound_ico);

}

//高音设置并图标显示 (1-明显 0-暗显)
void set_high_sound(bit x)
{
	high_sound=x;//修改变量
	if(high_sound)			//设置
	{
		vs1003_set_treble(7,10);   //高频在10KHz上加分贝比较明显
	}		
	else
	{
		if(low_sound)			 //低音存在 高音降低
			vs1003_set_treble(-8,10);
		else
			vs1003_set_treble(0,0);
	}

//显示
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
//函数名:step_show
//功  能:进度条推进
//参  数:length为进度条的长度 reset为1表示从开始位置重新显示整个进度条 reset为0表示仅显示增加的部分
/****************************************************************/
void step_show(unsigned char length,bit reset)
{
	static unsigned char fre_len;//进度条已显示的长度 (起初为0即从开始点画进度条 ,随后每显示一次就将进度条当前长度赋予它　即下次不再从头显示进度条直至reset置位)
	unsigned short color=tftRGB(0x5f>>3,0x3c>>4,0x23>>3);//进度条颜色
	if(length>60)
		return;
	if(reset||length<fre_len)  //从开始处重新显示
	{
		fre_len=0;
		tftClear(24,150,60,4,tftRGB(0,31,31));//置位后清除进度条
	}															 
	if(length==fre_len)//与上次的进度条长度相同
		return;

	tftClear(24+fre_len,150,length-fre_len,4,color);
	fre_len=length;//刷新已显示的长度
}



//显示频谱
void spec_show(bit reset)
{

	unsigned short color1=tftRGB(31,0,0);	  //前景色
	unsigned short color2=tftRGB(0,31,31); 		//背景色
	static unsigned short spec1[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned short spec2[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//ptr1用于存储将要获取的频谱 ptr2用于存储上次已显示的频谱值(通过比较两次的值只显示增量值或清除降量值 避免浪费时间)	
	static unsigned short *ptr1=spec1,*ptr2=spec2;	
	unsigned short *tmp;
	unsigned char i,val1,val2,x;

	//显示完整的频谱			
	if(reset)
	{
		for(i=0;i<14;i++)
			ptr2[i]=0;
	}

	x=60;//柱状图X坐标
	vs1003_get_spec(ptr1);
	for(i=0;i<14;i++) //显示柱状图
	{
		val1=ptr1[i]&0x000f;	//取后4位效果明显
		val2=ptr2[i]&0x000f;   	//取后4位效果明显
		if(val1>val2)			//新值比旧值多
		{
			tftClear(x,108+16-val1,1,val1-val2,color1);//仅显示增量
		}else if(val1<val2)	  //新值比旧值少
		{
			tftClear(x,108+16-val2,1,val2-val1,color2);//仅清除降量
		}
		//若相等本次不再显示
		x+=2;	
	}
	tmp=ptr1;
	ptr1=ptr2;
	ptr2=tmp;
}




/**********************************************************************/
//函数名:setup
//功  能:显示设置窗口供用户设置　确定后完成相关设置　
/**********************************************************************/
void setup(void)
{
	bit surround,low,high;//音效设置变量
	unsigned char style;   //播放方式设置变量
	unsigned char key_stat;//当前选项状态0~6共7个选项
	unsigned char code option_pos[7]={2*16,3*16,4*16,6*16,7*16,8*16,9*16};//各选项的Y坐标
	unsigned short color1,color2;//活动框的前景色及消退色
	unsigned char x,y;

	tftClear(0,0,128,160,tftRGB(0,31,31)); //刷屏

	TFT_CHAR_TYPE=0;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
//设置
	tftSetCharWindow((128-2*16)/2,0,2*16,16);
	tftShowGB("设置");
//音效
	tftSetCharWindow(0,1*16,2*16,16);
	tftShowGB("音效");
//环绕音
	tftSetCharWindow(32,2*16,3*16,16);
	tftShowGB("环绕音");
//低音加重
	tftSetCharWindow(32,3*16,4*16,16);
	tftShowGB("低音加重");
//高音加强
	tftSetCharWindow(32,4*16,4*16,16);
	tftShowGB("高音加强");
//播放方式
	tftSetCharWindow(0,5*16,4*16,16);
	tftShowGB("播放方式");
//单曲播放
	tftSetCharWindow(32,6*16,4*16,16);
	tftShowGB("单曲播放");
//单帕循环
	tftSetCharWindow(32,7*16,4*16,16);
	tftShowGB("单曲循环");
//顺序播放
	tftSetCharWindow(32,8*16,4*16,16);
	tftShowGB("顺序播放");
//顺序循环
	tftSetCharWindow(32,9*16,4*16,16);
	tftShowGB("顺序循环");

	
	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);
//环绕音复选框
	tftSetCharWindow(16,2*16,16,16);
	if(surround_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//低音加重复选框
	tftSetCharWindow(16,3*16,16,16);
	if(low_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//高音加强复选框
	tftSetCharWindow(16,4*16,16,16);
	if(high_sound)
		tftShowChar(16,16,check_yes_ico);
	else
		tftShowChar(16,16,check_no_ico);
//单曲播放音选框
	tftSetCharWindow(16,6*16,16,16);
	if(play_style==1)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//单曲循环音选框
	tftSetCharWindow(16,7*16,16,16);
	if(play_style==2)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//顺序播放音选框
	tftSetCharWindow(16,8*16,16,16);
	if(play_style==3)
		tftShowChar(16,16,option_yes_ico);
	else
		tftShowChar(16,16,option_no_ico);
//顺序循环音选框
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
		if(key(&x,&y))//有按键
		{
			delay1ms(50);//消弱键盘对LCD的影响
			switch(x*4+y)
			{
				case 1://上
					if(key_stat==0)
						break;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color2);//消色
					key_stat--;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//加色
					break;
				case 9://下
					if(key_stat==6)
						break;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color2);//消色
					key_stat++;
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//加色
					break;
				case 5://选中
					switch(key_stat)
					{
						case 0://环绕音
							surround=~surround;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(surround)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 1://低音
							low=~low;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(low)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 2://高音
							high=~high;
						 	tftSetCharWindow(16,option_pos[key_stat],16,16);
							if(high)
								tftShowChar(16,16,check_yes_ico);
							else
								tftShowChar(16,16,check_no_ico);
							break;
						case 3://单曲播放
						case 4://单曲循环
						case 5://顺序播放
						case 6://顺序循环
							tftSetCharWindow(16,option_pos[style+2],16,16);
							tftShowChar(16,16,option_no_ico);
							style=key_stat-2;
							tftSetCharWindow(16,option_pos[key_stat],16,16);
							tftShowChar(16,16,option_yes_ico);
							break;
					}
					tftDrawRect(16,option_pos[key_stat],31,option_pos[key_stat]+15,1,color1);//加色
					break;
				case 0://确定
					surround_sound=surround;
					low_sound=low;
					high_sound=high;
					play_style=style;
					return;					
				case 2://取消
					return;
			}
		}				
	}

}



/**********************************************************************/
//函数名:record
//功  能:显示录音机界面 供用户设置 设置确定后开始录音 录音数据保存在SD卡中
/**********************************************************************/
void recorder(void)
{
	unsigned char option_stat;//当前设置项 0-文件名序号 1-时 2-分 3-秒
	unsigned char x,y;
	bit isLock;//锁定选项 只有先锁定才能更改值
	unsigned short lockColor=tftRGB(31,0,0);//选项被锁定时的前景色
	unsigned short unlockColor=tftRGB(5,5,5);//选项未被锁定时的前景色
	unsigned char code option_pos[][2]=
	{
		{76,16*2},//文件序号选项位置
		{16,16*4},//时选项位置
		{48,16*4},//分选项位置
		{80,16*4}	//秒位置
	};
	unsigned char code option_max[4]={99,99,59,59};  //各选项所允许的最大值
	unsigned char option_val[4]={0,0,1,0};//各选项的当前值

	FAT_FILE file;
	unsigned char file_name[11]={'R','E','C','O','R','D',' ',' ','M','P','3'};
	unsigned long file_size;
	RiffHeader *head=FAT_BUF;

	tftClear(0,0,128,160,tftRGB(0,31,31)); //刷屏

	TFT_CHAR_TYPE=1;
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(5,5,5);
	TFT_CHAR_WINDOW_BACKCOLOR=tftRGB(0,31,31);

	//录音机
	tftSetCharWindow((128-3*16)/2,0,3*16,16);
	tftShowGB("录音机");
	//保存文件名为:
	tftSetCharWindow(0,16,7*16,16);
	tftShowGB("保存文件名为:");
	//RECORD
	tftSetCharWindow((128-9*8)/2,16*2,6*8,16);
	tftShowGB("RECORD");
	//最大录音时间为:
	tftSetCharWindow(0,16*3,8*16,16);
	tftShowGB("最大录音时间为:");
	//  时  分  秒
	tftSetCharWindow((128-6*16)/2,16*4,6*16,16);
	tftShowGB("  时  分  秒");

	option_stat=0; //当前选项为文件序号
	isLock=0;	   //未锁定状态

	tftSetCharWindow(option_pos[0][0],option_pos[0][1],16,16);
	tftShowULong(option_val[0]);   //显示文件序号
	tftSetCharWindow(option_pos[1][0],option_pos[1][1],16,16);
	tftShowULong(option_val[1]);   //显示时
	tftSetCharWindow(option_pos[2][0],option_pos[2][1],16,16);
	tftShowULong(option_val[2]);   //显示分
	tftSetCharWindow(option_pos[3][0],option_pos[3][1],16,16);
	tftShowULong(option_val[3]);   //显示秒
	tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));
	while(1)
	{
		if(key(&x,&y))
		{
			delay1ms(50);//消弱键盘对LCD的影响
			switch(x*4+y)
			{
				case 0://用户确定了 开始准备空间 录音
//					tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(0,31,31));//消框
					TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
					tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
					tftShowULong(option_val[option_stat]);	//重显当前值
					if(option_val[option_stat]<10)
					tftShowStr(" ");//第二位补空

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

					file_size=((unsigned long)option_val[1]*60+option_val[2])*60+option_val[3];//计算秒数
					if(file_size==0)
					{//秒数为0
						TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
						tftSetCharWindow(0,5*16,128,32);
						tftShowGB("录音时间太短!");
						delay10ms(200);
						tftClear(0,5*16,128,32,tftRGB(0,31,31));
						option_stat=3;//当前选项设为秒
						isLock=1;//锁定该项
						TFT_CHAR_WINDOW_FORECOLOR=lockColor;
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//重显当前值
						if(option_val[option_stat]<10)
						tftShowStr(" ");//降值时 可能从两位降为一位 所以第二位补空					
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框																								
						break;
					}
					file_size*=8110ul;	// 8110B/s
					file_size+=512;//计算文件大小


					if(fat_open_file(file_name,1,&file)==0)
					{	//该文件已存在 提示用户
						TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
						tftSetCharWindow(0,5*16,128,32);
						tftShowGB("文件已存在,是否覆盖?");
						option_stat=1;	 //借用变量
						while(option_stat)
						{
							if(key(&x,&y))
							{
								delay1ms(50);//消弱键盘对LCD的影响	
								switch(x*4+y)
								{
									case 0://覆盖原文件
										isLock=1;//借用变量
										option_stat=0;
										break;
									case 2://不覆盖原文件
										isLock=0;//借用变量
										option_stat=0;
										break;
								}
							}
						}
						tftClear(0,5*16,128,32,tftRGB(0,31,31));
						if(isLock==0)
						{//用户要求不覆盖
							option_stat=0;//当前选项设为文件序号
							isLock=1;//锁定该项
							TFT_CHAR_WINDOW_FORECOLOR=lockColor;
							tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
							tftShowULong(option_val[option_stat]);	//重显当前值
							if(option_val[option_stat]<10)
								tftShowStr(" ");//第二位补空					
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框																								
							break;						
						}
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
						tftSetCharWindow(0,5*16,128,16);
						tftShowGB("准备空间中...");
						if(fat_file_resize(file_size,&file))
							Error(30);
						tftShowGB("好");	
					}else
					{
						//创建文件
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
						tftSetCharWindow(0,5*16,128,16);
						tftShowGB("准备空间中...");
						if(fat_open_file(file_name,0,&file))
							Error(31);
						if(fat_file_resize(file_size,&file))
							Error(32);
						tftShowGB("好");
					}
					//所在准备工作已就绪
					tftSetCharWindow(0,6*16,128,16);
					tftShowGB("按任意键开始");
					if(fat_file_write(&file))//写假头模板
						Error(33);
					tftSetCharWindow(0,7*16,128,16);
					while(key(&x,&y)==0);  //等待用户按键
					delay1ms(50);//消弱键盘对LCD的影响
					vs1003_start_adpcm(0,1);//从麦克风 开启高通滤波	开启录音模式
					tftShowGB("录音中...");
					tftSetCharWindow(0,8*16,128,16);
					tftShowGB("按任意键停止录音");
					file_size-=512;
					while(1)
					{
						vs1003_get_adpcm(FAT_BUF);	 //读取录音数据
						vs1003_get_adpcm(FAT_BUF+256);
						if(fat_file_write(&file))	//写入文件中
							Error(34);
						if(file_size<512)	 //就后一个不满扇区的数据将丢弃
						{
							file_size=0;
							break;
						}
						file_size-=512;
						if(key(&x,&y))//用户按键了 离开
						{
							delay1ms(50);//消弱键盘对LCD的影响
							break;
						}
					}
					vs1003_stop_adpcm();//关闭录音模式
					if(fat_file_resize(file.file_size - file_size,&file))  //重置文件大小
						Error(35);
					if(fat_file_set_offset(0,&file))
						Error(36);
					for(lockColor=0;lockColor<512;lockColor++)	   //复制文件头模板
						FAT_BUF[lockColor]=RIFF_header[lockColor];
					head->ChunkSize=file.file_size-8;//填充文件长度
					fat_l2be((U8*)&(head->ChunkSize),4);
					head->SubChunk3Size = file.file_size-512;//填充数据长度
					fat_l2be((U8*)&(head->SubChunk3Size),4);
					head->NumOfSamples=(file.file_size-512)/256 * 505ul;	//填充采样数
					fat_l2be((U8*)&(head->NumOfSamples),4);
					if(fat_file_write(&file)) //写回首扇区
						Error(37);				
					if(fat_close_file(&file))  //关闭文件
						Error(38);
					tftSetCharWindow((128-4*16)/2,9*16,4*16,16);
					tftShowGB("录音完成");
					delay10ms(200);
					return ;
				case 1://用户按了上键
					if(isLock)
					{//在这里增值
						option_val[option_stat] = (option_val[option_stat]+1) % (option_max[option_stat]+1); //增值
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//重显当前值
						if(option_val[option_stat]<10)
						tftShowStr(" ");//降值时 可能从两位降为一位 所以第二位补空					
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框											
					}else
					{//在这上移一个选项
						if(option_stat>0)
						{
							option_stat--;
							tftDrawRect(option_pos[option_stat+1][0],option_pos[option_stat+1][1],option_pos[option_stat+1][0]+15,option_pos[option_stat+1][1]+15,1,tftRGB(0,31,31));//消框
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框
						}					
					}
					break;
				case 2://用户取消了本次录音操作
					return ;
				case 5://用户按了锁定/解锁定键
					if(isLock)
					{//解锁定
						isLock=0;//未锁定标记
						TFT_CHAR_WINDOW_FORECOLOR=unlockColor;
					}else
					{//锁定当前项
						isLock=1;//锁定标记
						TFT_CHAR_WINDOW_FORECOLOR=lockColor;
					}
					tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
					tftShowULong(option_val[option_stat]);						
					tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));
					break;
				case 9://用户按了下键
					if(isLock)
					{//在这里降值
						if(option_val[option_stat]==0)
							option_val[option_stat]=option_max[option_stat];
						else
							option_val[option_stat]--; //降值
						tftSetCharWindow(option_pos[option_stat][0],option_pos[option_stat][1],16,16);
						tftShowULong(option_val[option_stat]);	//重显当前值
						if(option_val[option_stat]<10)
						tftShowStr(" ");//降值时 可能从两位降为一位 所以第二位补空				
						tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框										
					}else
					{//在这上移一个选项
						if(option_stat<3)
						{
							option_stat++;
							tftDrawRect(option_pos[option_stat-1][0],option_pos[option_stat-1][1],option_pos[option_stat-1][0]+15,option_pos[option_stat-1][1]+15,1,tftRGB(0,31,31));//消框
							tftDrawRect(option_pos[option_stat][0],option_pos[option_stat][1],option_pos[option_stat][0]+15,option_pos[option_stat][1]+15,1,tftRGB(31,0,0));//显示框
						}					
					}
					break;
			}	
		}		
	}
}



//音乐播放器主程序
void main()
{
	unsigned char *buf=FAT_BUF;//公用缓冲区
	unsigned short file_total;//根目录中音乐文件总数
	unsigned short file_id;//当前文件号(从1开始,最大到file_total)
	unsigned char file_name[106];//用于保存文件名(最多可存放52个UNICODE码)
	bit file_name_type;			//文件名类型1-UNICODE 0-ASCII码
	unsigned long remain_size;//等待播放的音乐剩余字节数(起始等于音乐文件大小)


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

	//音乐播放器
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,5,0);
	tftSetCharWindow((128-16*5)/2,2,16*5,16);
	for(i=0;i<5;i++)
		tftShowChar(16,16,yybfq+i*32);

	//初始化...
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(10,21,10);
	tftSetCharWindow(8,128,15*8,16);
	for(i=0;i<3;i++)
		tftShowChar(16,16,csh+i*32);
	tftShowStr("...");

	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(31,0,0);
	//初始化硬件
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

	vs1003_sine_test();	  //正弦测试
	vs1003_init();		   //VS1003初始化
	vs1003_soft_reset();	//VS1003软件复位
//	vs1003_load_patch();	//给VS1003打频谱功能补丁程序
	irInit();			 //红外初始化

	file_id=1;
	//下三行计算SD卡中共有多少首音乐
	file_total=0;
	while(fat_open_file("????????MP3",file_total,&mp3File)==0)file_total++;
	file_total--;
				   
															 
	play_style=3;														   //默认播放方式
	play_stat=1;													   //开机播放
	surround_sound=0;													//无环绕		
	low_sound=0;													   //无低音
	high_sound=0;														//无高音
	vol=15;															   //默认音量


	//初始化成功
	TFT_CHAR_WINDOW_FORECOLOR=tftRGB(0,63,0);
	tftShowStr("[OK]");



	delay10ms(100);//等待数秒进入主界面


////////////////////////////////////////////////////////////////////////////
//以下为主操作界面
////////////////////////////////////////////////////////////////////////////

	frame_show();					//显示框架
	set_play_style(play_style);	  //设置并显示默认播放方式
	set_vol(vol);				   //设置并显示默认音量
	set_surround_sound(surround_sound);   //设置并显示默认环绕音
	set_low_sound(low_sound);	   //设置并显示默认低音
	set_high_sound(high_sound);		   //设置并显示默认高音
	set_play_stat(play_stat);				   //显示暂停(每首歌播放时会自动改变播放状态)


	if(file_total==0)
	{
		name_show("SD卡中没有音乐文件!",0);
		while(1);
	}

	while(1)
	{
		if(play_stat==1)
		{	//载入音乐文件
			if(fat_open_file("????????MP3",file_id,&mp3File))
				Error(2);
			remain_size=mp3File.file_size;
			i=mp3File.dir_entry_offset;
			sd_read_sector(mp3File.dir_entry_sector,buf);
			if(fat_has_long_file_name(fct+i))
			{	//显示长文件名
				for(x=0;x<4;x++)//最多取4个长文件项
				{
					if(i==0)
					{//当前文件项在扇区首部 需从前一个扇区取长文件名
						if(sd_read_sector(mp3File.dir_entry_sector-1,buf))	//读前一个扇区	(隐含了一个BUG　前一个扇区号并不一定是当前扇区号-1)
							Error(6);
						i=512/32-1;//前一个扇区最后一个文件项
					}else{
						i--;										  //前移一个文件项
					}
					if(fat_dir_entry_type(fct+i)==FAT_DIR_ENTRY_LONG_FILE)
					{//是长文件项
						get_long_file_name_from_fct(fct+i,file_name+x*26);	
					}else
					{	//不是长文件项了
						break;
					}
				}
				file_name[x*26]=file_name[x*26+1]=0;	//最后添两个0 够成有效的UNICODE码
				file_name_type=1; //长文件名
			}else
			{	//显示短文件名
				for(x=0;x<8;x++)
					file_name[x]=fct[i].file_name[x];
				file_name[8]='.';
				for(x=9;x<12;x++)
					file_name[x]=fct[i].extend_name[x-9];
				file_name[x]=0;
				file_name_type=0;  //短文件名
			}
			name_show(file_name,file_name_type);				//显示歌名
			samprate_show(0);	 //未获得有效采样率前显示为0  下同
			bitrate_show(0);		
			filetype_show("NUL");
			seq=15;//在play_stat=2送数据时适时获取比特率等等	 
			set_play_stat(2);	 //显示播放状态	 //状态改为播放文件已载入
			step_show(0,1);	//清空进度条
			time_show(0);
			vs1003_soft_reset();	//解码芯片软复位 准备解码
			vs1003_load_patch();	//由于软软复位需重新打补丁
		}
		else if(play_stat==2)
		{	//送音乐数据
			x=fat_file_read(&mp3File);
			if(x!=0 && x!=2)
				Error(4);
			if(remain_size<=512)
			{	//文件即将放完
				vs1003_sdi_write(buf,remain_size);
				remain_size=0;	//播放完置0 防止"setup"返回时重新计算错误
				step_show(60,0);//放完后不会执行后面的进度条更新 所以在这里补全
				switch(play_style)
				{
					case 1:	  //单曲播放 放完后 文件号不变 暂停文件未载入
						set_play_stat(3);
						break;
					case 2:	//单曲循环 放完后 文件号不变 播放且文件未载入
						set_play_stat(1);
						break;
					case 3:	//顺序播放 放完后 文件号+1 若到最后则暂停文件未载入 否则播放文件未载入 
						if(file_id==file_total)
							set_play_stat(3);
						else{
							file_id=file_id%file_total+1;
							set_play_stat(1);
						}
						break;
					case 4:	//顺序循环 放完后 文件号+1 播放文件未载入
						file_id=file_id%file_total+1;
						set_play_stat(1);
						break;
				}
				spec_show(0);//显示频谱 不应在写完数据后立即获取频谱(否则在获取频谱时会等待,浪费时间)
				continue;
			}
			else{ //满满的512字节全是音乐
				vs1003_sdi_write(buf,512);
				remain_size-=512;
				//推进度条
				step_show((unsigned char)((mp3File.file_size-remain_size)*60/mp3File.file_size),0);		//进度条(total-remain)*60/total
				spec_show(0);//显示频谱 不应在写完数据后立即获取频谱(否则在获取频谱时会等待,浪费时间)
			}

			if(seq>1)	 //每读一个使seq-1让它跳过前几个扇区后为1
				seq--;
		}

		if(seq==1)		  //跳过前几个扇区了 可以获得音乐参数了
		{//适时显示音乐状态
			time=vs1003_get_bitrate();		  //获得比特率
			bitrate_show(time);				  //显示比特率
			if(time!=0)
				time=mp3File.file_size/(time>>3)/1000;	   //计算播放时间
			samprate_show(vs1003_get_samprate());		 //显示采样率

			switch(vs1003_get_file_type())	  //获得文件类型并显示
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

			time_show(time);			   //显示播放时间
			seq=0;						   //seq置0 一首歌仅显示一次音乐参数
		}
		if(key(&x,&y))
		{	//处理键盘输入
//			delay1ms(50);//消弱键盘对LCD的影响
			switch(x*4+y)
			{
				case 0://设置
					setup();		   //调用设置窗口　返回后刷新并设置(它会修改surround_sound,low_sound,high_sournd,play_style变量值)
					frame_show();	  //刷屏
					name_show(file_name,file_name_type);	//显示文件名(已保存的)	
					samprate_show(vs1003_get_samprate());		 //显示采样率
					bitrate_show(vs1003_get_bitrate());
					set_play_style(play_style);	  //设置并显示默认播放方式
					switch(vs1003_get_file_type())	  //获得文件类型并显示
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
					set_vol(vol);				   //设置并显示默认音量
					set_surround_sound(surround_sound);   //设置并显示默认环绕音
					set_low_sound(low_sound);	   //设置并显示默认低音
					set_high_sound(high_sound);		   //设置并显示默认唪
					set_play_stat(play_stat);		//设置并显示播放状态
					spec_show(1);//重置显示频谱
					step_show((unsigned char)((mp3File.file_size-remain_size)*60/mp3File.file_size),1);		//重新显示进度条
					time_show(time);					   //显示时间
					break;
				case 1://音量增加
					set_vol(vol+1);
					break;
				case 2://录音模式
					recorder();
					//下三行计算SD卡中共有多少首音乐
					file_total=0;
					while(fat_open_file("????????MP3",file_total,&mp3File)==0)file_total++;
					file_total--;
					frame_show();	  //刷屏
					set_play_style(play_style);	  //设置并显示默认播放方式
					set_vol(vol);				   //设置并显示默认音量
					set_surround_sound(surround_sound);   //设置并显示默认环绕音
					set_low_sound(low_sound);	   //设置并显示默认低音
					set_high_sound(high_sound);		   //设置并显示默认唪
					set_play_stat(1);	 //显示播放状态	 //状态改为播放文件未载入				
					break;
				case 4://上一曲
					if(file_id<=1)
						file_id=file_total;
					else
						file_id--;
					play_stat=1;
					vs1003_flush();
					break;
				case 5://播放/暂停
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
				case 6://下一曲
					if(file_id>=file_total)
						file_id=1;
					else
						file_id++;
					play_stat=1;
					vs1003_flush();
					break;
				case 9://音量减少
					set_vol(vol-1);
					break;  
			}
		}		
	}
}


