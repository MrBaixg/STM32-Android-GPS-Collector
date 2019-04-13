#include "BL_GPRS_Ctrl.h"


u8 Scan_Wtime = 0;//��������ɨ����Ҫ��ʱ��
u8 BT_Scan_mode=0; //����ɨ���豸ģʽ��־

__align(4) u8 dtbuff[50];
//u8 hold_flag = 0;

//extern _xtime UTC_time;
//extern nmea_msg gpsx;


void BL_GPRS_SEND_task(void *pvParameters)
{
    portTickType CurrentControlTick = 0;
    const TickType_t TimeIncrement = pdMS_TO_TICKS(10);
    u8 sendbuf[100];
    u8 sendcnt=0;
    u8 res;
    u8 times = 0;
    float tp1, tp2;
		USART3_RX_STA = 0;
    sim800c_send_cmd("ATE0","OK",0);
		USART3_RX_STA = 0;
    for(;;)
    {
        times++;
        if(times == 100)
        {
            times = 0;
//		CurrentControlTick = xTaskGetTickCount();
//		IWDG_Feed();
            res = sim800c_send_cmd("AT+BTSPPSEND",">",0);//��������

            if(res==1)
            {
                LCD_Fill(30,520,330,540,WHITE);
                LCD_ShowString(30,520,300,16,16,"BTSPPSEND Failed.");
            }
            else
            {
                LCD_Fill(30,520,330,540,WHITE);
            }
//			  sprintf((char*)sendbuf,"Bluetooth test %d \r\n\32",sendcnt);
//			  sendcnt++;
//			  if(sendcnt>99) sendcnt = 0;
            tp1 = gpsx.longitude;
            tp2 = gpsx.latitude;
            sprintf((char*)sendbuf,"T:%i,Lo:%.5f,La:%.5f \r\n\32",Unix_time, tp1/=100000, tp2/=100000);
            res = sim800c_send_cmd((u8*)sendbuf,"OK",0);//��������
            if(res==0)
            {
                LCD_ShowString(30,520,300,16,16,(u8*)sendbuf);//��ʾ���͵�����
            }
            else
            {
                LCD_ShowString(30,520,300,16,16,"BlueTooth Disconnect.");
            }
        }


//		vTaskDelay(10);
        vTaskDelayUntil(&CurrentControlTick, TimeIncrement);
    }

}
void BL_GPRS_REC_task(void *pvParameters)
{
    portTickType CurrentControlTick = 0;
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5);
    u8 *p1,*p2, *p3;
//		u8 time = 0;
//		u8 time2 = 0;

    for(;;)
    {
				
        if(USART3_RX_STA&0x8000)
        {
//						time++;
//						if(time>100) time = 0;
//						sprintf((char *)dtbuff,"%i",time);
//						LCD_ShowString(30,540,300,16,16,dtbuff);
//						sim800c_check_cmd("Data");

            p3 =(u8*)strstr((const char*)USART3_RX_BUF,"DATA: ");
			
            if(p3!=NULL)
            {
//								time2++;
//								if(time2>100) time2 = 0;

                USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����

                sprintf((char *)dtbuff,"%s",USART3_RX_BUF);
                LCD_ShowString(30,600,300,16,16,dtbuff);
                p1 =(u8*)strstr((const char*)USART3_RX_BUF,"DATA: ");
                if(p1!=NULL)
                {
//										LCD_ShowString(30,560,300,16,16,"Receive Data");
                    p2 = (u8*)strstr((const char *)p1,"\x0d\x0a");
                    if(p2!= NULL)
                    {
                        p2 =(u8*)strstr((const char *)p1,",");
                        p1 =(u8*)strstr((const char *)p2+1,",");
                        // printf("���յ��������ǣ�");
                        // printf("%s\r\n",p1+1);//��ӡ������
                        LCD_Fill(30,560,330,580,WHITE);                       //�����ʾ
                        //LCD_ShowString(90,200,150,119,16,(u8*)(p1+1));						//��ʾ���յ�������
                        LCD_ShowString(30,560,300,16,16,(u8*)(p1+1));
                    }
                }
                else
                {
                    p1 =(u8*)strstr((const char*)USART3_RX_BUF,"+BTDISCONN: ");//�ж��Ƿ�Ͽ�����
                    if(p1!=NULL)
                    {
                        LCD_ShowString(30,560,300,16,16,"BlueTooth Disconnect.");
                    }
                }
            }
//						sprintf((char *)dtbuff,"%i",time2);
//						LCD_ShowString(30,560,300,16,16,dtbuff);
						USART3_RX_STA=0;
				}



//		}

//		t += 1;
        //sim_at_response(1);	//���GSMģ�鷢�͹���������
//		if(t>5000)
//		{
//			t = 0;
//		}

        //vTaskDelay(10);
        
        vTaskDelayUntil(&CurrentControlTick, TimeIncrement);
    }

}


u8 connect_BL()
{
    u8 status = 0;
    u8 res;
    USART3_RX_STA = 0;
    if(sim800c_send_cmd("ATE1","OK",200))//�򿪻���ʧ��
    {
        //printf("�򿪻���ʧ��");
        status = 1;
        return status;
    }
    delay_ms(10);
    USART3_RX_STA = 0;
    if(sim800c_send_cmd("AT+BTPOWER=1","AT",0))//��������Դ ���ж�OK����Ϊ��Դԭ�������ٷ��ʹ򿪵Ļ��᷵��error
    {
        sim800c_send_cmd("ATE0","OK",200);//�رջ��Թ���
        USART3_RX_STA = 0;
        status = 1;
        return status;
    }
		USART3_RX_STA = 0;
    res = sim800c_send_cmd("AT+BTDISCONN=1","AT",100);
    do
    {
        USART3_RX_STA = 0;
        res = sim800c_send_cmd("AT+BTUNPAIR=0","AT",100);//ɾ�������Ϣ
//        USART3_RX_STA = 0;
    } while(res);
    do
    {
        LCD_ShowString(30,480,300,16,16,"Waiting for CON request");
        res = sim800c_wait_request("+BTPAIRING:",600);             //�ȴ��ֻ��������������� 6s
        if(res==1)                                                 //�ֻ�����������
        {
            delay_ms(10);
						USART3_RX_STA = 0;
            sim800c_send_cmd("AT+BTPAIR=1,1","BTPAIR:",500);         //��Ӧ����
        }
        LCD_ShowString(30,480,300,16,16,"                         ");
        delay_ms(50);
    } while(strstr((const char*)USART3_RX_BUF,"+BTPAIR: 1")==NULL);//�ж��Ƿ�ƥ��ɹ�
    USART3_RX_STA=0;
    LCD_ShowString(30,480,300,16,16,"Device Connected...");
    LCD_ShowString(30,500,300,16,16,"Waiting for SSP CON");
    do
    {
        res = sim800c_wait_request("SPP",120);            //�ȴ��ֻ���SPP��������
        if(res==2)return 0;                               //����������һ��
        else if(res==1) break;                            //SPP���ӳɹ�
    } while(1);
    USART3_RX_STA = 0;
    if(!sim800c_send_cmd("AT+BTACPT=1","+BTCONNECT:",300))//Ӧ���ֻ���spp�������� 3S
    {
        LCD_Fill(30,500,330,520,WHITE);
        LCD_ShowString(30,500,300,16,16,"SSP connected...");
    }
    else
    {
        LCD_ShowString(30,500,300,16,16,"SSP Failed...");
        status = 2;
    }
    USART3_RX_STA = 0;
    sim800c_send_cmd("ATE1","OK",200);
    USART3_RX_STA = 0;
    //sim800c_send_cmd("ATE0","OK",200);//�رջ��Թ���
    return status;
}

u8 BL_GPRS_Init()
{
    u8 status = 0;
    u8 res;
    POINT_COLOR=BLACK;
    LCD_ShowString(30,100,300,16,16,"GPRS-BlueTooth Setting...");
    USART3_RX_STA = 0;
    while(sim800c_send_cmd("AT","OK",100))
    {
        USART3_RX_STA = 0;
        sim800c_send_cmd("ATE0","OK",200);
        delay_ms(400);
        USART3_RX_STA = 0;
    }
    LCD_ShowString(30,100,300,16,16,"GPRS-BlueTooth Set Done!!");
    USART3_RX_STA = 0;
    sim800c_send_cmd("ATE0","OK",200);//������
    USART3_RX_STA=0;

    delay_ms(50);
    BL_GPRS_Msg_Show();
    do
    {
        USART3_RX_STA = 0;
        res = connect_BL();
        sprintf((char *)dtbuff, "BL result: %i", res);
        LCD_ShowString(30,520,300,16,16,dtbuff);
    } while(res==2);
    if(res==1)
    {
        status = 1;
    }
    USART3_RX_STA = 0;
    sim800c_send_cmd("ATE1","OK",200);
    USART3_RX_STA = 0;
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
    USART3_RX_STA=0;
    if(res==0)	//��ѯ����������
    {
        p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");
        p1[0]=0;//���������
        sprintf((char *)dtbuff,"Manufac:%s",USART3_RX_BUF+2);
        //Show_Str(30,400,300,16,dtbuff,16,0);
        LCD_ShowString(30,400,300,16,16,dtbuff);
        USART3_RX_STA=0;
    }
    delay_ms(10);
    USART3_RX_STA = 0;
    res = sim800c_send_cmd("AT+CGMM","OK",200);
    USART3_RX_STA=0;
    if(res==0)//��ѯģ������
    {
        p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");
        p1[0]=0;//���������
        sprintf((char *)dtbuff,"Model:%s",USART3_RX_BUF+2);
        //Show_Str(30,420,300,16,dtbuff,16,0);
        LCD_ShowString(30,420,300,16,16,dtbuff);
        USART3_RX_STA=0;
    }
    delay_ms(10);
    USART3_RX_STA = 0;
    res = sim800c_send_cmd("AT+CGSN","OK",200);
    USART3_RX_STA=0;
    if(res==0)//��ѯ��Ʒ���к�
    {
        p1=(u8*)strstr((const char*)(USART3_RX_BUF+2),"\r\n");//���һس�
        p1[0]=0;//���������
        sprintf((char *)dtbuff,"Serial:%s",USART3_RX_BUF+2);
        //Show_Str(30,440,300,16,dtbuff,16,0);
        LCD_ShowString(30,440,300,16,16,dtbuff);
        USART3_RX_STA=0;
    }
    do {
        delay_ms(10);
        USART3_RX_STA=0;
        res = sim800c_send_cmd("AT+CPIN?","OK",200);	//��ѯSIM���Ƿ���λ
        sprintf((char *)dtbuff, "SIM result: %i", res);
        LCD_ShowString(30,660,300,16,16,dtbuff);
        USART3_RX_STA=0;
    } while(res == 1);
    do {
        delay_ms(10);
        USART3_RX_STA=0;
        res = sim800c_send_cmd("AT+COPS?","OK",200);	//��ѯ��Ӫ������
        sprintf((char *)dtbuff, "ISP result: %i", res);
        LCD_ShowString(30,680,300,16,16,dtbuff);
        USART3_RX_STA=0;
    } while(res == 1);
    delay_ms(10);
    do {
        delay_ms(10);
        USART3_RX_STA=0;
        res = sim800c_send_cmd("AT+CNUM","+CNUM",200);
        sprintf((char *)dtbuff, "NUM result: %i", res);
        LCD_ShowString(30,680,300,16,16,dtbuff);
        USART3_RX_STA=0;
    } while(res == 1);
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
    u8 *p1;
//	u8 i;
//	USART3_RX_STA=0;
    if((u32)cmd<=0XFF)
    {
        while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������
        USART3->DR=(u32)cmd;
    } else u3_printf("%s\r\n",cmd);//��������
//    LCD_Fill(30, 620, 330, 640, WHITE);
//    sprintf((char *)dtbuff,"%s",cmd);
//    LCD_ShowString(30,620,300,16,16,dtbuff);

    if(waittime==1100)//11s����ش�������(����ɨ��ģʽ)
    {
        Scan_Wtime = 11;//��Ҫ��ʱ��ʱ��
        TIM7_SetARR(9999);//����1S��ʱ�ж�

    }
    if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
    {
        while(--waittime)	//�ȴ�����ʱ
        {
//		  if(BT_Scan_mode)//����ɨ��ģʽ
//		  {
//			  res=KEY_Scan(0);//������һ��
//			  if(res==WKUP_PRES)return 2;
//		  }
            delay_ms(10);
            if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
            {
                if(sim800c_check_cmd(ack))break;//�õ���Ч����
//			  p1 = (u8*)strstr((const char*)USART3_RX_BUF,"DATA: ");
//				if(p1!=NULL)
//				{
////					for(i = 0;i<USART3_MAX_RECV_LEN;i++)
////					{
////						USART3_RX_BUFF[i] = USART3_RX_BUF[i];
////					}
//					delay_ms(10);
//				}
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
//				LCD_Fill(30, 600, 330, 620, WHITE);
//				sprintf((char *)dtbuff,"%s",USART3_RX_BUF);
//				LCD_ShowString(30,600,300,60,16,dtbuff);
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
    if(request && waittime)
    {
        while(--waittime)
        {
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
