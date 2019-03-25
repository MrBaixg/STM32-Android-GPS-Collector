#include "init.h"


u8 ctrl_Init(void)
{
	int status = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	usmart_dev.init(72);		//��ʼ��USMART		
	LED_Init();         //��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   		//��ʼ��LCD   
	KEY_Init();					//��ʼ������
	usart3_init(38400);		//��ʼ������3 
	LED0_on();
	LED1_on();
	
	
	LCD_ShowString(30,20,200,16,16,"ALIENTEK STM32F1 ^_^");	  
	LCD_ShowString(30,40,200,16,16,"S1216F8 GPS TEST");	
	LCD_ShowString(30,60,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,80,200,16,16,"KEY0:Upload NMEA Data SW");   	 										   	   
  LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:ON"); 
	
	/*--------------Watch Dog-------------------*/
	IWDG_Init(4,2875);    //���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s	  
	
	/*--------------------GPS-------------------*/
	if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ. 
	{
   	LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
		do
		{
			usart3_init(9600);			//��ʼ������3������Ϊ9600
	  	SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
			usart3_init(38400);			//��ʼ������3������Ϊ38400
      SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
		}while(SkyTra_Cfg_Rate(5)!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
	  LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Set Done!!");
		delay_ms(10);
		LCD_Fill(30,120,30+200,120+16,WHITE);//�����ʾ 
	}
	
	IWDG_Feed();
	/*------------GPRS + Bluetooth--------------*/
	
	
	
	/*-------------------END--------------------*/
	status = 1;
	LED0_off();
	LED1_off();
	return status;
}
