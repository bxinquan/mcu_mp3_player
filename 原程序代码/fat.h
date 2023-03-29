

/**********************************************************/
//FAT32�ļ�ϵͳ����ͷ�ļ�
//��Ҫ����51��Ƭ��
//���ڸ�Ŀ¼�½���"�½�/��/�ر�/ɾ��"�ļ�
//����Ȩ
//2012.3.20
/**********************************************************/




#ifndef __fat_h__ 
#define __fat_h__ 

#include"sd.h"



//���������ض���
#define U8		unsigned char
#define U16		unsigned short
#define U32		unsigned long


/******************����********************/
#define	FAT_SECTOR_SIZE			512					//������̵�������С
#define FAT_READ_SECTOR			sd_read_sector	  	//���̵Ķ���������(������ʽU8 read_sector(U32 sector,U8 *buf);)
#define	FAT_WRITE_SECTOR		sd_write_sector		//���̵�д��������(������ʽU8 write_sector(U32 sector,U8 *buf);)
/******************************************/




/*******************************************/
//���û�����(��С���ڴ���������С)
//�˻�����ֻ�ڱ�ͷ�ļ��еĲ��ֺ���ִ��ʱʹ��
//����ִ����� �û���ʹ��
U8		FAT_BUF[FAT_SECTOR_SIZE];
/*******************************************/





//��������¼�Ĵ��̷�����DPTÿ������ݽṹ(16�ֽ�)
//λ��MBRƫ��446�ֽ�
/*
�����Ĳ���ϵͳ����(�ļ���ʽ��־��)
00H--δ֪���ͻ���
01H--12λFAT
04H---DOS FAT1632M
0BH--FAT32�������2G
0CH--FAT32,����LBAģʽ������int 13h��չ�ж�
0FH--��չFAT32����������LBAģʽ������int 13h��չ�ж�
07H---NTFS(OS/2)
82H--linux
83H---LINUX swap>64M
*/

typedef struct
{
	unsigned char stat;//����״̬ 0x00--�ǻ�� 0x80--���
	unsigned char start_CHS[3];//������ʼCHS��ַ
	unsigned char type;//�������� 0x0b,0x0c,0x0fΪFAT32ϵͳ
	unsigned char end_CHS[3];//��������CHS��ַ
	unsigned long start_sector;//��������ʼLBA��ַ������ʼ���� ����С��ģʽ��
	unsigned long size;//������С������(����С��ģʽ)
}FAT_DPT;




/*
����DBR�ı�־��

FAT16��DBR:EB 3C 90��

FAT32��DBR:EB 58 90 

NTFS��DBR:   EB 52 90
*/
/*****************************************************/
//FAT32�ļ�ϵͳDBR�����ݽṹ(512�ֽ�)
/*****************************************************/
typedef struct
{
	U8		jmp[3];							//JMP����
	U8		oem[8];							//OEM
//���µ�53�ֽ�ΪBPB��Ϣ
	U16		bytes_per_sector;				//ÿ�����ֽ��� **
	U8		sectors_per_cluster;				//ÿ�������� **
	U16		reserved_sectors;				//���������� ��һ��FAT���������׵������� ��FAT��1����ڱ�������ʼλ�õ�ƫ��������  **
	U8		number_of_fats;					//FAT��ĸ��� **
	U16		root_entries;					//��Ŀ¼���� ֻ��FAT12/FAT16ʹ�ô��ֶ� FAT32��0
	U16		small_sectors;					//��������������������С��32MBʱʹ��,����ʱ�˴���0,ʹ��large_sectors��
	U8		media_descriptor;				//������������F8��ʾӲ�� F0��ʾ3.5������
	U16		sectors_per_fat16;				//FAT����ռ��������FAT16/FAT12��ʹ�ã�FAT32��ʹ�ô��ֶ�(��ʹ��sectors_per_fat32)
	U16		sectors_per_track;				//ÿ�ŵ�������
	U16		number_of_head;					//��ͷ��
	U32		hidden_sectors;					//��������������0��DBR��������	**
	U32		large_sectors;					//�������������� **
	U32		sectors_per_fat32;				//ÿ��FAT32��Ĵ�С������ ����FAT32ʹ��**
	U8		extended_flag[2];				//��չ��� ֻ��FAT32ʹ��
	U8		file_system_version[2];			//�ļ�ϵͳ�汾��ֻ��FAT32ʹ��
	U32		root_cluster_number;			//��Ŀ¼�غ�	ֻ��FAT32ʹ�� **
	U16		fsinfo_sector_number;			//�ļ�ϵͳ��Ϣ�����š�ֻ��FAT32ʹ��
	U16		backup_boot_sector_number;		//�������������� ֻ��FAT32ʹ��
	U8		reserved[12];					//���� ֻ��FAT32ʹ��
//���µ�26�ֽ�Ϊ��չ��BPB��Ϣ
	U8		physical_drive_number;			//BIOS�豸
	U8		reserved2;						//δʹ��
	U8		expend_boot_flag;				//��չ������־��28H,29H��
	U32		volume_serial_number;			//�����к� һ�����ֵ �������ִ���
	U8		volume_label[11];				//���
	U8		system_id[8];					//�ļ�ϵͳ��ʽ һ��Ϊ"FAT32" **
//����Ϊ����ϵͳ��������
}FAT_DBR;


/*****************************************************/
//FAT32Ŀ¼�����ݽṹ
//FAT32���ļ�Ŀ¼��32���ֽڵı�ʾ����
/*****************************************************/
typedef struct
{
	U8		file_name[8];					//�ļ��� (name[0]=0x00ʱ,����δʹ�� name[0]=0xe5��ɾ����)
	U8		extend_name[3];					//��չ��
	U8		file_attr;							//�����ֽ� (0x0f����Ϊ���ļ�����)
	U8		reserved;						//�����ֽ�
	U8		create_milli_sec;				//����ʱ���10����λ
	U8		create_time[2];					//�ļ�����ʱ��
	U8		create_date[2];					//�ļ���������
	U8		access_date[2];					//�ļ�����������
	U16		start_cluster_h16;				//�ļ���ʼ�غŵĸ�16λ
	U8		alter_time[2];					//�ļ�������޸�ʱ��
	U8		alter_date[2];					//�ļ�������޸�����
	U16		start_cluster_l16;				//�ļ���ʼ�غŵĵ�16λ
	U32		file_size;						//�ļ��ĳ���(���ֽ�Ϊ��λ) ��Ŀ¼����Ϊ0
}FAT_DIR_ENTRY;



/*****************************************************/
//FAT32�ļ�ϵͳ������Ϣ
/*****************************************************/
U8		FAT_Type;					//�ļ�ϵͳ���� 16-FAT16 32-FAT32
U16		FAT_BytesPerSector;			//ÿ�����ֽ��� **
U8		FAT_SectorsPerCluster;		//ÿ�������� **
U32		FAT_HiddenSectors;			//��������������0��DBR��������	**
U16		FAT_ReservedSectors;		//����������  **
U32		FAT_SectorsPerFat;			//ÿ��FAT��Ĵ�С������**
U8		FAT_NumberOfFats;			//FAT���� **
U32		FAT_RootCluster;			//��Ŀ¼��һ�شغ�	 **
U32		FAT_TotalSectors;			//������������ **
/*****************************************************/
//��һ��FAT������������� = hidden_sectors + reserved_sectors
//��Ŀ¼������ʼ������= hidden_sectors + reserved_sectors + number_of_fats * sectors_per_fat
//��I�ص�������ʼ������= hidden_sectors + reserved_sectors + number_of_fats * sectors_per_fat + (i - root_cluster_number) * sectors_per_cluster
/*****************************************************/




//FAT32�ļ���Ϣ�ṹ��
typedef struct
{
	U8			file_name[11];		//�ļ���(���ļ��� 8+3=11)
	U8			file_attr;			//�ļ�����ֵ
	U32			file_size;			//�ļ���С �ֽ���

	U32			dir_entry_sector;	//�ļ���Ŀ¼�����ڵ�����������
	U8			dir_entry_offset;	//�����Ŀ¼��������ƫ�� (����������Ŀ¼������ʱ Ŀ¼���Ӧ��Ԫ�����)

	U32			start_cluster;		//�ļ���ʼ�غ�
	U32			current_sector;		//��ǰ���ڲ�����������
	U32			current_offset;		//�ļ���ǰָ��ƫ����
}FAT_FILE;
			   

/******************************************************/
//��  ��:�Ƚ��ַ���
//��  ��:str1Ϊ��һ���ִ�,str2Ϊ�ڶ����ִ�
//����ֵ:str1==str2ʱ����0  str1<str2ʱ����1  str1>str2ʱ����2
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
//��  ��:��str2���Ƶ�str1������
//��  ��:str1Ϊ��һ���ִ�,str2Ϊ�ڶ����ִ�
//����ֵ:���Ƶ��ֽ���
/******************************************************/
void fat_strcpy( U8 *str1 , U8 *str2 , U8 len)
{
	U8 i=0;
	for(i=0;i<len;i++)
		str1[i]=str2[i];	
}


//С��ת���ģʽ ��keilʹ�ô��ģʽ��
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


//Ŀ¼������
#define FAT_DIR_ENTRY_NUL			0 //δ������
#define FAT_DIR_ENTRY_FILE			1 //���ļ�
#define FAT_DIR_ENTRY_DIR			2 //��Ŀ¼
#define FAT_DIR_ENTRY_LONG_FILE		3 //�ǳ��ļ���
#define FAT_DIR_ENTRY_VOL			4 //�Ǿ��
#define FAT_DIR_ENTRY_DEL			5 //����ɾ����
/******************************************************/
//��  ��:Ŀ¼�����Ͳ���
//��  ��:fctΪĿ¼��ָ��
//����ֵ:�Ѷ����Ŀ¼������
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
//��  ��:����Ŀ¼���Ƿ���г��ļ�����
//��  ��:fctΪĿ¼��ָ��
//����ֵ:���з���1 û�з���0
//��  ע:����֤ ��ʹ�û�����
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
//��  ��:��ʼ��FAT32�ļ�ϵͳ
//��  ��: partition_id(1-4)Ϊ�����ڴ��̵ĵڼ������� id=0ʱ��ʾ�����ڵ�һ�����õķ�����
//����ֵ:��ȷ����0
//��  ע:��ʹ�û�����
/********************************************************************************/
U8	fat_init( U8 partition_id )
{
	FAT_DPT* dpt=(FAT_DBR*)FAT_BUF;
	FAT_DBR* dbr=(FAT_DBR*)FAT_BUF;
	U8 i;

	if(partition_id>4) //����
		return 1;

	if(FAT_READ_SECTOR(0,FAT_BUF))	//��0��������
		return 1;

	//�����ǰ�FAT32�����ĵ�һ������(DBR)���뻺����
	if(FAT_BUF[0]!=0xeb || FAT_BUF[1]!=0x58 || FAT_BUF[2]!=0x90) //��MBR�� (FAT32��DBRǰ�����ֽ�:EB 58 90) 
	{	//��MBR
		dpt=(FAT_DPT*)(FAT_BUF+446);//Ѱ��DPT��ʼλ��(ƫ��446)
		if(partition_id) //ָ������ĳ������
		{
			if(dpt[partition_id-1].stat==0x80 &&( dpt[partition_id-1].type==0x0b || dpt[partition_id-1].type==0x0c || dpt[partition_id-1].type==0x0f) )	
				i=partition_id-1;
			else	 //û�и÷���
				return 1;			  

		}else{//�ҵ���һ������FAT32����
			for(i=0;i<4;i++)
				if(dpt[i].stat==0x80 &&(dpt[i].type==0x0b || dpt[i].type==0x0c || dpt[i].type==0x0f))  //��FAT32����
					break;
			if(i==4)  //δ�ҵ�
				return 1;
		}  	 
		fat_l2be((U8*)&(dpt[i].start_sector),4);   //���תС��
		if(FAT_READ_SECTOR(dpt[i].start_sector,FAT_BUF))//��FAT32������һ������
			return 1;
	}else if(partition_id>1)	 //ֻ��һ��FAT32���� ��Ҫ����ط����ų���1
		return 1;


	//�ļ�ϵͳ����
	if(dbr->jmp[0]!=0xeb || fat_strcmp(dbr->system_id,"FAT32",5))
		return 1;

	//��֤ͨ����ת����ֵ
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
//��  ��:ͨ��FAT������Ѱ�Ҵ�start_cluster�Ŵؿ�ʼ�ĵ�i���صĴغ�
//��  ��:start_clusterΪ��ʼ�غ� iΪҪѰ�ҵĵ�i�������
//����ֵ:��ȷ����Ŀ��صĴغ� δ�ҵ�����0(0�Ŵ���FAT32δ��)
//��  ע:
/********************************************************************************/
U32 fat_find_cluster( U32 start_cluster , U32 i )
{
	U32 *fat=FAT_BUF;
	U32 fat_sec;
	U8 count;

	if(i==0)
		return start_cluster;

	if(start_cluster >= FAT_BytesPerSector / sizeof(U32) * FAT_SectorsPerFat )	//Խ��
		return 0;

	fat_sec=FAT_HiddenSectors + FAT_ReservedSectors;//����FAT��������ʼ������
	for(count=100;count && FAT_READ_SECTOR(fat_sec + start_cluster/(FAT_BytesPerSector/sizeof(U32)),FAT_BUF);count--)	//����ؼ�¼�����ڵ����������Ų���ȡ
	if(count==0)
		return 0;

	while(i--)
	{
		if(fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0x00000000	//δ����
		|| fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0xffffff0f	//�ô��ǽ�����
		|| fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]==0xf7ffffff	//�ô��ǻ���
		)
			return 0;

		fat_l2be((U8*)&fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))],4);  //ת��

		if(i>0 && start_cluster/(FAT_BytesPerSector/sizeof(U32))
			!= fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))]/(FAT_BytesPerSector/sizeof(U32))
		)//�ôغ�����Ŀ��غŲ���ͬһ����������
		{
			start_cluster=fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))];//ǰ��һ����		
			for(count=100;count && FAT_READ_SECTOR(fat_sec + start_cluster/(FAT_BytesPerSector/sizeof(U32)),FAT_BUF);count--)//��ȡĿ��غ����ڵ�����
			if(count==0)
				return 0;
		}
		else
		{
			start_cluster=fat[start_cluster%(FAT_BytesPerSector/sizeof(U32))];//ǰ��һ����		
		}
	}	
	return start_cluster;
}



/********************************************************************************/
//��  ��:ͨ��FAT������Ѱ�Ҵ�start_sector��������ʼ�ĵ�i��������������
//��  ��:start_sectorΪ��ʼ������ iΪҪѰ�ҵĵ�i���������
//����ֵ:��ȷ����Ŀ�������������� δ�ҵ�����0(0��������FAT32�в���������ʹ��)
//��  ע:
/********************************************************************************/
U32 fat_find_sector( U32 start_sector , U32 i )
{
	U32 cluster;
	if(i==0)
		return start_sector;

	if(start_sector < FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat || start_sector >= FAT_HiddenSectors + FAT_TotalSectors )//����
		return 0;

	if((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) % FAT_SectorsPerCluster + i < FAT_SectorsPerCluster)
	{	//	��ʼ������Ŀ��������ͬһ������
		return start_sector + i;
	}

	//����ͬһ������
	cluster=fat_find_cluster((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster ,
	 ((start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) % FAT_SectorsPerCluster + i)/FAT_SectorsPerCluster );	//�ҵ�i���������ڴصĴغ�
	if(cluster==0)
		return 0;

	//����Ŀ��������
	return FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster + (start_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat + i) % FAT_SectorsPerCluster;
}



/********************************************************************************/
//��  ��:Ŀ¼��Ķ��ļ���ƥ��(11���ֽڱȽ�)
//��  ��:patΪƥ���ַ��� nameΪ��ƥ����ļ���(8+3���ļ���)
//����ֵ:��nameƥ��patʱ����1 ���򷵻�0
//��  ע:pat��ʹ��'?'�����ֽڵıȽ�
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
//��  ��:����һ���´ش�
//��  ��:numOfClustersΪ�·���Ĵ��� ��ֵ���ó���FAT_BytesPerSector/sizeof(U32)
//����ֵ:��ȷ�����´������״غ� ���򷵻�0
//��  ע:ʹ�û�����
/********************************************************************************/
U32 fat_new_clusters(U16 numOfClusters)
{
	U16 i,j;
	U32 sector=FAT_HiddenSectors + FAT_ReservedSectors;
	U32 *fat=FAT_BUF;
	U32 cluster;

	if(numOfClusters > FAT_BytesPerSector/sizeof(U32) || numOfClusters == 0) //�����Ƿ�Ϸ�
		return 0;

	cluster = 0;//��¼�غ�
	while(1)
	{
		for(i=100;i && FAT_READ_SECTOR(sector, FAT_BUF);i--);
		if(i==0)
			return 0;
		for(i=0;i < FAT_BytesPerSector/sizeof(U32) - numOfClusters + 1;i++)
		{
			if(fat[i]==0x00000000)
			{//���ԴӸôؿ�ʼ��numOfClusters�����Ƿ����
				for(j=i+1;j<i+numOfClusters;j++)
					if(fat[j]!=0x00000000) 
					{//�м��дز��ɷ���
						break;	
					}				

				if(j == i+numOfClusters)
				{//�������Ĵش��ɷ���
					for(j=i;j<i+numOfClusters-1;j++)
					{//������Ĵ�������ǰ���� ֱ�������ڶ�����
						fat[j] = cluster + j + 1;
						fat_l2be((U8*)&fat[j],sizeof(U32));
					}
					fat[j] = 0xffffff0f; //�����ر�־
					cluster+=i;
					for(i=100;i && FAT_WRITE_SECTOR(sector,FAT_BUF);i--);//д��ȥ
					if(i==0)
						return 0;
					return cluster;//�´�ͷ��						
				}else{
					i = j + 1;//�Ӳ��ɷ���Ĵغź�һ����ʼ����
				}
			}			
		}
		sector++;//����������FAT��������� ����FAT���ǰ�˳���ŵ� ����ֱ�Ӽ�1
		if(sector >= FAT_HiddenSectors + FAT_ReservedSectors +FAT_SectorsPerFat)
			return 0;
		cluster+=FAT_BytesPerSector/sizeof(U32);		
	}	
}


/********************************************************************************/
//��  ��:���һ������
//��  ��:cluster_head�Ǵ����Ĵ�ͷ��
//����ֵ:��ȷ����0
//��  ע:ʹ�û�����
/********************************************************************************/
U8 fat_clear_cluster(U32 cluster_head )
{
	U8 i;
	U32 *fat=FAT_BUF;
	U32 cluster;
	for(i=100;i && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//��ȡͷ�����ڵ�����
	if(i==0)
		return 1;
	while(1)
	{

		cluster=fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))];
		switch(cluster)
		{
			case 0xffffff0f://�������һ������
				fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))]=0x00000000;
				for(i=100;i && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//��д
				if(i==0)
					return 1;
				return 0;
			case 0x00000000://�����˴��� ָ����һ��δ�����
			case 0xf7ffffff://�����˴��� ָ����һ������
				return 1;
			default://������δ��
				fat[cluster_head%(FAT_BytesPerSector / sizeof(U32))]=0x00000000;  //�����ڴ����޸� ����һ�صĴ����뵱ǰ�����ͬһ����ʱ �Ż�д
				fat_l2be((U8*)&cluster,4);
				if(cluster_head/(FAT_BytesPerSector/sizeof(U32)) != cluster/(FAT_BytesPerSector/sizeof(U32)))
				{	//Ŀ��ز��ڸ������� ������������
					for(i=100;i && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster_head/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);i--);//��д
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
//��  ��:��/�½� ��Ŀ¼�µ��ļ�
//��  ��:file_name�ļ��� 
//		flag==0��ʾû��ʹ��ͨ���,���Ҳ����ļ�ʱ�½�
//		flag>0��ʾʹ����ͨ����Ҵ򿪿�ƥ��ĵ�flag���ļ�
//		����ȷ���ļ�ʱ���ļ���Ϣװ��file������
//����ֵ:��ȷ����0
//��  ע:�ļ���������϶��ļ�����׼(8+3��ʽ) ��ʹ��'?'����ƥ���κ��ַ�
//			"ABC     TXT"��ʾ��Ŀ¼�µ�"ABC.TXT"�ļ�
//			"????????TXT"��ʾ��չ��ΪTXT���ļ�
//			"???????????"��ʾ�κ��ļ���
//			"A?B?????MP3"��ʾA?B.MP3���ļ���
/********************************************************************************/
U8	fat_open_file( U8 *file_name , U16 flag , FAT_FILE *file )
{
	U32 sector,cluster,tmp;
	U16 NthFile=flag;
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U32 *fat=FAT_BUF;
	U8 i;
	U16 count;

	sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat;//�����Ŀ¼������������
	for(count=100;count && FAT_READ_SECTOR(sector,FAT_BUF);count--);//��������
	if(count==0)
		return 1;
	
	//·��Ŀ¼�����뻺��
	while(1)
	{
		for(i=0;i<FAT_BytesPerSector/sizeof(FAT_DIR_ENTRY);i++)
		{
			switch(fat_dir_entry_type(fct+i))
			{
				case FAT_DIR_ENTRY_NUL://�Ǵ��½�
					if(flag>0)	 //ʹ��ͨ���ʱδ�ҵ�
						return 1;
					//��������ûʹ��ͨ��� �����½�
					cluster=fat_new_clusters(1);	//�����´�
					if(cluster==0)	 //����ʧ��
						return 1;
					for(count=100;count && FAT_READ_SECTOR(sector,FAT_BUF);count--);//����ʹ��fat_new_cluster,������������������
					if(count==0)
						return 1;
					fat_strcpy(fct[i].file_name , file_name ,11);	//�����ļ���
					fct[i].file_attr=0x20;						//��������(0x20�����ļ�)
					fct[i].reserved=0;
					fct[i].start_cluster_h16=cluster>>16;		//������ʼ�ظ���
					fat_l2be((U8*)&fct[i].start_cluster_h16,2);
					fct[i].start_cluster_l16=cluster&0x0000ffff;			//������ʼ�ص���
					fat_l2be((U8*)&fct[i].start_cluster_l16,2);
					fct[i].file_size=0;	  //�����ļ���С

					for(count=100;count && FAT_WRITE_SECTOR(sector,FAT_BUF);count--);//��д
					if(count==0)
						return 1;
					//�½����
					fat_strcpy(file->file_name , fct[i].file_name , 11);
					file->file_attr=fct[i].file_attr;
					file->file_size=0;
					file->dir_entry_sector=sector;
					file->dir_entry_offset=i;
					file->start_cluster=cluster;
					file->current_sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster;
					file->current_offset=0;
					return 0;			
				case FAT_DIR_ENTRY_FILE://�ڴ˲����Ƿ�ͬ��
					if(fat_file_name_match(file_name,fct[i].file_name))
					{
						if(flag==0 || flag>0 && NthFile==1)
						{//���ļ�
							fat_strcpy(file->file_name,fct[i].file_name,11);  //�����ļ���
							file->file_attr=fct[i].file_attr;		  //��������
							file->file_size=fct[i].file_size;		    //�����ļ���С
							fat_l2be((U8*)&(file->file_size),4);
							file->dir_entry_sector=sector;				 //����Ŀ¼������������
							file->dir_entry_offset=i;					   //����Ŀ¼��ƫ��
							file->start_cluster=fct[i].start_cluster_l16;	  //������ʼ�غ�
							file->start_cluster<<=16;
							file->start_cluster|=fct[i].start_cluster_h16;
							fat_l2be((U8*)&(file->start_cluster),4);
							file->current_sector=FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster;	 //������ʼ������
							file->current_offset=0;		//��ʼƫ��Ϊ0(���ļ�ͷ)
							return 0;								
						}else{
							if(flag>0)
								NthFile--;//��û�е��Ǹ��ļ�
						}
					}
					break;
				default:
					break;
			}
		}
		tmp=fat_find_sector(sector,1);//Ѱ�Ҹ�Ŀ¼����һ��������
		if(tmp==0)
		{//��Ŀ¼���� �Ҳ������ļ� ��Ҫ�½���ҪΪ��Ŀ¼����ռ�
			if(flag>0)
			{//���½� �����Ҳ���
				return 1;
			}
			//�½��ļ� Ϊ��Ŀ¼����ռ�
			cluster=fat_new_clusters(1);
			if(cluster==0)		//δ����ɹ�
				return 1;
			tmp=(sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster;//���㵱ǰ���ڵĴغ�
			for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//����FAT��
			if(count==0)
				return 1;
			fat[tmp % (FAT_BytesPerSector / sizeof(U32))] = cluster;	//�����´�
			fat_l2be((U8*)&fat[tmp % (FAT_BytesPerSector / sizeof(U32))] , 4);
			for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + tmp / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//д��FAT��
			if(count==0)
				return 1;
			sector = FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster;  //���´صĵ�һ��������Ϊ��ǰ����
			//��������´ص�ȫ������
			for(tmp=0 ; tmp<FAT_SectorsPerCluster ; tmp++)
			{
				for(count=0;count<FAT_BytesPerSector;count++)
					FAT_BUF[count]=0;
				for(count=100;count && FAT_WRITE_SECTOR(sector + tmp,FAT_BUF);count--);//��ո�����
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
//��  ��:�ر��ļ�
//��  ��:fileΪ�ļ���Ϣָ��
//����ֵ:��ȷ����0
//��  ע:
/********************************************************************************/
U8	fat_close_file( FAT_FILE *file )
{
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U8 count;

	for(count=100;count && FAT_READ_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//��������
	if(count==0)
		return 1; 

	//�����ļ���Ϣ	
	fat_strcpy(fct[file->dir_entry_offset].file_name , file->file_name , 11);
	fct[file->dir_entry_offset].file_attr=file->file_attr;
	fct[file->dir_entry_offset].start_cluster_h16=file->start_cluster>>16;
	fct[file->dir_entry_offset].start_cluster_l16=file->start_cluster & 0x0000ffff;
	fat_l2be((U8*)&(fct[file->dir_entry_offset].start_cluster_h16),2);
	fat_l2be((U8*)&(fct[file->dir_entry_offset].start_cluster_l16),2);
	fct[file->dir_entry_offset].file_size=file->file_size;
	fat_l2be((U8*)&(fct[file->dir_entry_offset].file_size),4);

	for(count=100;count && FAT_WRITE_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//��д������
	if(count==0)
		return 1;

	return 0;
}


/********************************************************************************/
//��  ��:ɾ���ļ�
//��  ��:fileΪ�ļ���Ϣָ��
//����ֵ:��ȷ����0
//��  ע:
/********************************************************************************/
U8	fat_delete_file( FAT_FILE *file )
{
	FAT_DIR_ENTRY *fct=FAT_BUF;
	U8 count;

	for(count=100;count && FAT_READ_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//��Ŀ¼������
	if(count==0)
		return 1;

	file->file_name[0]=0xe5;	
	fct[file->dir_entry_offset].file_name[0]=0xe5;//ɾ����־
	
	for(count=100;count && FAT_WRITE_SECTOR(file->dir_entry_sector,FAT_BUF);count--);//д�����
	if(count==0)
		return 1;
		
	return fat_clear_cluster(file->start_cluster);//��մؿռ�		
}


/********************************************************************************/
//��  ��:�����ļ��ĵ�ǰƫ��
//��  ��:offsetΪƫ�Ƶ��ֽ��� ,fileΪ�ļ���Ϣָ��
//����ֵ:��ȷ����0
//��  ע:ֻ��ƫ�Ƶ������ı߽�,��offset�����ļ���Сʱ �����Ĳ�����0��䲢�޸��ļ���С
/********************************************************************************/
U8	fat_file_set_offset( U32 offset , FAT_FILE *file )
{
	U32 sector;
	offset=offset/FAT_BytesPerSector*FAT_BytesPerSector;//ƫ�Ƶ������߽�
	sector=fat_find_sector(FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (file->start_cluster - FAT_RootCluster) * FAT_SectorsPerCluster , offset/FAT_BytesPerSector);
	if(sector==0)
	{//δ�ҵ�
		return 1;
	}
	file->current_sector=sector;
	file->current_offset=offset;
	return 0;
}



/********************************************************************************/
//��  ��:���ļ��ĵ�ǰλ�ö�ȡһ�������������͵�������
//��  ��:fileΪ�ļ���Ϣָ��
//����ֵ:����0��ʾ ��ȷ��ȡ��һ������������ ����ǰƫ�������һ��������λ��
//		����1��ʾ ������ʱ�����˴��� ���ƶ�ƫ��ָ��
//		����2��ʾ ��ǰ�����������ļ����һ������ ���ƶ�ƫ��ָ��
//		����3��ʾ ���ζ���ȷ ���������쳣(�ļ�δ�� �������Ҳ���������) ���ƶ�ƫ��ָ��
//��  ע:��ȡ��ǰƫ���Զ�����ƶ�һ��������λ�� 
/********************************************************************************/
U8	fat_file_read( FAT_FILE *file )
{
	U32 sector;
	U8	count;

	sector=fat_find_sector(file->current_sector , 1);//Ѱ����һ������

	for(count=100;count && FAT_READ_SECTOR(file->current_sector,FAT_BUF);count--);//��������
	if(count==0)
		return 1;
	
	if(file->current_offset + FAT_BytesPerSector >= file->file_size)
		return 2;	   //ƫ�ƺ󳬹����ļ���С(���������ļ����һ������)

//δ�ﵽ�ļ�β
	if(sector)
	{  //���� ��������ҵ�����
		file->current_offset += FAT_BytesPerSector;
		file->current_sector = sector;
		return 0;
	}
	else //������ �����Ҳ���������
		return 3;		
}




/********************************************************************************/
//��  ��:�ѻ����������ݴ�д�뵽�ļ��ĵ�ǰƫ��λ��
//��  ��:fileΪ�ļ���Ϣָ��
//����ֵ:����0��ʾ ��ȷд����һ������������ ����ǰƫ�������һ��������λ�� 
//			��ƫ�Ƴ����ļ���Сʱ �Զ�������һ���صĴ�С ���޸��ļ���С����ǰƫ��
//		����1��ʾ ��д������ʱ�����˴��� ���ƶ�ƫ��ָ��
//��  ע:д���ǰƫ���Զ�����ƶ�һ��������λ��
/********************************************************************************/
U8	fat_file_write( FAT_FILE *file )
{
	U8 count;
	U32 cluster;
	U32 sector;
	U32 *fat=FAT_BUF;

	for(count=100;count && FAT_WRITE_SECTOR(file->current_sector,FAT_BUF);count--);//д�����
	if(count==0)
		return 1;
		
	sector=fat_find_sector(file->current_sector , 1);//Ѱ����һ������
	if(sector)
	{
		file->current_sector=sector;	//ǰ��һ������	
	}else{	//�������һ���ص����һ����������Ҫ����һ����
		cluster=fat_new_clusters(1);
		if(cluster==0)		//δ����ɹ�
			return 1;
		sector=(file->current_sector - FAT_HiddenSectors - FAT_ReservedSectors - FAT_NumberOfFats * FAT_SectorsPerFat) / FAT_SectorsPerCluster + FAT_RootCluster;//���㵱ǰ���ڵĴغ�
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//����FAT��
		if(count==0)
			return 1;
		fat[sector % (FAT_BytesPerSector / sizeof(U32))] = cluster;	//�����´�
		fat_l2be((U8*)&fat[sector % (FAT_BytesPerSector / sizeof(U32))] , 4);
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + sector / (FAT_BytesPerSector / sizeof(U32)),FAT_BUF);count--);//д��FAT��
		if(count==0)
			return 1;
		file->current_sector = FAT_HiddenSectors + FAT_ReservedSectors + FAT_NumberOfFats * FAT_SectorsPerFat + (cluster - FAT_RootCluster) * FAT_SectorsPerCluster;  //���´صĵ�һ��������Ϊ��ǰ����
	}
	file->current_offset += FAT_BytesPerSector;	   //ǰ��һ��������ƫ��
	if(file->current_offset > file->file_size)		  //�����ļ��ܴ�Сʱ�޸��ļ���С
		file->file_size=file->current_offset;	
	return 0;
}


/********************************************************************************/
//��  ��:�����ļ��Ĵ�С
//��  ��:file_sizeΪ�����ú��ļ��Ĵ�С,fileΪ�ļ���Ϣָ��
//����ֵ:��ȷ����0
//��  ע:���ļ���ǰ��С����file_size,�������ļ���С;���ļ���ǰ��С����file_size,����ļ�β��ɾ���ļ���С
/********************************************************************************/
U8 fat_file_resize(U32 file_size , FAT_FILE *file)
{
	U32 cluster1;
	U32 cluster2;
	U32 *fat=FAT_BUF;
	U32 tmp,new_clusters,fore_cluster;
	U8 count;


	//�°��м��㵱ǰ�ļ���ռ�Ĵ���
	if(file->file_size==0)
		cluster1=1;	 //�ļ�����ռһ���� ��ʹ��СΪ0
	else
	{
		cluster1 = file->file_size / FAT_SectorsPerCluster / FAT_BytesPerSector; 
		if(file->file_size % (FAT_SectorsPerCluster * FAT_BytesPerSector))
			cluster1++;
	}

	//�°��м������ú��ļ���ռ�Ĵ���
	if(file_size==0)
		cluster2=1;	 //�ļ�����ռһ���� ��ʹ��СΪ0
	else
	{
		cluster2 = file_size / FAT_SectorsPerCluster / FAT_BytesPerSector; 
		if(file_size % (FAT_SectorsPerCluster * FAT_BytesPerSector))
			cluster2++;
	}

	if(cluster1 > cluster2)
	{//��ɾ��
		cluster1 = fat_find_cluster(file->start_cluster , cluster2 - 1);//�������ú��β�غ�
		if(cluster1 == 0)
			return 1;//���� δ�ҵ���
		cluster2 = fat_find_cluster(cluster1 , 1);//�������ú��β����һ�غ� ������յ��״غ�
		if(cluster2 == 0)
			return 1;//���� δ�ҵ���
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��ȡβ�����ڵ�����
		if(count==0) //����������
			return 1;
		fat[cluster1%(FAT_BytesPerSector / sizeof(U32))] = 0xffffff0f; //�����ر�־
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��д
		if(count==0)
			return 1;
		fat_clear_cluster(cluster2);//���β����				
	}else if(cluster1 < cluster2)
	{//������
		tmp = cluster2 - cluster1;//������Ҫ������ٸ���
		cluster2=0;//������¼�´������״غ�
		while(tmp>0)
		{//����һ������
			new_clusters = (tmp>FAT_BytesPerSector/sizeof(U32))?FAT_BytesPerSector/sizeof(U32):tmp;//���������Ĵ��������������������������������������� ��������tmp��
			tmp -= new_clusters;//����ʣ��������Ĵ���
			new_clusters = fat_new_clusters(new_clusters);//����һ������
			if(new_clusters==0)
				return 1;//����ʧ��	  
			if(cluster2==0)
				cluster2=new_clusters;//�����´������״غ�
			else{//���ӵ���һ��������β��
				for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + fore_cluster/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��ȡ�ϸ�������β�����ڵ�����
				if(count==0) //����������
					return 1;
				fat[fore_cluster%(FAT_BytesPerSector / sizeof(U32))] = new_clusters; //���Ӵ�
				fat_l2be((U8*)&fat[fore_cluster%(FAT_BytesPerSector / sizeof(U32))],sizeof(U32));//ת��
				for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + fore_cluster/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��д
				if(count==0)
					return 1;								
			}
			fore_cluster = new_clusters + FAT_BytesPerSector/sizeof(U32) - 1;//��¼��������Ĵ��������һ���صĴغ�(��Ϊ�������Ĵ��� ����ֱ�Ӽ���)
		}

		cluster1 = fat_find_cluster(file->start_cluster , cluster1 - 1);//�����ļ���ǰ��β�غ�
		if(cluster1 == 0)
			return 1;//���� δ�ҵ���
		for(count=100;count && FAT_READ_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��ȡ�ļ���β�����ڵ�����
		if(count==0) //����������
			return 1;
		fat[cluster1%(FAT_BytesPerSector / sizeof(U32))] = cluster2; //���Ӵ�
		fat_l2be((U8*)&fat[cluster1%(FAT_BytesPerSector / sizeof(U32))],sizeof(U32));//ת��
		for(count=100;count && FAT_WRITE_SECTOR(FAT_HiddenSectors + FAT_ReservedSectors + cluster1/(FAT_BytesPerSector / sizeof(U32)), FAT_BUF);count--);//��д
		if(count==0)
			return 1;					
	}
	//�ļ�����һ��
	file->file_size = file_size;
	return 0;
}
 
#endif
