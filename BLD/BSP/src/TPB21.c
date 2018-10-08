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
struct TPB21_Str TPB21;            //TPB21 �õļĴ���
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
 Function:      //void TPB21_Power_On(void)
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void TPB21_Power_On(void)        //TPB21�ϵ�
{
  MeterParameter.DeviceStatus = RUN;
  
  RCC_Configuration();
  USART2_Configuration();       //��ʼ������2
  
  GPIO_SetBits(GPIOE,GPIO_Pin_2);       //VBAT����        3.1-4.2V������ֵ3.6V
  GPIO_SetBits(GPIOE,GPIO_Pin_1);      //��λ������
  
  TPB21.Report_Bit = 1;
  Create_Timer(ONCE,1,
               TPB21_Reset,0,PROCESS); 
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
void TPB21_Power_Off(void)        //TPB21�ϵ�
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_2); 
}
/*********************************************************************************
 Function:      //
 Description:   //TPB21��λ
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void TPB21_Reset(void)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_1);     //��λ������

  Create_Timer(ONCE,5,
               TPB21_Start,0,PROCESS); 
}

/*********************************************************************************
 Function:      //
 Description:   ////TPB21����
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void TPB21_Start(void)        
{
  Uart2.Sent_Length = 0;                                //��մ���2���ͳ���
  Uart2.Receive_Length = 0;                             //��մ���2���ճ���
  Uart2.Send_Busy = FALSE;                              //��մ���2����æ��־  
  Uart2.Receive_Busy = FALSE;                           //��մ���2����æ
  Uart2.Receive_Pend = FALSE;                           //��մ���2����
  
  TPB21.Start_Process = NRB; //ָ����һ������
  TPB21.Incident_Pend = TRUE; //�¼���־����
  TPB21.Err_Conner.Connect = SEND_ERROR_NUM;     //���ӳ�ʱ����
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
void TPB21_Process(void)                         //TPB21������
{
  char *str = NULL;
  char *str1 = NULL;
  char *str2 = NULL;

  //�����Ҫ�ϱ���Ϣ������TPB21
  if(TPB21.Report_Bit != 0)
  {
    if(TPB21.Start_Process == TPB21_RECONNECT)
    {
      if(TPB21.Reconnect_Times >= 3)  //������������˯��
      {
        TPB21.Reconnect_Times = 0;  
        TPB21.FailTimes++;  
        MCU_DeInit(); 
      }
      else     //��������
      {
        MeterParameter.DeviceStatus = RUN;
        TPB21_Start();
      }
    }
  }
  
  if((TPB21.Incident_Pend != FALSE)||(Upgrade.Incident_Pend != FALSE)) //����Ƿ����¼�����
  {
    TPB21.Incident_Pend = FALSE; //����¼�����
    Upgrade.Incident_Pend = FALSE;
    switch(TPB21.Start_Process)
    {
    case NRB:                  //����
      {
        TPB21_Data_Send("AT+NRB\r\n",8);
        Create_Timer(ONCE,10,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case AT:                  //ͬ��������
      {
        TPB21_Data_Send("AT\r\n",4);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);
      }
      break;
    case MLWNMI0:                 //���ý�����Ϣָʾ
      {
        TPB21_Data_Send("AT+MLWNMI=0\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETNBAND:               //��ѯƵ��
      {
        
        TPB21_Data_Send("AT+NBAND?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETCFUN:                //��ѯ�绰����
      {
        TPB21_Data_Send("AT+CFUN?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      } 
      break;
    case CGSN:                 //��ѯIMEI      
      {
        TPB21_Data_Send("AT+CGSN=1\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;  
     case CCID:                 //��ѯCCID
      {
        TPB21_Data_Send("AT+NCCID\r\n",10); 
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;  
    case CSQ:                //��ѯ�ź�ǿ��
      {
        TPB21_Data_Send("AT+CSQ\r\n",8);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      } 
      break;
    case GETCGATT:                //��ѯ���缤��״̬    
      {
        TPB21_Data_Send("AT+CGATT?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;    
    case CEREG:                //��ѯ����ע��״̬     
      {
        TPB21_Data_Send("AT+CEREG?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case CCLK:                //��ѯʵʱʱ�� 
      {
        TPB21_Data_Send("AT+CCLK?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETNCDP:                 //��ѯCDP������
      {
        TPB21_Data_Send("AT+NCDP?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case SETNCDP:                 //����CDP������ 
      {
        TPB21_Data_Send("AT+NCDP=180.101.147.115,5683\r\n",30);   
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break; 
    case MLWSMI:                 //���÷�����Ϣָʾ
      {
        TPB21_Data_Send("AT+MLWSMI=1\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);//���鶨ʱ����ʱ�ص�
      }
      break;
    case MLWNMI1:                 //���ý�����Ϣָʾ
      {
        TPB21_Data_Send("AT+MLWNMI=1\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);//���鶨ʱ����ʱ�ص�
      }
      break;
    case MLWULDATA:                 //������Ϣ     
      { 
        //��ִ���ϱ����Ա�֤�����ɹ�
        if(TPB21.Report_Bit != 0)
        {
          Report_History_Data();          //��������
          Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                       TPB21_Recv_Timeout_CallBack,0,PROCESS); 
        }
        else
        {
          if(Upgrade.Process != IDLE)
          {
            Upgrade_Send_Process();       //��������
          }
          else
          {
            Create_Timer(ONCE,10,
                         MCU_DeInit,0,PROCESS); //10s��ػ�
          }
        }
      }
      break;
    case TPB21_CONNECT_ERROR:      //����ʧ��
      TPB21.Start_Process = TPB21_RECONNECT;
      TPB21.Reconnect_Times++;
      break;
    case TPB21_POWER_DOWN:       //���ͽ��������ֱ��˯��
      MCU_DeInit(); 
      break;
    default:
      break;
    }
  }
  
  if(Uart2.Receive_Pend != FALSE)//�ж�������
  {   
    switch(TPB21.Start_Process)
    {
    case NRB:                  //����
      {
        if(strstr(Uart2.R_Buffer,"REBOOT_CAUSE_APPLICATION_AT") != NULL)
        {
          TPB21.Start_Process = AT;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
    case AT:            //ͬ��������
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWNMI0;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
    case MLWNMI0:                 //���ý�����Ϣָʾ
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = GETNBAND;

          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
          Create_Timer(ONCE,20,
                       TPB21_Delay_CallBack,0,PROCESS); 
        }  
      }
      break;
    case GETNBAND:               //��ѯƵ��
      {       
        if(strstr(Uart2.R_Buffer,"+NBAND:5") != NULL)    //֧��Ƶ��5
        {
          TPB21.Start_Process = GETCFUN;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
    case GETCFUN:                //��ѯ�绰����
      {
        if(strstr(Uart2.R_Buffer,"+CFUN:1") != NULL)     //ȫ����
        {         
          TPB21.Start_Process = CGSN;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      } 
      break;
    case CGSN:         // ��ѯIMEI
      {
        str = strstr(Uart2.R_Buffer,"+CGSN");
        if( str != NULL)//��ȡ��IMEI
        {
          TPB21.Start_Process = CCID;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
     case CCID:          //��ѯCCID
      {
        str = strstr(Uart2.R_Buffer,"+NCCID");
        if( str != NULL)
        {
          TPB21.Start_Process = CSQ;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
    case CSQ:           //��ѯ�ź�ǿ��
      {
        str = strstr(Uart2.R_Buffer,"+CSQ");
        if( str != NULL)
        {
          TPB21.Rssi =0;//�����ź�ǿ��
          if( (str[5] >= '0') && (str[5] <= '9') )
          {
            TPB21.Rssi += str[5]-0x30;
          }
          if( (str[6] >= '0') && (str[6] <= '9') )
          {
            TPB21.Rssi *=10;
            TPB21.Rssi += str[6]-0x30;
          }
    
          if(TPB21.Rssi < 99)
          {
            TPB21.Start_Process = GETCGATT;
            TPB21.Incident_Pend = TRUE;//��ǹ���
            Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
          }
        }
      }
      break;
    case GETCGATT:       //��ѯ���缤��״̬
      {
        if( strstr(Uart2.R_Buffer,"+CGATT:1") != NULL)//���缤��
        {        
          TPB21.Start_Process = CEREG;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }       
      }
      break;
    case CEREG:       //��ѯ����ע��״̬
      {
        if( strstr(Uart2.R_Buffer,"+CEREG:0,1") != NULL)//����ע��
        {        
          TPB21.Start_Process = CCLK;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;   
    case CCLK:       //��ѯʵʱʱ��
      {
        str = strstr(Uart2.R_Buffer,"+CCLK:");
        if( str != NULL)
        {  
          GMT_to_BT((unsigned char*)str);
        }
        TPB21.Start_Process = GETNCDP;
        TPB21.Incident_Pend = TRUE;//��ǹ���
        Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
      }
      break;   
    case  GETNCDP:                 //��ѯCDP������
      {
        if( strstr(Uart2.R_Buffer,"+NCDP:180.101.147.115,5683") != NULL)//��ȡ��NCDP
        {        
          TPB21.Start_Process = MLWSMI;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
        else
        {        
          TPB21.Start_Process = SETNCDP;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }
      }
      break;
    case  SETNCDP:                 //����CDP������ 
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = GETNCDP;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }  
      }
      break;
    case MLWSMI:                 //���÷�����Ϣָʾ
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWNMI1;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }  
      }
      break;
    case MLWNMI1:                 //���ý�����Ϣָʾ
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWULDATA;
          TPB21.Incident_Pend = TRUE;//��ǹ���
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ����ʱ�ص�
        }  
      }
      break;
    case MLWULDATA:        //������Ϣ
      {  
        str1 = strstr(Uart2.R_Buffer,"+MLWDLDATA:"); 
        //������Ϣճ��
        while(str1 != NULL)
        { 
          str2 = strstr(str1+6,"+MLWDLDATA:"); 
          if(strnstr(str1,"+MLWDLDATA:4,AAAA0002",24) != NULL)     //�ϱ���ʷ���ݵ���Ӧ
          {
            TPB21.Report_Bit = 0;
            TPB21.Incident_Pend = TRUE;//��ǹ���
            Delete_Timer(TPB21_Recv_Timeout_CallBack);//ɾ�����ճ�ʱ�ص�
          }
          else if(strnstr(str1,",FFFE",24) != NULL)           //�����������
          { 
            Delete_Timer(MCU_DeInit);//ɾ����ʱ�ص�
            Upgrade_Recv_Process((unsigned char*)str1);
          }
          
          str1 = str2;
        }
      }
      break;
    case TPB21_CONNECT_ERROR:
      //����쳣����
      break;
    default:
      break;
    }
    
    memset(Uart2.R_Buffer,'\0',Uart2.Receive_Length);//����ջ�����
    Uart2.Receive_Length = 0;
    Uart2.Receive_Pend = FALSE;
  }
  
}
   
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void TPB21_Data_Send(unsigned char *Data,unsigned short Len)
{    
  Uart2_Send((unsigned char*)Data,Len);
}

/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void TPB21_Recv_Timeout_CallBack(void)//������ʱ�ط�
{
  if(TPB21.Err_Conner.Connect != 0)//�жϴ����Ƿ�ʱ
  {
    TPB21.Incident_Pend = TRUE;
    TPB21.Err_Conner.Connect--;
  }
  else
  {   
    TPB21.Incident_Pend = TRUE;
    TPB21.Start_Process = TPB21_CONNECT_ERROR;//��������
  }
}
/*********************************************************************************
 Function:      //
 Description:   //��ʱ�ص�����
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void TPB21_Delay_CallBack(void)
{
  TPB21.Incident_Pend = TRUE;//��ǹ���
}
/*********************************************************************************
 Function:      //
 Description:   //�ϱ���ʷ����
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void Report_History_Data(void)
{
  uint8_t data[50] = "AT+MLWULDATA=12,080000000000000000000000\r\n";

   //��ȡʱ��   
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
  
  //�ۻ�����
  data[18] = Int_to_ASCLL(Cal.Water_Data.flow8[0]/0x10);
  data[19] = Int_to_ASCLL(Cal.Water_Data.flow8[0]%0x10);
  data[20] = Int_to_ASCLL(Cal.Water_Data.flow8[1]/0x10);
  data[21] = Int_to_ASCLL(Cal.Water_Data.flow8[1]%0x10);
  data[22] = Int_to_ASCLL(Cal.Water_Data.flow8[2]/0x10);
  data[23] = Int_to_ASCLL(Cal.Water_Data.flow8[2]%0x10);
  data[24] = Int_to_ASCLL(Cal.Water_Data.flow8[3]/0x10);
  data[25] = Int_to_ASCLL(Cal.Water_Data.flow8[3]%0x10);
  //��
  data[26] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)/0x1000);
  data[27] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x1000/0x100);
  data[28] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x100/0x10);
  data[29] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x10); 
  //��
  data[30] = Int_to_ASCLL(RTC_DateStr.RTC_Month/0x10);
  data[31] = Int_to_ASCLL(RTC_DateStr.RTC_Month%0x10);
  //��
  data[32] = Int_to_ASCLL(RTC_DateStr.RTC_Date/0x10);
  data[33] = Int_to_ASCLL(RTC_DateStr.RTC_Date%0x10);
  //ʱ
  data[34] = Int_to_ASCLL(RTC_TimeStr.RTC_Hours/0x10);
  data[35] = Int_to_ASCLL(RTC_TimeStr.RTC_Hours%0x10);
  //��
  data[36] = Int_to_ASCLL(RTC_TimeStr.RTC_Minutes/0x10);
  data[37] = Int_to_ASCLL(RTC_TimeStr.RTC_Minutes%0x10);
  //��
  data[38] = 0x30;
  data[39] = 0x30;
  
  TPB21_Data_Send(data,42);
}
/******************************************END********************************************************/