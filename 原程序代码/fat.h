

/**********************************************************/
//FAT32文件系统操作头文件
//主要用于51单片机
//可在根目录下进行"新建/打开/关闭/删除"文件
//卞新权
//2012.3.20
/**********************************************************/




#ifndef __fat_h__ 
#define __fat_h__ 

#include"sd.h"



//数据类型重定义
#define U8		unsigned char
#define U16		unsigned short
#define U32		unsigned long


/******************配置********************/
#define	FAT_SECTOR_SIZE			512					//物理磁盘的扇区大小
#define FAT_READ_SECTOR			sd_read_sector	  	//磁盘的读扇区函数(函数格式U8 read_sector(U32 sector,U8 *buf);)
#define	FAT_WRITE_SECTOR		sd_write_sector		//磁盘的写扇区函数(函数格式U8 write_sector(U32 sector,U8 *buf);)
/******************************************/




/*******************************************/
//公用缓冲区(大小等于磁盘扇区大小)
//此缓冲区只在本头文件中的部分函数执行时使用
//函数执行完后 用户可使用
U8		FAT_BUF[FAT_SECTOR_SIZE];
/*******************************************/





//主引导记录的磁盘分区表DPT每项的数据结构(16字节)
//位于MBR偏移446字节
/*
分区的操作系统类型(文件格式标志码)
00H--未知类型或不用
01H--12位FAT
04H---DOS FAT1632M
0BH--FAT32容量最大2G
0CH--FAT32,采用LBA模式，调用int 13h扩展中断
0FH--扩展FAT32分区，采用LBA模式，调用int 13h扩展中断
07H---NTFS(OS/2)
82H--linux
83H---LINUX swap>64M
*/

typedef struct
{
	unsigned char stat;//分区状态 0x00--非活动的 0x80--活动的
	unsigned char start_CHS[3];//分区起始CHS地址
	unsigned char type;//分区类型 0x0b,0x0c,0x0f为FAT32系统
	unsigned char end_CHS[3];//分区结束CHS地址
	unsigned long start_sector;//分区的起始LBA地址（即起始扇区 采用小端模式）
	unsigned long size;//分区大小扇区数(采用小端模式)
}FAT_DPT;




/*
搜索DBR的标志：

FAT16的DBR:EB 3C 90　

FAT32的DBR:EB 58 90 

NTFS的DBR:   EB 52 90
*/
/*****************************************************/
//FAT32文件系统DBR的数据结构(512字节)
/*****************************************************/
typedef struct
{
	U8		jmp[3];							//JMP代码
	U8		oem[8];							//OEM
//以下的53字节为BPB信息
	U16		bytes_per_sector;				//每扇区字节数 **
	U8		sectors_per_cluster;				//每簇扇区数 **
	U16		reserved_sectors;				//保留扇区数 第一个FAT表到本分区首的扇区数 即FAT表1相对于本分区起始位置的偏移扇区数  **
	U8		number_of_fats;					//FAT表的个数 **
	U16		root_entries;					//根目录项数 只有FAT12/FAT16使用此字段 FAT32置0
	U16		small_sectors;					//分区总扇区数（当分区小于32MB时使用,超过时此处置0,使用large_sectors）
	U8		media_descriptor;				//介质描述符：F8表示硬盘 F0表示3.5寸软盘
	U16		sectors_per_fat16;				//FAT表所占扇区数，FAT16/FAT12下使用，FAT32不使用此字段(而使用sectors_per_fat32)
	U16		sectors_per_track;				//每磁道扇区数
	U16		number_of_head;					//磁头数
	U32		hidden_sectors;					//隐含扇区数，从0到DBR的扇区数	**
	U32		large_sectors;					//本分区总扇区数 **
	U32		sectors_per_fat32;				//每个FAT32表的大小扇区数 仅被FAT32使用**
	U8		extended_flag[2];				//扩展标记 只被FAT32使用
	U8		file_system_version[2];			//文件系统版本　只被FAT32使用
	U32		root_cluster_number;			//根目录簇号	只被FAT32使用 **
	U16		fsinfo_sector_number;			//文件系统信息扇区号　只被FAT32使用
	U16		backup_boot_sector_number;		//备份引导扇区号 只被FAT32使用
	U8		reserved[12];					//保留 只被FAT32使用
//以下的26字节为扩展的BPB信息
	U8		physical_drive_number;			//BIOS设备
	U8		reserved2;						//未使用
	U8		expend_boot_flag;				//扩展引导标志（28H,29H）
	U32		volume_serial_number;			//卷序列号 一个随机值 用于区分磁盘
	U8		volume_label[11];				//卷标
	U8		system_id[8];					//文件系统格式 一般为"FAT32" **
//后面为操作系统引导代码
}FAT_DBR;


/*****************************************************/
//FAT32目录项数据结构
//FAT32短文件目录项32个字节的表示定义
/*****************************************************/
typedef struct
{
	U8		file_name[8];					//文件名 (name[0]=0x00时,此项未使用 name[0]=0xe5已删除项)
	U8		extend_name[3];					//扩展名
	U8		file_attr;							//属性字节 (0x0f此项为长文件名项)
	U8		reserved;						//保留字节
	U8		create_milli_sec;				//创建时间的10毫秒位
	U8		create_time[2];					//文件创建时间
	U8		create_date[2];					//文件创建日期
	U8		access_date[2];					//文件最后访问日期
	U16		start_cluster_h16;				//文件起始簇号的高16位
	U8		alter_time[2];					//文件的最近修改时间
	U8		alter_date[2];					//文件的最近修改日期
	U16		start_cluster_l16;				//文件起始簇号的低16位
	U32		file_size;						//文件的长度(以字节为单位) 子目录此项为0
}FAT_DIR_ENTRY;



/*****************************************************/
//FAT32文件系统基本信息
/*****************************************************/
U8		FAT_Type;					//文件系统类型 16-FAT16 32-FAT32
U16		FAT_BytesPerSector;			//每扇区字节数 **
U8		FAT_SectorsPerCluster;		//每簇扇区数 **
U32		FAT_HiddenSectors;			//隐含扇区数，从0到DBR的扇区数	**
U16		FAT_ReservedSectors;		//保留扇区数  **
U32		FAT_SectorsPerFat;			//每个FAT表的大小扇区数**
U8		FAT_NumberOfFats;			//FAT个数 **
U32		FAT_RootCluster;			//根目录第一簇簇号	 **
U32		FAT_TotalSectors;			//本分区扇区数 **
/*****************************************************/
//第一个FAT表的物理扇区号 = hidden_sectors + reserved_sectors
//根目录物理起始扇区号= hidden_sectors + reserved_sectors + number_of_fats * sectors_per_fat
//第I簇的物理起始扇区号= hidden_sectors + reserved_sectors + number_of_fats * sectors_per_fat + (i - root_cluster_number) * sectors_per_cluster
/*****************************************************/




//FAT32文件信息结构体
typedef struct
{
	U8			file_name[11];		//文件名(短文件名 8+3=11)
	U8			file_attr;			//文件属性值
	U32			file_size;			//文件大小 字节数

	U32			dir_entry_sector;	//文件的目录项所在的物理扇区号
	U8			dir_entry_offset;	//相对于目录项扇区的偏移 (把扇区看到目录项数组时 目录项对应的元素序号)

	U32			start_cluster;		//文件起始簇号
	U32			current_sector;		//当前正在操作的扇区号
	U32			current_offset;		//文件当前指针偏移量
}FAT_FILE;
			   

/******************************************************/
//功  能:比较字符串
//参  数:str1为第一个字串,str2为第二个字串
//返回值:str1==str2时返回0  str1<str2时返回1  str1>str2时返回2
/******************************************************/
U8 fat_strcmp( U8 *str1 , U8 *str2 , U8 len )
{
	while(len--)
	{
		if(*str1<*str2)
			return 1;
		else if(*str1>*str2)
			return 2;
		str1++;
		str2++;
	}
	return 0;	
}


/******************************************************/
//功  能:将str2复制到str1数组中
//参  数:str1为第一个字串,str2为第二个字串
//返回值:复制的字节数
/******************************************************/
void fat_strcpy( U8 *str1 , U8 *str2 , U8 len)
{
	U8 i=0;
	for(i=0;i<len;i++)
		str1[i]=str2[i];	
}


//小端转大端模式 （keil使用大端模式）
void fat_l2be(U8* buf,U8 len)
{
	U8 tmp,i,j=len/2;
	for(i=0;i<j;i++)
	{
		tmp=buf[i];
		buf[i]=buf[len-i-1];
		buf[len-i-1]=tmp;	
	}
}


//目录项类型
#define FAT_DIR_ENTRY_NUL			0 //未分配项
#define FAT_DIR_ENTRY_FILE			1 //是文件
#define FAT_DIR_ENTRY_DIR			2 //是目录
#define FAT_DIR_ENTRY_LONG_FILE		3 //是长文件项
#define FAT_DIR_ENTRY_VOL			4 //是卷标
#define FAT_DIR_ENTRY_DEL			5 //是已删除项
/******************************************************/
//功  能:目录项类型测试
//参  数:fct为目录项指针
//返回值:已定义的目录项类型
/******************************************************/
U8 fat_dir_entry_type(FAT_DIR_ENTRY* fct)
{
	if(fct->file_name[0]==0)
		return FAT_DIR_ENTRY_NUL;
	if(fct->file_name[0]==0xe5)
		return FAT_DIR_ENTRY_DEL;
	if(fct->file_attr==0x0f)
		return FAT_DIR_ENTRY_LONG_FILE;
	if(fct->file_attr & 0x08)
		return FAT_DIR_ENTRY_VOL;
	if(fct->file_attr & 0x10)
		return FAT_DIR_ENTRY_DIR;
	return FAT_DIR_ENTRY_FILE;
}


/********************************************************************************/
//功  能:测试目录项是否具有长文件名项
//参  数:fct为目录项指针
//返回值:若有返回1 没有返回0
//备  注:待验证 不使用缓冲区
/********************************************************************************/
U8 fat_has_long_file_name(FAT_DIR_ENTRY *fct)
{
	U8 i;
	for(i=0;i<8;i++)
		if(fct->file_name[i]&0x80 || fct->file_name[i]=='~')
			return 1;
	return 0;
}

/********************************************************************************/
//功  能:初始化FAT32文件系统
//参  数: partition_id(1-4)为挂载在磁盘的第几个分区 id=0时表示挂载在第一个可用的分区上
//返回值:正确返回0
//备  注:会使用缓冲区
/********************************************************************************/
U8	fat_init( U8 partition_id )
{
	FAT_DPT* dpt=(FAT_DBR*)FAT_BUF;
	FAT_DBR* dbr=(FAT_DBR*)FAT_BUF;
	U8 i;

	if(partition_id>4) //超界
		return 1;

	if(FAT_READ_SECTOR(0,FAT_BUF))	//读0扇区错误
		return 1;

	//下面是把FAT32分区的第一个扇区(DBR)载入缓冲区
	if(FAT_BUF[0]!=0xeb || FAT_BUF[1]!=0x58 || FAT_BUF[2]!=0x90) //是MBR吗 (FAT32的DBR前三个字节:EB 58 90) 
	{	//是MBR
		dpt=(FAT_DPT*)(FAT_BUF+446);//寻找DPT起始位置(偏移446)
		if(partition_id) //指定挂载某个分区
		{
			if(dpt[partition_id-1].stat==0x80 &&( dpt[partition_id-1].type==0x0b || dpt[partition_id-1].type==0x0c || dpt[partition_id-1].type==0x0f) )	
				i=partition_id-1;
			else	 //没有该分区
				return 1;			  

		}else{//找到第一个可用FAT32分区
			for(i=0;i<4;i++)
				if(dpt[i].stat==0x80 &&(dpt[i].type==0x0b || dpt[i].type==0x0c || dpt[i].type==0x0f))  //是FAT32分区
					break;
			if(i==4)  //未找到
				return 1;
		}  	 
		fat_l2be((U8*)&(dpt[i].start_sector),4);   //大端转小端
		if(FAT_READ_SECTOR(dpt[i].start_sector,FAT_BUF))//读FAT32分区第一个扇区
			return 1;
	}else if(partition_id>1)	 //只有一个FAT32分区 但要求挂载分区号超出1
		return 1;


	//文件系统错误
	if(dbr->jmp[0]!=0xeb || fat_strcmp(dbr->system_id,"FAT32",5))
		return 1;

	//验证通过并转换赋值
	FAT_Type=32;

	fat_l2be((U8*)&(dbr->bytes_per_sector),2);
	FAT_BytesPerSector=dbr->bytes_per_sector;

	FAT_SectorsPerCluster=dbr->sectors_per_cluster;

	fat_l2be((U8*)&(dbr->hidden_sectors),4);
	FAT_HiddenSectors=dbr->hidden_sectors;

	fat_l2be((U8*)&(dbr->reserved_sectors),2);
	FAT_ReservedSectors=dbr->reserved_sectors;

	fat_l2be((U8*)&(dbr->sectors_per_fat32),4);
	FAT_SectorsPerFat=dbr->sectors_per_fat32;

	FAT_NumberOfFats=dbr->number_of_fats;

	fat_l2be((U8*)&(dbr->root_cluster_number),4);
	FAT_RootCluster=dbr->root_cluster_number;

	fat_l2be((U8*)&(dbr->large_sectors),4);
	FAT_TotalSectors=dbr->large_sectors;

	return 0;
}





/********************************************************************************/
//功  能:通过FAT表沿链寻找从start_cluster号簇开始的第i个簇的簇号
//参  数:start_cluster为起始簇号 i为要寻找的第i个簇序号
//返回值:正确返回目标簇的簇号 未找到返回0(0号簇在FAT32未用)
//备  注:
/********************************************************************************/
U32 fat_find_cluster( U32 start_cluster , U32 i )
{
	U32 *fat=FAT_BUF;
	U32 fat_sec;
	U8 count;

	if(i==0)
		return start_cluster;

	if(start_cluster >= FAT_BytesPerSector / sizeof(U32) * FAT_SectorsPerFat )	//越界
		return 0;

	fat_sec=FAT_HiddenSectors + FAT_ReservedSectors;//计算FAT表物理起始扇区号
	for(count=100;count && FAT_READ_SECTOR(fat_sec + start_cluster/(FAT_BytesPerSector/sizeof(U32)),FAT_BUF);count--)	//计算簇记录项所在的物理扇区号并读取
	if(count==0)
		return 0;

	while(i--)
	{
		if(fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0x00000000	//未分配
		|| fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0xffffff0f	//该簇是结束簇
		|| fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0xf7ffffff	//该簇是坏簇
		)
			return 0;

		fat_l2be((U8*)&fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))],4);  //转换

		if(i>0 && start_cluster/(FAT_BytesPerSector/sizeof(U32))
			!= fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]/(FAT_BytesPerSector/sizeof(U32))
		)//该簇号项与目标簇号不在同一个物理扇区
		{
			start_cluster=fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))];//前进一个簇		
			for(count=100;count && FAT_READ_SECTOR(fat_sec + start_cluster/(FAT_BytesPerSector/sizeof(U32)),FAT_BUF);count--)//读取目标簇号所在的扇区
			if(count==0)
				return 0;
		}
		else
		{
			start_cluster=fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))];//前进一个簇		
		}
	}	
	return start_cluster;
}



/********************************************************************************/
//功  能:通过FAT表沿链寻找从start_sector号扇区开始的第i个扇区的扇区号
//参  数:start_sector为起始扇区号 i为要寻找的第i个扇区序号
//返回值:正确返回目标扇区的扇区号 未找到返回0(0号扇区在FAT32中不允许数据使用)
//备  注:
/********************************************************************************/
U32 fat_find_sector( U32 start_sector , U32 i )
{
	U32 cluster;
	if(i==0)
		return start_sector;

	if(start_sector < FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat || start_sector >= FAT_HiddenSectors + FAT_TotalSectors )//超界
		return 0;

	if((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) % FAT_SectorsPerCluster + i < FAT_SectorsPerCluster)
	{	//	起始扇区与目标扇区在同一个簇上
		return start_sector + i;
	}

	//不在同一个簇上
	cluster=fat_find_cluster((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster ,
	 ((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) % FAT_SectorsPerCluster + i)/FAT_SectorsPerCluster );	//找第i个扇区所在簇的簇号
	if(cluster==0)
		return 0;

	//计算目标扇区号
	return FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster + (start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat + i) % FAT_SectorsPerCluster;
}



/********************************************************************************/
//功  能:目录项的短文件名匹配(11个字节比较)
//参  数:pat为匹配字符串 name为待匹配的文件名(8+3短文件名)
//返回值:当name匹配pat时返回1 否则返回0
//备  注:pat可使用'?'忽略字节的比较
/********************************************************************************/
U8 fat_file_name_match(U8 *pat,U8 *name)
{
	U8 i;
	for(i=0;i<11;i++)
		if(pat[i]!='?')
			if(pat[i]!=name[i])
				return 0;
	return 1;
}





/********************************************************************************/
//功  能:分配一个新簇串
//参  数:numOfClusters为新分配的簇数 此值不得超过FAT_BytesPerSector/sizeof(U32)
//返回值:正确返回新簇链的首簇号 否则返回0
//备  注:使用缓冲区
/********************************************************************************/
U32 fat_new_clusters(U16 numOfClusters)
{
	U16 i,j;
	U32 sector=FAT_HiddenSectors + FAT_ReservedSectors;
	U32 *fat=FAT_BUF;
	U32 cluster;

	if(numOfClusters > FAT_BytesPerSector/sizeof(U32) || numOfClusters == 0) //参数是否合法
		return 0;

	cluster = 0;//记录簇号
	while(1)
	{
		for(i=100;i && FAT_READ_SECTOR(sector, FAT_BUF);i--);
		if(i==0)
			return 0;
		for(i=0;i < FAT_BytesPerSector/sizeof(U32) - numOfClusters + 1;i++)
		{
			if(fat[i]==0x00000000)
			{//测试从该簇开始的numOfClusters个簇是否可用
				for(j=i+1;j<i+numOfClusters;j++)
					if(fat[j]!=0x00000000) 
					{//中间有簇不可分配
						break;	
					}				

				if(j == i+numOfClusters)
				{//有连续的簇串可分配
					for(j=i;j<i+numOfClusters-1;j++)
					{//将后面的簇链到当前簇上 直到倒数第二个簇
						fat[j] = cluster + j + 1;
						fat_l2be((U8*)&fat[j],sizeof(U32));
					}
					fat[j] = 0xffffff0f; //结束簇标志
					cluster+=i;
					for(i=100;i && FAT_WRITE_SECTOR(sector,FAT_BUF);i--);//写回去
					if(i==0)
						return 0;
					return cluster;//新簇头号						
				}else{
					i = j + 1;//从不可分配的簇号后一个开始测试
				}
			}			
		}
		sector++;//该扇区号是FAT表的扇区号 由于FAT表是按顺序存放的 所以直接加1
		if(sector >= FAT_HiddenSectors + FAT_ReservedSectors +FAT_SectorsPerFat)
			return 0;
		cluster+=FAT_BytesPerSector/sizeof(U32);		
	}	
}


/********************************************************************************/
//功  能:清除一条簇链
//参  数:cluster_head是簇链的簇头号
//返回值:正确返回0
//备  注:使用缓冲区
/********************************************************************************/
U8 fat_clear_cluster(U32 cluster_head )
{
	U8 i;
	U32 *fat=FAT_BUF;
	U32 cluster;
	for(i=100;i && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//读取头簇所在的扇区
	if(i==0)
		return 1;
	while(1)
	{

		cluster=fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))];
		switch(cluster)
		{
			case 0xffffff0f://这是最后一个簇了
				fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))]=0x00000000;
				for(i=100;i && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//回写
				if(i==0)
					return 1;
				return 0;
			case 0x00000000://发生了错误 指向了一个未分配簇
			case 0xf7ffffff://心生了错误 指向了一个坏簇
				return 1;
			default://簇链还未完
				fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))]=0x00000000;  //仅在内存中修改 当下一簇的簇项与当前簇项不在同一扇区时 才回写
				fat_l2be((U8*)&cluster,4);
				if(cluster_head/(FAT_BytesPerSector/sizeof(U32)) != cluster/(FAT_BytesPerSector/sizeof(U32)))
				{	//目标簇不在该扇区上 重新载入扇区
					for(i=100;i && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//回写
					if(i==0)
						return 1;
					for(i=100;i && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);
					if(i==0)
						return 1;					
				}
				cluster_head=cluster;				
				break;
		}		
	}	
}




/********************************************************************************/
//功  能:打开/新建 根目录下的文件
//参  数:file_name文件名 
//		flag==0表示没有使用通配符,当找不到文件时新建
//		flag>0表示使用了通配符且打开可匹配的第flag个文件
//		当正确打开文件时将文件信息装入file区域中
//返回值:正确返回0
//备  注:文件名必须符合短文件名标准(8+3格式) 可使用'?'进行匹配任何字符
//			"ABC     TXT"表示根目录下的"ABC.TXT"文件
//			"????????TXT"表示扩展名为TXT的文件
//			"???????????"表示任何文件名
//			"A?B?????MP3"表示A?B.MP3的文件名
/********************************************************************************/
U8	fat_open_file( U8 *file_name , U16 flag , FAT_FILE *file )
{
	U32 sector,cluster,tmp;
	U16 NthFile=flag;
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U32 *fat=FAT_BUF;
	U8 i;
	U16 count;

	sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat;//计算根目录的物理扇区号
	for(count=100;count && FAT_READ_SECTOR(sector,FAT_BUF);count--);//载入扇区
	if(count==0)
		return 1;
	
	//路径目录已载入缓冲
	while(1)
	{
		for(i=0;i<FAT_BytesPerSector/sizeof(FAT_DIR_ENTRY);i++)
		{
			switch(fat_dir_entry_type(fct+i))
			{
				case FAT_DIR_ENTRY_NUL://是此新建
					if(flag>0)	 //使用通配符时未找到
						return 1;
					//下面由于没使用通配符 所以新建
					cluster=fat_new_clusters(1);	//申请新簇
					if(cluster==0)	 //申请失败
						return 1;
					for(count=100;count && FAT_READ_SECTOR(sector,FAT_BUF);count--);//由于使用fat_new_cluster,所以需重新载入扇区
					if(count==0)
						return 1;
					fat_strcpy(fct[i].file_name , file_name ,11);	//填入文件名
					fct[i].file_attr=0x20;						//填入属性(0x20代表文件)
					fct[i].reserved=0;
					fct[i].start_cluster_h16=cluster>>16;		//填入起始簇高字
					fat_l2be((U8*)&fct[i].start_cluster_h16,2);
					fct[i].start_cluster_l16=cluster&0x0000ffff;			//填入起始簇低字
					fat_l2be((U8*)&fct[i].start_cluster_l16,2);
					fct[i].file_size=0;	  //填入文件大小

					for(count=100;count && FAT_WRITE_SECTOR(sector,FAT_BUF);count--);//回写
					if(count==0)
						return 1;
					//新建后打开
					fat_strcpy(file->file_name , fct[i].file_name , 11);
					file->file_attr=fct[i].file_attr;
					file->file_size=0;
					file->dir_entry_sector=sector;
					file->dir_entry_offset=i;
					file->start_cluster=cluster;
					file->current_sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster;
					file->current_offset=0;
					return 0;			
				case FAT_DIR_ENTRY_FILE://在此测试是否同名
					if(fat_file_name_match(file_name,fct[i].file_name))
					{
						if(flag==0 || flag>0 && NthFile==1)
						{//打开文件
							fat_strcpy(file->file_name,fct[i].file_name,11);  //拷贝文件名
							file->file_attr=fct[i].file_attr;		  //拷贝属性
							file->file_size=fct[i].file_size;		    //拷贝文件大小
							fat_l2be((U8*)&(file->file_size),4);
							file->dir_entry_sector=sector;				 //拷贝目录项所在扇区号
							file->dir_entry_offset=i;					   //拷贝目录项偏移
							file->start_cluster=fct[i].start_cluster_l16;	  //拷贝起始簇号
							file->start_cluster<<=16;
							file->start_cluster|=fct[i].start_cluster_h16;
							fat_l2be((U8*)&(file->start_cluster),4);
							file->current_sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster;	 //计算起始扇区号
							file->current_offset=0;		//初始偏移为0(即文件头)
							return 0;								
						}else{
							if(flag>0)
								NthFile--;//还没有到那个文件
						}
					}
					break;
				default:
					break;
			}
		}
		tmp=fat_find_sector(sector,1);//寻找根目录的下一个扇区号
		if(tmp==0)
		{//根目录已完 找不到该文件 若要新建需要为根目录扩大空间
			if(flag>0)
			{//不新建 返回找不到
				return 1;
			}
			//新建文件 为根目录扩大空间
			cluster=fat_new_clusters(1);
			if(cluster==0)		//未申请成功
				return 1;
			tmp=(sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster;//计算当前所在的簇号
			for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//载入FAT表
			if(count==0)
				return 1;
			fat[tmp % (FAT_BytesPerSector / sizeof(U32))] = cluster;	//连接新簇
			fat_l2be((U8*)&fat[tmp % (FAT_BytesPerSector / sizeof(U32))] , 4);
			for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + tmp / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//写回FAT表
			if(count==0)
				return 1;
			sector = FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster;  //将新簇的第一个扇区作为当前扇区
			//下面清空新簇的全部数据
			for(tmp=0 ; tmp<FAT_SectorsPerCluster ; tmp++)
			{
				for(count=0;count<FAT_BytesPerSector;count++)
					FAT_BUF[count]=0;
				for(count=100;count && FAT_WRITE_SECTOR(sector + tmp,FAT_BUF);count--);//清空该扇区
				if(count==0)
					return 1;
			}
		}else
			sector=tmp;
		for(count=100;count && FAT_READ_SECTOR(sector,FAT_BUF);count--);
		if(count==0)
			return 1;		
	}	
}



/********************************************************************************/
//功  能:关闭文件
//参  数:file为文件信息指针
//返回值:正确返回0
//备  注:
/********************************************************************************/
U8	fat_close_file( FAT_FILE *file )
{
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U8 count;

	for(count=100;count && FAT_READ_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//载入扇区
	if(count==0)
		return 1; 

	//更新文件信息	
	fat_strcpy(fct[file->dir_entry_offset].file_name , file->file_name , 11);
	fct[file->dir_entry_offset].file_attr=file->file_attr;
	fct[file->dir_entry_offset].start_cluster_h16=file->start_cluster>>16;
	fct[file->dir_entry_offset].start_cluster_l16=file->start_cluster & 0x0000ffff;
	fat_l2be((U8*)&(fct[file->dir_entry_offset].start_cluster_h16),2);
	fat_l2be((U8*)&(fct[file->dir_entry_offset].start_cluster_l16),2);
	fct[file->dir_entry_offset].file_size=file->file_size;
	fat_l2be((U8*)&(fct[file->dir_entry_offset].file_size),4);

	for(count=100;count && FAT_WRITE_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//回写到磁盘
	if(count==0)
		return 1;

	return 0;
}


/********************************************************************************/
//功  能:删除文件
//参  数:file为文件信息指针
//返回值:正确返回0
//备  注:
/********************************************************************************/
U8	fat_delete_file( FAT_FILE *file )
{
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U8 count;

	for(count=100;count && FAT_READ_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//读目录项扇区
	if(count==0)
		return 1;

	file->file_name[0]=0xe5;	
	fct[file->dir_entry_offset].file_name[0]=0xe5;//删除标志
	
	for(count=100;count && FAT_WRITE_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//写入磁盘
	if(count==0)
		return 1;
		
	return fat_clear_cluster(file->start_cluster);//清空簇空间		
}


/********************************************************************************/
//功  能:设置文件的当前偏移
//参  数:offset为偏移的字节数 ,file为文件信息指针
//返回值:正确返回0
//备  注:只能偏移到扇区的边界,当offset超过文件大小时 超出的部分用0填充并修改文件大小
/********************************************************************************/
U8	fat_file_set_offset( U32 offset , FAT_FILE *file )
{
	U32 sector;
	offset=offset/FAT_BytesPerSector*FAT_BytesPerSector;//偏移到扇区边界
	sector=fat_find_sector(FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster , offset/FAT_BytesPerSector);
	if(sector==0)
	{//未找到
		return 1;
	}
	file->current_sector=sector;
	file->current_offset=offset;
	return 0;
}



/********************************************************************************/
//功  能:从文件的当前位置读取一个扇区的数据送到缓冲区
//参  数:file为文件信息指针
//返回值:返回0表示 正确读取了一个扇区的数据 将当前偏移向后移一个扇区的位置
//		返回1表示 读磁盘时发生了错误 不移动偏移指针
//		返回2表示 当前读的数据是文件最后一个扇区 不移动偏移指针
//		返回3表示 本次读正确 但出现了异常(文件未完 但后面找不到数据了) 不移动偏移指针
//备  注:读取后当前偏移自动向后移动一个扇区的位置 
/********************************************************************************/
U8	fat_file_read( FAT_FILE *file )
{
	U32 sector;
	U8	count;

	sector=fat_find_sector(file->current_sector , 1);//寻找下一个扇区

	for(count=100;count && FAT_READ_SECTOR(file->current_sector,FAT_BUF);count--);//载入扇区
	if(count==0)
		return 1;
	
	if(file->current_offset + FAT_BytesPerSector >= file->file_size)
		return 2;	   //偏移后超过了文件大小(即到达了文件最后一个扇区)

//未达到文件尾
	if(sector)
	{  //正常 后面可以找到数据
		file->current_offset += FAT_BytesPerSector;
		file->current_sector = sector;
		return 0;
	}
	else //不正常 后面找不到数据了
		return 3;		
}




/********************************************************************************/
//功  能:把缓冲区的数据从写入到文件的当前偏移位置
//参  数:file为文件信息指针
//返回值:返回0表示 正确写入了一个扇区的数据 将当前偏移向后移一个扇区的位置 
//			当偏移超过文件大小时 自动新申请一个簇的大小 并修改文件大小到当前偏移
//		返回1表示 在写入数据时发生了错误 不移动偏移指针
//备  注:写入后当前偏移自动向后移动一个扇区的位置
/********************************************************************************/
U8	fat_file_write( FAT_FILE *file )
{
	U8 count;
	U32 cluster;
	U32 sector;
	U32 *fat=FAT_BUF;

	for(count=100;count && FAT_WRITE_SECTOR(file->current_sector,FAT_BUF);count--);//写入磁盘
	if(count==0)
		return 1;
		
	sector=fat_find_sector(file->current_sector , 1);//寻找下一个扇区
	if(sector)
	{
		file->current_sector=sector;	//前进一个扇区	
	}else{	//到达最后一个簇的最后一个扇区　需要申请一个簇
		cluster=fat_new_clusters(1);
		if(cluster==0)		//未申请成功
			return 1;
		sector=(file->current_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster;//计算当前所在的簇号
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//载入FAT表
		if(count==0)
			return 1;
		fat[sector % (FAT_BytesPerSector / sizeof(U32))] = cluster;	//连接新簇
		fat_l2be((U8*)&fat[sector % (FAT_BytesPerSector / sizeof(U32))] , 4);
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//写回FAT表
		if(count==0)
			return 1;
		file->current_sector = FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster;  //将新簇的第一个扇区作为当前扇区
	}
	file->current_offset += FAT_BytesPerSector;	   //前移一个扇区的偏移
	if(file->current_offset > file->file_size)		  //超过文件总大小时修改文件大小
		file->file_size=file->current_offset;	
	return 0;
}


/********************************************************************************/
//功  能:重置文件的大小
//参  数:file_size为把重置后文件的大小,file为文件信息指针
//返回值:正确返回0
//备  注:当文件当前大小低于file_size,则扩充文件大小;当文件当前大小高于file_size,则从文件尾部删剪文件大小
/********************************************************************************/
U8 fat_file_resize(U32 file_size , FAT_FILE *file)
{
	U32 cluster1;
	U32 cluster2;
	U32 *fat=FAT_BUF;
	U32 tmp,new_clusters,fore_cluster;
	U8 count;


	//下八行计算当前文件所占的簇数
	if(file->file_size==0)
		cluster1=1;	 //文件至少占一个簇 即使大小为0
	else
	{
		cluster1 = file->file_size / FAT_SectorsPerCluster / FAT_BytesPerSector; 
		if(file->file_size % (FAT_SectorsPerCluster * FAT_BytesPerSector))
			cluster1++;
	}

	//下八行计算重置后文件所占的簇数
	if(file_size==0)
		cluster2=1;	 //文件至少占一个簇 即使大小为0
	else
	{
		cluster2 = file_size / FAT_SectorsPerCluster / FAT_BytesPerSector; 
		if(file_size % (FAT_SectorsPerCluster * FAT_BytesPerSector))
			cluster2++;
	}

	if(cluster1 > cluster2)
	{//需删剪
		cluster1 = fat_find_cluster(file->start_cluster , cluster2 - 1);//计算重置后的尾簇号
		if(cluster1 == 0)
			return 1;//出错 未找到簇
		cluster2 = fat_find_cluster(cluster1 , 1);//计算重置后的尾簇下一簇号 即需清空的首簇号
		if(cluster2 == 0)
			return 1;//出错 未找到簇
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//读取尾簇所在的扇区
		if(count==0) //读扇区出错
			return 1;
		fat[cluster1%(FAT_BytesPerSector / sizeof(U32))] = 0xffffff0f; //结束簇标志
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//回写
		if(count==0)
			return 1;
		fat_clear_cluster(cluster2);//清空尾簇链				
	}else if(cluster1 < cluster2)
	{//需扩充
		tmp = cluster2 - cluster1;//计算需要申请多少个簇
		cluster2=0;//用来记录新簇链的首簇号
		while(tmp>0)
		{//申请一个簇链
			new_clusters = (tmp>FAT_BytesPerSector/sizeof(U32))?FAT_BytesPerSector/sizeof(U32):tmp;//如果需申请的簇数超过了最大允许申请簇数则申请最大允许簇数 否则申请tmp个
			tmp -= new_clusters;//计算剩余需申请的簇数
			new_clusters = fat_new_clusters(new_clusters);//申请一个簇链
			if(new_clusters==0)
				return 1;//申请失败	  
			if(cluster2==0)
				cluster2=new_clusters;//这是新簇链的首簇号
			else{//连接到上一个簇链的尾部
				for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + fore_cluster/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//读取上个簇链的尾簇所在的扇区
				if(count==0) //读扇区出错
					return 1;
				fat[fore_cluster%(FAT_BytesPerSector / sizeof(U32))] = new_clusters; //连接簇
				fat_l2be((U8*)&fat[fore_cluster%(FAT_BytesPerSector / sizeof(U32))],sizeof(U32));//转换
				for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + fore_cluster/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//回写
				if(count==0)
					return 1;								
			}
			fore_cluster = new_clusters + FAT_BytesPerSector/sizeof(U32) - 1;//记录本次申请的簇链的最后一个簇的簇号(因为是连续的簇链 所以直接加上)
		}

		cluster1 = fat_find_cluster(file->start_cluster , cluster1 - 1);//计算文件当前的尾簇号
		if(cluster1 == 0)
			return 1;//出错 未找到簇
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//读取文件的尾簇所在的扇区
		if(count==0) //读扇区出错
			return 1;
		fat[cluster1%(FAT_BytesPerSector / sizeof(U32))] = cluster2; //连接簇
		fat_l2be((U8*)&fat[cluster1%(FAT_BytesPerSector / sizeof(U32))],sizeof(U32));//转换
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//回写
		if(count==0)
			return 1;					
	}
	//文件簇数一样
	file->file_size = file_size;
	return 0;
}
 
#endif
