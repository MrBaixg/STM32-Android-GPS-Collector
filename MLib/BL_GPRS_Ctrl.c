#include "BL_GPRS_Ctrl.h"


u8 Scan_Wtime = 0;//��������ɨ����Ҫ��ʱ�� 
u8 BT_Scan_mode=0; //����ɨ���豸ģʽ��־

__align(4) u8 dtbuff[50];


void BL_GPRS_SEND_task(void *pvParameters)
{
	portTickType CurrentControlTick = 0;
	//u16 i, rxlen;
	u16 t = 0;
	//u8 key;
	//int connect;
	
	for(;;)
	{
		CurrentControlTick = xTaskGetTickCount();
		t += 1;
		//sim_at_response(1);	//���GSMģ�鷢�͹���������
		if(t>100)
		{
			t = 0;
		}

		//vTaskDelay(10);
		vTaskDelayUntil(&CurrentControlTick, 100 / portTICK_RATE_MS);
	}

}
void BL_GPRS_REC_task(void *pvParameters)
{
	portTickType CurrentControlTick = 0;
	//u16 i, rxlen;
	u16 t = 0;
	//u8 key;
	//int connect;
	
	for(;;)
	{
		CurrentControlTick = xTaskGetTickCount();
		t += 1;
		//sim_at_response(1);	//���GSMģ�鷢�͹���������
		if(t>100)
		{
			t = 0;
		}

		//vTaskDelay(10);
		vTaskDelayUntil(&CurrentControlTick, 100 / portTICK_RATE_MS);
	}

}


u8 connect_BL()
{
	u8 status = 0;
	u8 res;
	if(sim800c_send_cmd("ATE1","OK",200))//�򿪻���ʧ��
	 {
		  //printf("�򿪻���ʧ��");
		 status = 1;
		  return status;
	 }
	 delay_ms(10);
	 if(sim800c_send_cmd("AT+BTPOWER=1","AT",300))//��������Դ ���ж�OK����Ϊ��Դԭ�������ٷ��ʹ򿪵Ļ��᷵��error
     {
		  sim800c_send_cmd("ATE0","OK",200);//�رջ��Թ���
		 status = 1;
		  return status;
	 }			
	 sim800c_send_cmd("AT+BTUNPAIR=0","AT",100);//ɾ�������Ϣ				 
	 do
	{		
		LCD_ShowString(30,480,300,16,16,"Waiting for CON request");	   
			   res = sim800c_wait_request("+BTPAIRING:",600);             //�ȴ��ֻ��������������� 6s
			   if(res==1)                                                 //�ֻ�����������
			   {
				 delay_ms(10);
				 sim800c_send_cmd("AT+BTPAIR=1,1","BTPAIR:",500);         //��Ӧ����
			   }
			   else if(res==2) return 0;                                  //����������һ��
			   LCD_ShowString(30,480,300,16,16,"                         ");
			   delay_ms(50);
			}while(strstr((const char*)USART3_RX_BUF,"+BTPAIR: 1")==NULL);//�ж��Ƿ�ƥ��ɹ�
			USART3_RX_STA=0;
			LCD_ShowString(30,480,300,16,16,"Device Connected...");
			LCD_ShowString(30,500,300,16,16,"Waiting for SSP CON");
			do
	    {
			res = sim800c_wait_request("SPP",120);            //�ȴ��ֻ���SPP��������
			if(res==2)return 0;                               //����������һ��
			else if(res==1) break;                            //SPP���ӳɹ�
	    }while(1);
					if(!sim800c_send_cmd("AT+BTACPT=1","+BTCONNECT:",300))//Ӧ���ֻ���spp�������� 3S
	    {
		   LCD_ShowString(30,500,300,16,16,"SSP connected...");
	    }
	    else  
	    {
		   LCD_ShowString(30,500,300,16,16,"SSP Failed...");
		 status = 2;
		  return status;
	    }
	return status;
}

u8 BL_GPRS_Init()
{
	u8 status = 0;
	u8 key=0; 
	u8 res;
	LCD_ShowString(30,100,300,16,16,"GPRS-BlueTooth Setting...");
	while(sim800c_send_cmd("AT","OK",100))
	{
		delay_ms(400);
	}
	LCD_ShowString(30,100,300,16,16,"GPRS-BlueTooth Set Done!!");
	key+=sim800c_send_cmd("ATE0","OK",200);//������
	USART3_RX_STA=0;
	
	delay_ms(50);
	BL_GPRS_Msg_Show();
	do
	{
		res = connect_BL();
		sprintf((char *)dtbuff, "BL result: %i", res);
		LCD_ShowString(30,100,300,16,16,dtbuff);
	}while(res==2);
	if(res==1)
	{
		status = 1;
	}
	return status;
}
void BL_GPRS_Msg_Show()
{
	u8 *p1, *p2;
	u8 res;
	POINT_COLOR=RED;
	LCD_ShowString(30,380,300,16,16,"BlueTooth + GPRS Data:");
	POINT_COLOR=BLUE;
	USART3_RX_STA=0;
	res = sim800c_send_cmd("AT+CGMI","OK",200);
	if(res==0)	//��ѯ����������
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");
		p1[0]=0;//���������
		sprintf((char *)dtbuff,"Manufac:%s",USART3_RX_BUF+2);
		//Show_Str(30,400,300,16,dtbuff,16,0);
		LCD_ShowString(30,400,300,16,16,dtbuff);
		USART3_RX_STA=0;		
	} 
	res = sim800c_send_cmd("AT+CGMM","OK",200);
	if(res==0)//��ѯģ������
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n"); 
		p1[0]=0;//���������
		sprintf((char *)dtbuff,"Model:%s",USART3_RX_BUF+2);
		//Show_Str(30,420,300,16,dtbuff,16,0);
		LCD_ShowString(30,420,300,16,16,dtbuff);
		USART3_RX_STA=0;		
	} 
	res = sim800c_send_cmd("AT+CGSN","OK",200);
	if(res==0)//��ѯ��Ʒ���к�
	{ 
		p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");//���һس�
		p1[0]=0;//��������� 
		sprintf((char *)dtbuff,"Serial:%s",USART3_RX_BUF+2);
		//Show_Str(30,440,300,16,dtbuff,16,0);
		LCD_ShowString(30,440,300,16,16,dtbuff);
		USART3_RX_STA=0;		
	}
	res = sim800c_send_cmd("AT+CNUM","+CNUM",200);
	if(res==0)//��ѯ��������
	{
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+2),"\"");
		p2[0]=0;//���������
		p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
		sprintf((char *)dtbuff,"Phone:%s",p1+2);
		//Show_Str(30,460,300,16,dtbuff,16,0);
		LCD_ShowString(30,460,300,16,16,dtbuff);
		USART3_RX_STA=0;		
	}
}
//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
//void sim_at_response(u8 mode)
//{
//	if(USART3_RX_STA&0X8000)		//���յ�һ��������
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
//		printf("%s",USART3_RX_BUF);	//���͵�����
//		if(mode)USART3_RX_STA=0;
//	} 
//}



//SIM800C��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//��������
	
	if(waittime==1100)//11s����ش�������(����ɨ��ģʽ)
	{
		 Scan_Wtime = 11;//��Ҫ��ʱ��ʱ��
		 TIM7_SetARR(9999);//����1S��ʱ�ж�
		 
	}
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
	   while(--waittime)	//�ȴ�����ʱ
	   { 
		  if(BT_Scan_mode)//����ɨ��ģʽ
		  {  
			  res=KEY_Scan(0);//������һ��
			  if(res==WKUP_PRES)return 2;
		  }
		  delay_ms(10);
		  if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		  {
			  if(sim800c_check_cmd(ack))break;//�õ���Ч���� 
			  USART3_RX_STA=0;
		  } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 
u8* sim800c_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

//����SIM800C�������ݣ���������ģʽ��ʹ�ã�
//request:�ڴ����������ַ���
//waittimg:�ȴ�ʱ��(��λ��10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim800c_wait_request(u8 *request ,u16 waittime)
{
	 u8 res = 1;
	 u8 key;
	 if(request && waittime)
	 {
	    while(--waittime)
		{   
		   key=KEY_Scan(0);
		   if(key==WKUP_PRES) return 2;//������һ��
		   delay_ms(10);
		   if(USART3_RX_STA &0x8000)//���յ��ڴ���Ӧ����
		   {
			  if(sim800c_check_cmd(request)) break;//�õ���Ч����
			  USART3_RX_STA=0;
		   }
		}
		if(waittime==0)res=0;
	 }
	 return res;
}
