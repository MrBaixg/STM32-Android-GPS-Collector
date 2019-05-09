#include "GPS_Ctrl.h"

u8 TX_BUF[USART2_MAX_RECV_LEN];
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
unsigned int Unix_time;
_xtime UTC_time;
const u8*fixmode_tbl[4]= {"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ���

int status_gps = 0;

//��ʾGPS��λ��Ϣ
void Gps_Msg_Show(void)
{
    float tp;
    POINT_COLOR=RED;
    LCD_ShowString(30,120,200,16,16,"GPS Data:");
    POINT_COLOR=BLUE;
    tp=gpsx.longitude;
    sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
    LCD_ShowString(30,140,200,16,16,dtbuf);
    tp=gpsx.latitude;
    sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
    LCD_ShowString(30,160,200,16,16,dtbuf);
    tp=gpsx.altitude;
    sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
    LCD_ShowString(30,180,200,16,16,dtbuf);
    tp=gpsx.speed;
    sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���
    LCD_ShowString(30,200,200,16,16,dtbuf);
		tp=gpsx.direction;
		sprintf((char *)dtbuf,"Direction:%.1f   ",tp/=1000);	//��ʾ����
    LCD_ShowString(30,220,200,16,16,dtbuf);
    if(gpsx.fixmode<=3)														//��λ״̬
    {
        sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);
        LCD_ShowString(30,240,200,16,16,dtbuf);
    }
    sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��GPS������
    LCD_ShowString(30,260,200,16,16,dtbuf);
    sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�GPS������
    LCD_ShowString(30,280,200,16,16,dtbuf);

    sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//�ɼ�����������
    LCD_ShowString(30,300,200,16,16,dtbuf);
		
    sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
    LCD_ShowString(30,320,200,16,16,dtbuf);
    sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
    LCD_ShowString(30,340,200,16,16,dtbuf);

		sprintf((char *)dtbuf,"UNIX Time:%d   ", Unix_time);	//��ʾUTCʱ��
    LCD_ShowString(30,360,200,16,16,dtbuf);
}




void GPS_task(void *pvParameters)
{
    portTickType CurrentControlTick = 0;
    const TickType_t TimeIncrement = pdMS_TO_TICKS(50);
    u16 i, rxlen;
    u16 t = 0;


    for(;;)
    {
//		CurrentControlTick = xTaskGetTickCount();
        t += 1;
        if(USART2_RX_STA&(1<<15))		//���յ�һ��������
        {
            t = 0;
						IWDG_Feed();
            status_gps = 0;
            rxlen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
            for(i=0; i<rxlen; i++)TX_BUF[i]=USART2_RX_BUF[i];
            USART2_RX_STA=0;		   	//������һ�ν���
            TX_BUF[i]=0;			//�Զ���ӽ�����
            GPS_Analysis(&gpsx,(u8*)TX_BUF);//�����ַ���
            time_transfer();
            Gps_Msg_Show();				//��ʾ��Ϣ
        }
        if(t>1000)
        {
            status_gps = 1;
        }
        //vTaskDelay(10);
        vTaskDelayUntil(&CurrentControlTick, TimeIncrement);
    }
}

void gps_Init(void)
{
    u8 flag, flag1;
    POINT_COLOR=BLACK;
    flag = SkyTra_Cfg_Rate(5);
    if(flag!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ.
    {
        LCD_ShowString(30,80,200,16,16,"SkyTraF8-BD Setting...");
        do
        {
            usart2_init(9600);			//��ʼ������3������Ϊ9600
            flag1 = SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
            usart2_init(38400);			//��ʼ������3������Ϊ38400
            SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
            flag = SkyTra_Cfg_Rate(5);
        } while(flag!=0&&flag1!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
        //LCD_Fill(30,120,30+200,120+16,WHITE);//�����ʾ
    }
//	if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ.
//	{
//   	LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
//		do
//		{
//			uart_init(9600);			//��ʼ������3������Ϊ9600
//	  	SkyTra_Cfg_Prt(5);			//��������ģ��Ĳ�����Ϊ38400
//			uart_init(115200);			//��ʼ������3������Ϊ38400
//      flag = SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
//		}while(SkyTra_Cfg_Rate(5)!=0&&flag!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
//	}
    LCD_ShowString(30,80,200,16,16,"SkyTraF8-BD Set Done!!");
}
void time_transfer()
{
    UTC_time.year = gpsx.utc.year;
    UTC_time.month = gpsx.utc.month;
    UTC_time.day = gpsx.utc.date;
    UTC_time.hour = gpsx.utc.hour;
    UTC_time.minute = gpsx.utc.min;
    UTC_time.second = gpsx.utc.sec;

    Unix_time = xDate2Seconds(&UTC_time);
}

