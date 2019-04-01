#include "init.h"


u8 ctrl_Init(void)
{
	u8 status = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
//	usmart_dev.init(72);		//��ʼ��USMART		
	LED_Init();         //��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   		//��ʼ��LCD   
	KEY_Init();					//��ʼ������
	usart3_init(115200);		//��ʼ������3 
	
	LED0_on();
	LED1_on();
	
	
	LCD_ShowString(30,20,200,16,16,"STM32F1 ^_^");	  
	LCD_ShowString(30,40,200,16,16,"GPS + BL + GPRS TEST");	
	LCD_ShowString(30,60,200,16,16,"Xuesong Bai 2019.03");
	//LCD_ShowString(30,80,200,16,16,"KEY0:Upload NMEA Data SW");   	 										   	   
  //LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:ON"); 
	
	
	/*------------GPRS + Bluetooth--------------*/
	do
	{
		status = BL_GPRS_Init();
	}while(status==1);
	
	/*--------------Watch Dog-------------------*/
	IWDG_Init(4,2875);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s	  
	
	/*--------------------GPS-------------------*/
	uart_init(38400);	 //���ڳ�ʼ��Ϊ38400
	gps_Init();
	
	IWDG_Feed();
	/*-------------------END--------------------*/
	status = 1;
	LED0_off();
	LED1_off();
	return status;
}
