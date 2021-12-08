#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"
#include "usmart.h"   
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "motor.h"
#include "servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "chassis.h"
#include "gimbal.h"
#include "refloat.h"

//空闲任务任务堆栈
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
//空闲任务控制块
static StaticTask_t IdleTaskTCB;

//定时器服务任务堆栈
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
//定时器服务任务控制块
static StaticTask_t TimerTaskTCB;

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务堆栈
StackType_t StartTaskStack[START_STK_SIZE];
//任务控制块
StaticTask_t StartTaskTCB;
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务堆栈
StackType_t Task1TaskStack[TASK1_STK_SIZE];
//任务控制块
StaticTask_t Task1TaskTCB;
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		3
//任务堆栈大小	
#define TASK2_STK_SIZE 		128 
//任务堆栈
StackType_t Task2TaskStack[TASK2_STK_SIZE];
//任务控制块
StaticTask_t Task2TaskTCB;
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);
//获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
//静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
//有用户来提供，FreeRTOS提供了接口函数vApplicationGetIdleTaskMemory()
//实现此函数即可。
//ppxIdleTaskTCBBuffer:任务控制块内存
//ppxIdleTaskStackBuffer:任务堆栈内存
//pulIdleTaskStackSize:任务堆栈大小
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}

//获取定时器服务任务的任务堆栈和任务控制块内存
//ppxTimerTaskTCBBuffer:任务控制块内存
//ppxTimerTaskStackBuffer:任务堆栈内存
//pulTimerTaskStackSize:任务堆栈大小
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
	*ppxTimerTaskStackBuffer=TimerTaskStack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

int main(void)
 {	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为500000
	delay_init();	//延时初始化 
	usmart_dev.init(72);		//初始化USMART
	MPU_Init();					//初始化MPU6050
	chassis_Init();
 	gimbal_Init();	//电机初始化
	 LED_Init();
	REFLOAT_Init();
	 
	 //创建开始任务
	StartTask_Handler=xTaskCreateStatic((TaskFunction_t	)start_task,		//任务函数
										(const char* 	)"start_task",		//任务名称
										(uint32_t 		)START_STK_SIZE,	//任务堆栈大小
										(void* 		  	)NULL,				//传递给任务函数的参数
										(UBaseType_t 	)START_TASK_PRIO, 	//任务优先级
										(StackType_t*   )StartTaskStack,	//任务堆栈
										(StaticTask_t*  )&StartTaskTCB);	//任务控制块              
    vTaskStartScheduler();          //开启任务调度
 }
 
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
	Task1Task_Handler=xTaskCreateStatic((TaskFunction_t	)task1_task,		
										(const char* 	)"task1_task",		
										(uint32_t 		)TASK1_STK_SIZE,	
										(void* 		  	)NULL,				
										(UBaseType_t 	)TASK1_TASK_PRIO, 	
										(StackType_t*   )Task1TaskStack,	
										(StaticTask_t*  )&Task1TaskTCB);	
    //创建TASK2任务
	Task2Task_Handler=xTaskCreateStatic((TaskFunction_t	)task2_task,		
										(const char* 	)"task2_task",		
										(uint32_t 		)TASK2_STK_SIZE,	
										(void* 		  	)NULL,				
										(UBaseType_t 	)TASK2_TASK_PRIO, 	
										(StackType_t*   )Task2TaskStack,	
										(StaticTask_t*  )&Task2TaskTCB);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	while(1){
		
		
		if(USART_RX_STA&0x8000)
			{			
				
			
		
				switch (USART_RX_BUF[0])
				{
					case 49://如果接收到指令1
						GPIO_SetBits(GPIOG,GPIO_Pin_11);	//打开灯1关闭灯2
						GPIO_ResetBits(GPIOG,GPIO_Pin_13);	
						vTaskDelay(3000);
						GPIO_ResetBits(GPIOG,GPIO_Pin_11);	
						GPIO_ResetBits(GPIOG,GPIO_Pin_13);	
						break;
					case 50://如果接收到指令2
						GPIO_SetBits(GPIOG,GPIO_Pin_13);	//打开灯2关闭灯1
						GPIO_ResetBits(GPIOG,GPIO_Pin_11);	
						vTaskDelay(3000);
						GPIO_ResetBits(GPIOG,GPIO_Pin_13);	
						GPIO_ResetBits(GPIOG,GPIO_Pin_11);
						break;
				
				}
				USART_RX_STA=0;//标志位清零
				
				
			}
	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	while(1){
		
		printf("7.8$33");//打印ph值和温度给树莓派
		vTaskDelay(5000);  
		
	}
}

//vTaskDelay(1000);  
