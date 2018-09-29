/*********************************************************************************
//������
//���ߣ����ƮԾ     ʱ�䣺       �ص㣺
//CPU�ͺţ�         ϵͳ��Ƶ��
//�汾�ţ�V0.0        
*********************************************************************************/
/*********************************************************************************
�ļ�������
*********************************************************************************/
#include "include.h"
#include "Globaldata.h"
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
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
//void MCU_Config(void)
//{
//  RCC_Configuration();
//  GPIO_Configuration();
//  TIM4_Config();
//  USART2_Configuration();
//  USART3_Configuration();
//  ITC_Config();
//  Pulse_Acquire_Config();
//}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void MCU_DeInit(void)
{ 
  USART_DeInit(USART2);                                         //���USART2�Ĵ���
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);    //�ر�USART2ʱ��
  GPIO_Init(GPIOE, GPIO_Pin_4 , GPIO_Mode_Out_PP_Low_Slow);    //USART2 TXD
  
  USART_DeInit(USART3);                                         //���USART3�Ĵ���
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);    //�ر�USART3ʱ��
  GPIO_Init(GPIOE, GPIO_Pin_6 , GPIO_Mode_Out_PP_Low_Slow);    //USART3 TXD
  
  GPIO_Init(GPIOA, GPIO_Pin_4,  GPIO_Mode_Out_PP_Low_Slow);         // ��������
  GPIO_Init(GPIOA, GPIO_Pin_5,  GPIO_Mode_Out_PP_Low_Slow);         // ��������ADC����
  
  GPIO_Init(GPIOE,GPIO_Pin_1,GPIO_Mode_Out_PP_Low_Slow);       //TPB21 ��λ��
  GPIO_Init(GPIOE,GPIO_Pin_2,GPIO_Mode_Out_PP_Low_Slow);        //TPB21 VBAT
  GPIO_Init(GPIOD,GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Slow);        //�̵�
  GPIO_Init(GPIOD,GPIO_Pin_7,GPIO_Mode_Out_PP_Low_Slow);        //�Ƶ�
  
  MeterParameter.DeviceStatus = SLEEP;
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
void RCC_Configuration(void)                                                   //ʱ�ӳ�ʼ��
{
//  CLK_DeInit(); 
//  CLK_HSICmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);                                         //16M�ٶ�

//  CLK_ClockSecuritySystemEnable();//
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
void GPIO_Configuration(void)
{
  GPIO_Init(GPIOA, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow); //GPIO�˿����������
  GPIO_Init(GPIOB, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(GPIOC, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(GPIOD, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(GPIOE, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(GPIOF, GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);
  
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
//void TIM4_Config(void)
//{ 
//  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4 , ENABLE);              //ʹ�ܶ�ʱ��4ʱ��
//  TIM4_TimeBaseInit(TIM4_Prescaler_128 , 125);    //���ö�ʱ��4Ϊ128��Ƶ�����ϼ���������ֵΪ125��Ϊ1����ļ���ֵ
//  TIM4_ITConfig(TIM4_IT_Update , ENABLE);         //ʹ�����ϼ�������ж�
//  TIM4_ARRPreloadConfig(ENABLE);                  //ʹ�ܶ�ʱ��4�Զ����ع���    
//  TIM4_Cmd(ENABLE);                               //������ʱ��4��ʼ����
//}
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:      	//
 Others:        //
*********************************************************************************/
void USART2_Configuration ( void )
{  
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);
  USART_DeInit(USART2);
  GPIO_Init(GPIOE, GPIO_Pin_3 , GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOE, GPIO_Pin_4 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART2,
             9600,
             USART_WordLength_8b,
             USART_StopBits_1,
             USART_Parity_No,
             USART_Mode_TxRx);    
  USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);//�����������ж�
  USART_Cmd(USART2,ENABLE);
  
  ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_2);  //����2���ȼ�
  ITC_SetSoftwarePriority(TIM2_CC_USART2_RX_IRQn, ITC_PriorityLevel_2);              //����2���ȼ�
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
/*********************************************************************************
 Function:      //
 Description:   //
 Input:         //
                //
 Output:        //
 Return:	    	//
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
