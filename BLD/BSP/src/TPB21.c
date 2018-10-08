/*********************************************************************************
文件包含区
*********************************************************************************/
#include "include.h"
/*********************************************************************************
常量定义区
*********************************************************************************/

/*********************************************************************************
公共变量定义区
*********************************************************************************/
/*********************************************************************************
外部变量声明区
*********************************************************************************/
/*********************************************************************************
私有变量定义区
*********************************************************************************/ 
struct TPB21_Str TPB21;            //TPB21 用的寄存器
/*********************************************************************************
测试变量定义区
*********************************************************************************/
/*********************************************************************************
内部函数定义区
*********************************************************************************/
/*********************************************************************************
功能代码定义区
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
void TPB21_Power_On(void)        //TPB21上电
{
  MeterParameter.DeviceStatus = RUN;
  
  RCC_Configuration();
  USART2_Configuration();       //初始化串口2
  
  GPIO_SetBits(GPIOE,GPIO_Pin_2);       //VBAT拉高        3.1-4.2V，典型值3.6V
  GPIO_SetBits(GPIOE,GPIO_Pin_1);      //复位脚拉低
  
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
void TPB21_Power_Off(void)        //TPB21断电
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_2); 
}
/*********************************************************************************
 Function:      //
 Description:   //TPB21复位
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void TPB21_Reset(void)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_1);     //复位脚拉高

  Create_Timer(ONCE,5,
               TPB21_Start,0,PROCESS); 
}

/*********************************************************************************
 Function:      //
 Description:   ////TPB21启动
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void TPB21_Start(void)        
{
  Uart2.Sent_Length = 0;                                //清空串口2发送长度
  Uart2.Receive_Length = 0;                             //清空串口2接收长度
  Uart2.Send_Busy = FALSE;                              //清空串口2发送忙标志  
  Uart2.Receive_Busy = FALSE;                           //清空串口2接收忙
  Uart2.Receive_Pend = FALSE;                           //清空串口2挂起
  
  TPB21.Start_Process = NRB; //指向下一个流程
  TPB21.Incident_Pend = TRUE; //事件标志挂起
  TPB21.Err_Conner.Connect = SEND_ERROR_NUM;     //连接超时次数
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
void TPB21_Process(void)                         //TPB21主进程
{
  char *str = NULL;
  char *str1 = NULL;
  char *str2 = NULL;

  //如果需要上报消息，启动TPB21
  if(TPB21.Report_Bit != 0)
  {
    if(TPB21.Start_Process == TPB21_RECONNECT)
    {
      if(TPB21.Reconnect_Times >= 3)  //重连超次数则睡眠
      {
        TPB21.Reconnect_Times = 0;  
        TPB21.FailTimes++;  
        MCU_DeInit(); 
      }
      else     //否则重连
      {
        MeterParameter.DeviceStatus = RUN;
        TPB21_Start();
      }
    }
  }
  
  if((TPB21.Incident_Pend != FALSE)||(Upgrade.Incident_Pend != FALSE)) //检测是否有事件挂起
  {
    TPB21.Incident_Pend = FALSE; //清除事件挂起
    Upgrade.Incident_Pend = FALSE;
    switch(TPB21.Start_Process)
    {
    case NRB:                  //重启
      {
        TPB21_Data_Send("AT+NRB\r\n",8);
        Create_Timer(ONCE,10,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case AT:                  //同步波特率
      {
        TPB21_Data_Send("AT\r\n",4);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);
      }
      break;
    case MLWNMI0:                 //禁用接收消息指示
      {
        TPB21_Data_Send("AT+MLWNMI=0\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETNBAND:               //查询频段
      {
        
        TPB21_Data_Send("AT+NBAND?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETCFUN:                //查询电话功能
      {
        TPB21_Data_Send("AT+CFUN?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      } 
      break;
    case CGSN:                 //查询IMEI      
      {
        TPB21_Data_Send("AT+CGSN=1\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;  
     case CCID:                 //查询CCID
      {
        TPB21_Data_Send("AT+NCCID\r\n",10); 
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;  
    case CSQ:                //查询信号强度
      {
        TPB21_Data_Send("AT+CSQ\r\n",8);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      } 
      break;
    case GETCGATT:                //查询网络激活状态    
      {
        TPB21_Data_Send("AT+CGATT?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;    
    case CEREG:                //查询网络注册状态     
      {
        TPB21_Data_Send("AT+CEREG?\r\n",11);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case CCLK:                //查询实时时间 
      {
        TPB21_Data_Send("AT+CCLK?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case GETNCDP:                 //查询CDP服务器
      {
        TPB21_Data_Send("AT+NCDP?\r\n",10);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break;
    case SETNCDP:                 //设置CDP服务器 
      {
        TPB21_Data_Send("AT+NCDP=180.101.147.115,5683\r\n",30);   
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS); 
      }
      break; 
    case MLWSMI:                 //设置发送消息指示
      {
        TPB21_Data_Send("AT+MLWSMI=1\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);//建议定时器延时回调
      }
      break;
    case MLWNMI1:                 //设置接收消息指示
      {
        TPB21_Data_Send("AT+MLWNMI=1\r\n",13);
        Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                     TPB21_Recv_Timeout_CallBack,0,PROCESS);//建议定时器延时回调
      }
      break;
    case MLWULDATA:                 //发送消息     
      { 
        //先执行上报，以保证联网成功
        if(TPB21.Report_Bit != 0)
        {
          Report_History_Data();          //发送数据
          Create_Timer(ONCE,TPB21_R_TIMEROUT_TIME,
                       TPB21_Recv_Timeout_CallBack,0,PROCESS); 
        }
        else
        {
          if(Upgrade.Process != IDLE)
          {
            Upgrade_Send_Process();       //发送升级
          }
          else
          {
            Create_Timer(ONCE,10,
                         MCU_DeInit,0,PROCESS); //10s后关机
          }
        }
      }
      break;
    case TPB21_CONNECT_ERROR:      //连接失败
      TPB21.Start_Process = TPB21_RECONNECT;
      TPB21.Reconnect_Times++;
      break;
    case TPB21_POWER_DOWN:       //发送接收完成则直接睡眠
      MCU_DeInit(); 
      break;
    default:
      break;
    }
  }
  
  if(Uart2.Receive_Pend != FALSE)//判断有数据
  {   
    switch(TPB21.Start_Process)
    {
    case NRB:                  //重启
      {
        if(strstr(Uart2.R_Buffer,"REBOOT_CAUSE_APPLICATION_AT") != NULL)
        {
          TPB21.Start_Process = AT;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
    case AT:            //同步波特率
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWNMI0;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
    case MLWNMI0:                 //禁用接收消息指示
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = GETNBAND;

          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
          Create_Timer(ONCE,20,
                       TPB21_Delay_CallBack,0,PROCESS); 
        }  
      }
      break;
    case GETNBAND:               //查询频段
      {       
        if(strstr(Uart2.R_Buffer,"+NBAND:5") != NULL)    //支持频段5
        {
          TPB21.Start_Process = GETCFUN;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
    case GETCFUN:                //查询电话功能
      {
        if(strstr(Uart2.R_Buffer,"+CFUN:1") != NULL)     //全功能
        {         
          TPB21.Start_Process = CGSN;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      } 
      break;
    case CGSN:         // 查询IMEI
      {
        str = strstr(Uart2.R_Buffer,"+CGSN");
        if( str != NULL)//获取到IMEI
        {
          TPB21.Start_Process = CCID;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
     case CCID:          //查询CCID
      {
        str = strstr(Uart2.R_Buffer,"+NCCID");
        if( str != NULL)
        {
          TPB21.Start_Process = CSQ;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
    case CSQ:           //查询信号强度
      {
        str = strstr(Uart2.R_Buffer,"+CSQ");
        if( str != NULL)
        {
          TPB21.Rssi =0;//保存信号强度
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
            TPB21.Incident_Pend = TRUE;//标记挂起
            Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
          }
        }
      }
      break;
    case GETCGATT:       //查询网络激活状态
      {
        if( strstr(Uart2.R_Buffer,"+CGATT:1") != NULL)//网络激活
        {        
          TPB21.Start_Process = CEREG;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }       
      }
      break;
    case CEREG:       //查询网络注册状态
      {
        if( strstr(Uart2.R_Buffer,"+CEREG:0,1") != NULL)//网络注册
        {        
          TPB21.Start_Process = CCLK;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;   
    case CCLK:       //查询实时时间
      {
        str = strstr(Uart2.R_Buffer,"+CCLK:");
        if( str != NULL)
        {  
          GMT_to_BT((unsigned char*)str);
        }
        TPB21.Start_Process = GETNCDP;
        TPB21.Incident_Pend = TRUE;//标记挂起
        Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
      }
      break;   
    case  GETNCDP:                 //查询CDP服务器
      {
        if( strstr(Uart2.R_Buffer,"+NCDP:180.101.147.115,5683") != NULL)//获取到NCDP
        {        
          TPB21.Start_Process = MLWSMI;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
        else
        {        
          TPB21.Start_Process = SETNCDP;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }
      }
      break;
    case  SETNCDP:                 //设置CDP服务器 
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = GETNCDP;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }  
      }
      break;
    case MLWSMI:                 //设置发送消息指示
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWNMI1;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }  
      }
      break;
    case MLWNMI1:                 //设置接收消息指示
      {
        if(strstr(Uart2.R_Buffer,"OK") != NULL)
        {         
          TPB21.Start_Process = MLWULDATA;
          TPB21.Incident_Pend = TRUE;//标记挂起
          Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除超时回调
        }  
      }
      break;
    case MLWULDATA:        //发送消息
      {  
        str1 = strstr(Uart2.R_Buffer,"+MLWDLDATA:"); 
        //处理消息粘包
        while(str1 != NULL)
        { 
          str2 = strstr(str1+6,"+MLWDLDATA:"); 
          if(strnstr(str1,"+MLWDLDATA:4,AAAA0002",24) != NULL)     //上报历史数据的响应
          {
            TPB21.Report_Bit = 0;
            TPB21.Incident_Pend = TRUE;//标记挂起
            Delete_Timer(TPB21_Recv_Timeout_CallBack);//删除接收超时回调
          }
          else if(strnstr(str1,",FFFE",24) != NULL)           //升级相关命令
          { 
            Delete_Timer(MCU_DeInit);//删除超时回调
            Upgrade_Recv_Process((unsigned char*)str1);
          }
          
          str1 = str2;
        }
      }
      break;
    case TPB21_CONNECT_ERROR:
      //添加异常处理
      break;
    default:
      break;
    }
    
    memset(Uart2.R_Buffer,'\0',Uart2.Receive_Length);//清接收缓冲区
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
void TPB21_Recv_Timeout_CallBack(void)//启动超时重发
{
  if(TPB21.Err_Conner.Connect != 0)//判断次数是否超时
  {
    TPB21.Incident_Pend = TRUE;
    TPB21.Err_Conner.Connect--;
  }
  else
  {   
    TPB21.Incident_Pend = TRUE;
    TPB21.Start_Process = TPB21_CONNECT_ERROR;//启动错误
  }
}
/*********************************************************************************
 Function:      //
 Description:   //延时回调函数
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void TPB21_Delay_CallBack(void)
{
  TPB21.Incident_Pend = TRUE;//标记挂起
}
/*********************************************************************************
 Function:      //
 Description:   //上报历史数据
 Input:         //
                //
 Output:        //
 Return:        //
 Others:        //
*********************************************************************************/
void Report_History_Data(void)
{
  uint8_t data[50] = "AT+MLWULDATA=12,080000000000000000000000\r\n";

   //获取时间   
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
  
  //累积流量
  data[18] = Int_to_ASCLL(Cal.Water_Data.flow8[0]/0x10);
  data[19] = Int_to_ASCLL(Cal.Water_Data.flow8[0]%0x10);
  data[20] = Int_to_ASCLL(Cal.Water_Data.flow8[1]/0x10);
  data[21] = Int_to_ASCLL(Cal.Water_Data.flow8[1]%0x10);
  data[22] = Int_to_ASCLL(Cal.Water_Data.flow8[2]/0x10);
  data[23] = Int_to_ASCLL(Cal.Water_Data.flow8[2]%0x10);
  data[24] = Int_to_ASCLL(Cal.Water_Data.flow8[3]/0x10);
  data[25] = Int_to_ASCLL(Cal.Water_Data.flow8[3]%0x10);
  //年
  data[26] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)/0x1000);
  data[27] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x1000/0x100);
  data[28] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x100/0x10);
  data[29] = Int_to_ASCLL((RTC_DateStr.RTC_Year+2000)%0x10); 
  //月
  data[30] = Int_to_ASCLL(RTC_DateStr.RTC_Month/0x10);
  data[31] = Int_to_ASCLL(RTC_DateStr.RTC_Month%0x10);
  //日
  data[32] = Int_to_ASCLL(RTC_DateStr.RTC_Date/0x10);
  data[33] = Int_to_ASCLL(RTC_DateStr.RTC_Date%0x10);
  //时
  data[34] = Int_to_ASCLL(RTC_TimeStr.RTC_Hours/0x10);
  data[35] = Int_to_ASCLL(RTC_TimeStr.RTC_Hours%0x10);
  //分
  data[36] = Int_to_ASCLL(RTC_TimeStr.RTC_Minutes/0x10);
  data[37] = Int_to_ASCLL(RTC_TimeStr.RTC_Minutes%0x10);
  //秒
  data[38] = 0x30;
  data[39] = 0x30;
  
  TPB21_Data_Send(data,42);
}
/******************************************END********************************************************/