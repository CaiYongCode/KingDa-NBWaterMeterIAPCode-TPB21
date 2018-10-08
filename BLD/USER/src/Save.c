/*********************************************************************************************************
//������
//���ߣ����ƮԾ    ʱ�䣺       �ص㣺
//CPU�ͺţ�         ϵͳ��Ƶ��
//�汾�ţ�V0.0        
*********************************************************************************************************/
/*********************************************************************************
�ļ�������
*********************************************************************************/
#include "include.h"
/*********************************************************************************
����������
*********************************************************************************/

/*********************************************************************************
��������������
*********************************************************************************/
/*********************************************************************************
�ⲿ����������
*********************************************************************************/
/*********************************************************************************
˽�б���������
*********************************************************************************/ 
/*********************************************************************************
���Ա���������
*********************************************************************************/
/*********************************************************************************
�ڲ�����������
*********************************************************************************/
/*********************************************************************************
���ܴ��붨����
*********************************************************************************/
/*********************************************************************************
 Function:      //
 Description:   //��ROMд����
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void WriteRom (unsigned short addr, void *pbuff, unsigned char length)
{
 unsigned char itemp;
 unsigned char cnt;
 unsigned char *p; 
 unsigned short itemp16;
 do
 {FLASH_Unlock(FLASH_MemType_Data); }//����EEPROM���
 while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET); 
 itemp = 0;
 p = pbuff;
 cnt =0 ;
 while (itemp < length)
 {
   FLASH_ProgramByte(addr ,p[itemp]);
   itemp16 = 0xffff;
   while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET)
   {
    itemp16 --;
    if(itemp16 <= 1)
    {break;}
   }
   itemp16 = FLASH_ReadByte(addr);
   if(p[itemp] != itemp16)
   {
    if(++cnt > 50)
    {
     FLASH_Lock(FLASH_MemType_Data);
    }
    continue ; 
   }
   else
   {cnt = 0;itemp ++;addr ++;}
 }
 FLASH_Lock(FLASH_MemType_Data);
}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Read_Version(unsigned short addr,unsigned char *version)
{
  unsigned char i = 0;
  
  for(i = 0;i < 16;i++)
  {
    version[i] = *((const unsigned char *)(addr+i));
  }
}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Save_Version(unsigned short addr,unsigned char *version)
{
  WriteRom (addr,version,16);
}
   /*********************************************************************************
 Function:      //
 Description:   //��ȡˮ�����
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Read_Meter_Parameter(void)
{
  //��ȡ�ϱ�Ƶ��
  MeterParameter.ReportFrequency = *((const unsigned short *)(REPORT_FREQUENCY_ADDR));
  if(MeterParameter.ReportFrequency < 5) //Ĭ���ϱ�Ƶ��24Сʱ
  {
    MeterParameter.ReportFrequency = 1440;
  } 
}
/*********************************************************************************
 Function:      //
 Description:   //��ȡ�ۻ�ˮ��
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Read_ACUM_Flow(unsigned short addr,union flow_union *Flow)       
{
  union flow_union *flow;
  flow = (union flow_union *)(addr);
  Flow->flow32 = flow->flow32;
}
/*********************************************************************************
 Function:      //
 Description:   //�洢�ۻ�ˮ��
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Save_Add_Flow(u16 addr,union flow_union *Flow)       
{
    WriteRom (addr,Flow->flow8,4);      
}

/*********************************************************************************
 Function:      //
 Description:   //��ȡAPP������Ч��־
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Read_APP_Valid(void)
{  
  APPValid = *((const unsigned char *)(APP_VALID_ADD));
}
/*********************************************************************************
 Function:      //
 Description:   //�洢APP������Ч��־
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Save_APP_Valid(void)
{  
  WriteRom (APP_VALID_ADD,&APPValid,1);
}
/*********************************************************************************
 Function:      //
 Description:   //��ȡ������Ϣ
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Read_Upgrade_Info(void)
{  
//  unsigned char i = 0; 
  Upgrade.Process =(enum Upgrade_Process)(*((const unsigned char *)(Upgrade_Process_ADD)));
  if(Upgrade.Process != IDLE)
  {
    Upgrade.TimeoutCounter = UPGRADE_TIMEOUT_MAX;
  }
  
//  for(i = 0;i < 16;i++)
//  {
//    Upgrade.Version[i] = *((const unsigned char *)(UPGRADE_VERSION_ADD+i));
//  }
//  Upgrade.PackageSize = *((const unsigned short *)(UPGRADE_PACKAGE_SINGLE_SIZE));
//  Upgrade.PackageTotalNum = *((const unsigned short *)(UPGRADE_PACKAGE_TOTAL_NUMBER));
}
/*********************************************************************************
 Function:      //
 Description:   //�洢������Ϣ
 Input:         //
                //
 Output:        //
 Return:	//
 Others:        //
*********************************************************************************/
void Save_Upgrade_Info(void)
{
  //�������п��У������������Ϣ
//  if(Upgrade.Process == IDLE)
//  {
//    memset(Upgrade.Version,'\0',16);
//    Upgrade.PackageSize = 0;
//    Upgrade.PackageTotalNum = 0;
//  }
  WriteRom (Upgrade_Process_ADD,&(Upgrade.Process),1);
//  WriteRom (UPGRADE_VERSION_ADD,Upgrade.Version,16);
//  WriteRom (UPGRADE_PACKAGE_SINGLE_SIZE,&(Upgrade.PackageSize),2);
//  WriteRom (UPGRADE_PACKAGE_TOTAL_NUMBER,&(Upgrade.PackageTotalNum),2);
}
/******************************************END********************************************************/
