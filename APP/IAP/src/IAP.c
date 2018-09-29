/*********************************************************************************
//������
//���ߣ����ƮԾ     ʱ�䣺       �ص㣺
//CPU�ͺţ�         ϵͳ��Ƶ��
//�汾�ţ�V0.0        
*********************************************************************************/
/*********************************************************************************
�ļ�������
*********************************************************************************/
#include "IAP.h"
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
struct Upgrade_Str Upgrade;
unsigned char APPValid;
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
 Description:   //��EEPROM��ַ����
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
uint32_t EEPROM_ReadWord(uint32_t Address)
{
 return(*(PointerAttr uint32_t *) (uint32_t)Address);       
}

/*********************************************************************************
 Function:      //
 Description:   ////���³�ʼ��STM8���ж�������  �������¶��嵽BLD���ж�������
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void STM8_Interrupt_Vector_Table_Redirection(void)
{
  disableInterrupts();   //�ر��ж�  
  uint8_t Index = 0;	
  uint32_t data = 0;
 
  FLASH_Unlock(FLASH_MemType_Program);
  for(Index = 1; Index < 0x20;Index++)
  {
    data = EEPROM_ReadWord(INTERRUPT_VECTOR_ADD+4*Index);
    FLASH_ProgramWord(0x8000+4*Index,data);
  }
  FLASH_Lock(FLASH_MemType_Program);
}
/*********************************************************************************
 Function:      //
 Description:   //��ת��������������
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void JumptoBLD(void)
{
  sim();               // disable interrupts�����������תǰ���жϣ���ת���³��������һ���жϡ�
  asm("LDW X,  SP ");
  asm("LD  A,  $FF");
  asm("LD  XL, A  ");
  asm("LDW SP, X  ");
  asm("JPF $8000");
}
/*********************************************************************************
 Function:      //
 Description:   //����BLD����
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Run_BLD(void)
{
  STM8_Interrupt_Vector_Table_Redirection();
  JumptoBLD();
}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Upgrade_Recv_Process(unsigned char *str)
{
  unsigned short i = 0,j = 0;

  unsigned char MessageID = 0;
  unsigned short crc16 = 0;
  
  memset(Upgrade.Buffer,'\0',Upgrade.Length);//�建��
  Upgrade.Length = 0;
  
  i = 6;
  while(str[i] != ',')
  {
    if( (str[i] >= '0')&&(str[i] <= '9') )
    {
      Upgrade.Length = Upgrade.Length*10+ASCLL_to_Int(str[i]);
    }
    i++;
  }
 
  for(j = 0;j < Upgrade.Length;j++)
  {
    Upgrade.Buffer[j] = ASCLL_to_Int(str[i+1+2*j])*16+ASCLL_to_Int(str[i+2+2*j]);
  }

  crc16 = Upgrade.Buffer[4]*256+Upgrade.Buffer[5];
  Upgrade.Buffer[4] = 0;
  Upgrade.Buffer[5] = 0;
  
  //CRC16У��
  if(crc16 != CRC16((unsigned char*)Upgrade.Buffer,Upgrade.Length))
  {
    return;
  }    
   
  MessageID = Upgrade.Buffer[3];
  switch(MessageID)
  {
    case MESSAGE19:            //��ѯ�豸�汾
      {
        Upgrade.Process = MESSAGE19;
        Save_Upgrade_Info();
        
        Run_BLD();
      }
      break;
    case MESSAGE24:            //�豸�ϱ������ɹ���ƽ̨��Ӧ
      {
        if(Upgrade.Buffer[8] == 0x00)     //ƽ̨����ɹ�
        {
          Upgrade.Process = IDLE;
          Save_Upgrade_Info();         

          Delete_Timer(Upgrade_TimeOut_CallBack);//ɾ��������ʱ�ص�
          
          MCU_DeInit();
        }
      }
      break;
    default:
      break;
  }
}
/*********************************************************************************
 Function:      //
 Description:   //�������ͽ���
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Upgrade_Send_Process(void)
{
  
  switch(Upgrade.Process)
  {
    case MESSAGE24:            //�豸�ϱ������ɹ�
      {
        SendUpgradeMessage24();
        Create_Timer(ONCE,UPGRADE_TIMEROUT_TIME,
                     Upgrade_TimeOut_CallBack,0,PROCESS);//������ʱ�ص�
      }
      break;
    default:
      break;
  }
}
/*********************************************************************************
 Function:      //
 Description:   //�豸�ϱ������ɹ�
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void SendUpgradeMessage24(void)
{
  unsigned char i = 0;
  unsigned short crc16 = 0;
  unsigned char version[16] = {0};
  unsigned char buff[26] = {0};
  uint8_t data[70] = "AT+MLWULDATA=25,00000000000000000000000000000000000000000000000000\r\n";
  
  Read_Version(APP_VERSION_ADD,version);
    
  buff[0] = 0xFF;
  buff[1] = 0xFE;
  buff[2] = 0x01;
  buff[3] = MESSAGE24;
  buff[4] = 0;
  buff[5] = 0;
  buff[6] = 0;
  buff[7] = 17;
  buff[8] = 0x00;
  memcpy(&buff[9],version,16);
  
  crc16 = CRC16(buff,25);
  buff[4] = (crc16>>8)&0xFF;
  buff[5] = crc16&0xFF;
  
  for(i = 0;i < 25;i++)    
  {
    data[16+2*i] = Int_to_ASCLL(buff[i]/0x10);
    data[17+2*i] = Int_to_ASCLL(buff[i]%0x10);
  }
  
  TPB21_Data_Send(data,68);
}
/*********************************************************************************
 Function:      //
 Description:   //������ʱ�ص�����

 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void Upgrade_TimeOut_CallBack(void)
{
  if(Upgrade.TimeoutCounter != 0)//�жϴ����Ƿ�ʱ
  {
    Upgrade.Incident_Pend = TRUE;
    Upgrade.TimeoutCounter--;
  }
  else
  {
    Upgrade.Process = IDLE;
    Save_Upgrade_Info();
    MCU_DeInit();
  }
}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
/**************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/

/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/

/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/

/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
