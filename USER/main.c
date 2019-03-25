#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "scheduler.h"

#define START_TASK_PRIO 1
#define START_STK_SIZE 128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

int main(void)
 {		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	LED_Init();
  xTaskCreate((TaskFunction_t )start_task,
	            (const char*    )"start_task",
							(uint16_t       )START_STK_SIZE,
							(void*          )NULL,
							(UBaseType_t    )START_TASK_PRIO,
							(TaskHandle_t*  )&StartTask_Handler);
	vTaskStartScheduler();
} 

