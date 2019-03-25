#include "delay.h"
#include "FreeRTOS.h"					//ucos ʹ��	  
#include "task.h" 


static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��
extern void xPortSysTickHandler(void);
//systick�жϷ�����,ʹ��ucosʱ�õ�
void SysTick_Handler(void)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
}


			   
//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/1000000;				//Ϊϵͳʱ�ӵ�1/8  
	reload=SystemCoreClock/1000000;				//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/configTICK_RATE_HZ;		//����delay_ostickspersec�趨���ʱ��
												//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/configTICK_RATE_HZ;			//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 						//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
}								    


//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks=nus*fac_us; 							//��Ҫ�Ľ�����	  		 
	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};							    
}
//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(u16 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if(nms>=fac_ms)							//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			vTaskDelay(nms/fac_ms);		//OS��ʱ
		}
		nms%=fac_ms;							//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));					//��ͨ��ʽ��ʱ  
}

void delay_xms(u32 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);

}
