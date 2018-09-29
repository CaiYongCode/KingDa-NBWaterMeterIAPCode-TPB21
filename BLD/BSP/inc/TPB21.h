#ifndef __TPB21_H__
#define __TPB21_H__
/*********************************************************************************************************
�ļ�������
*********************************************************************************************************/
#include "include.h"
/*********************************************************************************************************
�궨����
*********************************************************************************************************/
#define         RECV_BUFF_SIZE               1100//���ջ�������С
#define         SEND_BUFF_SIZE               128//���ͻ�������С
#define         CONNECT_ERROR_NUM            3  //���Ӵ������
#define         SEND_ERROR_NUM               3  //���ʹ������

#define         TPB21_POWER_UP_TIME           5//TPB21�ϵ�ʱ�� ��λms
#define         TPB21_R_TIMEROUT_TIME         5 //���ճ�ʱ ��λms

#define         TPB21_S_TIMEROUT_TIME         6000 //�������ݳ�ʱ ��λms

/*********************************************************************************************************
�������Ͷ���
*********************************************************************************************************/
struct Error_Connter_Str
{
  unsigned char Send;
  unsigned char Connect;
};
 

enum Start_Process_En   //��������
{
  TPB21_RECONNECT,        //����
  TPB21_POWER_DOWN,      //����
  TPB21_POWER_UP,        //�ϵ�
  
  NRB,                  //����
  AT,                   //ͬ��������
  GETNBAND,             //��ѯƵ��
  SETNBAND,             //����Ƶ��
  GETCFUN,              //��ѯ�绰����
  SETCFUN,              //���õ绰����
  CIMI,                 //��ѯIMSI
  CGSN,                 //��ѯIMEI  
  CCID,                //��ѯCCID
  CSQ,                  //��ѯ�ź�ǿ��
  GETCGDCONT,           //��ѯAPN
  SETCGDCONT,           //����APN  
  GETCGATT,             //��ѯ���缤��״̬
  SETCGATT,             //��������
  CEREG,                //��ѯ����ע��״̬
  CCLK,                 //��ѯʵʱʱ��
  GETNCDP,              //��ѯCDP������
  SETNCDP,              //����CDP������
  MLWSMI,                 //���÷�����Ϣָʾ
  MLWNMI,                 //���ý�����Ϣָʾ
  MLWULDATA,              //������Ϣ
  MLWQMGR,                //��ѯ��Ϣ����
  MLWMGR,                 //������Ϣ
  TPB21_CONNECT_ERROR,      //��������
};
  
struct TPB21_Str//TPB21 �ܽṹ��
{
//  char R_Buffer[RECV_BUFF_SIZE];        //���ջ�����
//  unsigned short RecvLength;         //���ճ���
  enum Start_Process_En Start_Process;  //���ӽ���
  u8 Report_Bit;                        //����λ
  struct Error_Connter_Str Err_Conner; //�������
  unsigned char ErrorRecord;          //�����¼
  unsigned char Rssi;                 //�ź�ǿ��
  bool Incident_Pend;                  //�¼������־

  unsigned char Reconnect_Times;      //��������
  unsigned char FailTimes;            //ʧ�ܴ���
};
 
/*********************************************************************************************************
�ⲿ����������
*********************************************************************************************************/
extern struct TPB21_Str TPB21;//TPB21 �õļĴ���
/*********************************************************************************************************
����������
*********************************************************************************************************/
void TPB21_Power_On(void); 
void TPB21_Power_Off(void);
void TPB21_Reset(void);
void TPB21_Process(void);

void TPB21_Start(void);
void TPB21_Recv_Timeout_CallBack(void);
void TPB21_Delay_CallBack(void);

void Recv_Data_Process(unsigned char* buff);

void Report_History_Data(void);
void TPB21_Data_Send(unsigned char *Data,unsigned short Len);
/********************************************************************************/
#endif

/******************************************END********************************************************/