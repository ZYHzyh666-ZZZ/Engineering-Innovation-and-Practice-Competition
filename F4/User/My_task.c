#include "ALL_include.h"

/*      任何曲柄和任务函数      */
TaskHandle_t StartTask_Handler;             //开始任务（创建所有任务）
void start_task(void *pvParameters);


TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);


TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);


TaskHandle_t Basic_Handler;             	//基础外设
void Basic_task(void *pvParameters);


TaskHandle_t User_Handler;            		//主任务程序
void User_task(void *pvParameters);


TaskHandle_t Systeam_Handler;             	//坐标系转化更新
void Systeam_task(void *pvParameters);

//user_control任务周期 单位ms
#define mission_period		5
TimerHandle_t 	user_200Hz_Handle;			
void user_200Hz(TimerHandle_t xTimer); 	


TaskHandle_t ANO_Handler;             		//ANO外设创建
void ANO_task(void *pvParameters);

TaskHandle_t USART_Handler;             	//串口数据更新
void USART_task(void *pvParameters);

TaskHandle_t BAT_Handler;             	//电池电压检测
void BAT_task(void *pvParameters);

TaskHandle_t ANO_Exchange_Handler;             	//外部传感器数据处理任务
void ANO_Exchange_task(void *pvParameters);

TaskHandle_t FC_State_Handler;             	//飞控状态更新任务
void FC_State_task(void *pvParameters);

TaskHandle_t RC_Handler;             	//遥控数据处理任务
void RC_task(void *pvParameters);

TaskHandle_t ESC_Out_Handler;             	//电调输出任务
void ESC_Out_task(void *pvParameters);

TaskHandle_t Gstation_Handler;             	//地面站同步任务
void Gstation_task(void *pvParameters);
/*---------------------------------*/
u32 times;
double Work_time = 0;

u16 User_times_count; 

/*---------------------------------*/

//空闲任务钩子函数
void vApplicationIdleHook(void)
{
	
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	  //创建软件周期定时器
    user_200Hz_Handle = xTimerCreate( (const char*		)"user_200Hz",
                                    (TickType_t			)mission_period,
                                    (UBaseType_t		)pdTRUE,
                                    (void*				)1,//ID号
							        (TimerCallbackFunction_t)user_200Hz); //周期定时器，周期5ms(5个时钟节拍)，周期
    //创建ANO相关外设任务
    xTaskCreate((TaskFunction_t )ANO_task,             
                (const char*    )"ANO_task",           
                (uint16_t       )ANO_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ANO_TASK_PRIO,        
                (TaskHandle_t*  )&ANO_Handler);   
	//创建基础外设任务
	xTaskCreate((TaskFunction_t )Basic_task,     
				(const char*    )"Basic_task",   
				(uint16_t       )Basic_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )Basic_TASK_PRIO,
				(TaskHandle_t*  )&Basic_Handler); 
	//创建主任务调用程序
	xTaskCreate((TaskFunction_t )User_task,     
				(const char*    )"User_task",   
				(uint16_t       )User_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )User_TASK_PRIO,
				(TaskHandle_t*  )&User_Handler); 


    MY_BEEP_ON(10);

	xTimerStart(user_200Hz_Handle,0);	//开启周期定时器
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
/**
  * 函数作用：  ANO相关函数创建回调函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的外设调用函数，优先级 2
  * 更新日期：   2026-1-22
  */
void ANO_task(void *pvParameters)//
{
	taskENTER_CRITICAL();           //进入临界区
	//创建USART任务
    xTaskCreate((TaskFunction_t )USART_task,             
                (const char*    )"USART_task",           
                (uint16_t       )USART_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )USART_TASK_PRIO,        
                (TaskHandle_t*  )&USART_Handler); 
	//创建BAT任务
    xTaskCreate((TaskFunction_t )BAT_task,             
                (const char*    )"BAT_task",           
                (uint16_t       )BAT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )BAT_TASK_PRIO,        
                (TaskHandle_t*  )&BAT_Handler); 
    //创建外部传感器数据处理任务
    xTaskCreate((TaskFunction_t )ANO_Exchange_task,             
                (const char*    )"ANO_Exchange_task",           
                (uint16_t       )ANO_Exchange_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ANO_Exchange_TASK_PRIO,        
                (TaskHandle_t*  )&ANO_Exchange_Handler); 
    //创建飞控状态更新任务
    xTaskCreate((TaskFunction_t )FC_State_task,             
                (const char*    )"FC_State_task",           
                (uint16_t       )FC_State_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )FC_State_TASK_PRIO,        
                (TaskHandle_t*  )&FC_State_Handler); 
    //创建遥控数据处理任务
    xTaskCreate((TaskFunction_t )RC_task,             
                (const char*    )"RC_task",           
                (uint16_t       )RC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )RC_TASK_PRIO,        
                (TaskHandle_t*  )&RC_Handler); 
    //创建电调输出任务
    xTaskCreate((TaskFunction_t )ESC_Out_task,             
                (const char*    )"ESC_Out_task",           
                (uint16_t       )ESC_Out_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ESC_Out_TASK_PRIO,        
                (TaskHandle_t*  )&ESC_Out_Handler); 
    //创建地面站同步任务
    xTaskCreate((TaskFunction_t )Gstation_task,             
                (const char*    )"Gstation_task",           
                (uint16_t       )Gstation_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )Gstation_TASK_PRIO,        
                (TaskHandle_t*  )&Gstation_Handler); 
    vTaskDelete(ANO_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/**
  * 函数作用：  基础外设调用回调函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的外设调用函数，优先级 1
  * 更新日期：   2026-1-22
  */
void Basic_task(void *pvParameters)
{
    MY_Lazer_1_ON(0);
    MY_Lazer_2_ON(2);
	LAZER_1_ON;
	while(1)
    {		
        KEY_updata();							//MY_key  
		MY_beep_20ms();			                //MY_beep 
		MY_led_light(5);                        //MY_LED（RGB）  
//        MY_Lazer_1_updata(5);                 //MY_Lazer(LED闪烁)   
		MY_Lazer_2_updata(5);                   //MY_Lazer(LED闪烁)   

		Tiva_postion_print((s16)(MY_fly.C_system.x),(s16)(MY_fly.C_system.y),(s16)ano_of.of_alt_cm); 
        vTaskDelay(5);
    }
}


/**
  * 函数作用：  坐标系转化回调函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的外设调用函数，优先级 10
  * 更新日期：   2026-1-22
  */
void Systeam_task(void *pvParameters)
{
    while(1)
    {
        // Oblique_flight(30,10);

        vTaskDelay(5);
    }
}

/**
  * 函数作用：  USART更新回调函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      解析串口数据任务，优先级 15
  * 更新日期：   2026-1-22
  */
void USART_task(void *pvParameters)
{	
	while(1)
    {		
        //发送数据
        ANO_LX_Data_Exchange_Task(0.001f);

        //接收数据
		drvU1DataCheck();
		drvU2DataCheck();
		// drvU3DataCheck();//sbus
		drvU4DataCheck();
		drvU5DataCheck();
		drvU6DataCheck();
        vTaskDelay(1);
    }
}

/**
  * 函数作用：  电池电压检测函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      读取电池电压数据任务，优先级 15
  * 更新日期：   2026-4-26
  */
void BAT_task(void *pvParameters)
{	
    uint16_t Bat_data;
	while(1)
    {		
		INA226_Read(&Bat_data);
        vTaskDelay(100);
    }
}

/**
  * 函数作用：  飞控状态更新函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      更新飞控状态数据任务，优先级 15
  * 更新日期：   2026-4-26
  */
void FC_State_task(void *pvParameters)
{	
	while(1)
    {		
		LX_FC_State_Task(0.01f);
        vTaskDelay(10);
    }
}

/**
  * 函数作用：  遥控器数据更新函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      更新遥控器数据任务，优先级 15
  * 更新日期：   2026-4-26
  */
void RC_task(void *pvParameters)
{	
	while(1)
    {		
		//遥控输入
		DrvRcInputTask(0.01f);
		//遥控数据处理
		RC_Data_Task(0.01f);

        vTaskDelay(10);
    }
}

/**
  * 函数作用：  电调输出函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      更新电调输出任务，优先级 16
  * 更新日期：   2026-4-26
  */
void ESC_Out_task(void *pvParameters)
{	
	while(1)
    {		
	    ESC_Output(1); //unlocked

        vTaskDelay(1);
    }
}

static float ANO_work[3] = {0};

static u8 flag = 0, green_last = 0;

/**
  * 函数作用：  外部传感器数据处理函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      处理外部传感器数据任务，优先级 10
  * 更新日期：   2026-4-26
  */
void ANO_Exchange_task(void *pvParameters)
{	
    u8 times_count = 0;
	while(1)
    {		
        if(times_count++ == 10)
        {
            AnoOF_Check_State(0.01f);
            if(flag == 1)
                Radar_work_judgment(0.01f);
            times_count = 0;
        }
        LX_FC_EXT_Sensor_Task(0.001f);

        LED_1ms_DRV();                          //凌霄LED同步
        vTaskDelay(1);
    }
}


/**
  * 函数作用：  主任务调用回调函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的主任务调用，优先级 20
  * 更新日期：   2026-1-22
  */
void User_task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
	create_task_1();// 创建工作任务1
	create_task_2();// 创建工作任务2

	while(1)
    {		
        task_run(mission_period);               //任务运行
        // fly_velocity_updata();
		// MY_HMI.HMI_time += mission_period;

		Work_time = Work_time + ((float)mission_period)/1000;
		MY_work_time_update(((float)mission_period)/1000);
		MY_System_transformation(((float)mission_period)/1000);

        vTaskDelayUntil(&xLastWakeTime,mission_period);//绝对5ms
    }
}

/**
  * 函数作用：  地面站状态更新函数
  * 参数1：     参数句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的主任务调用，优先级 10
  * 更新日期：   2026-7-29
  */
void Gstation_task(void *pvParameters)
{
    while(1)
    {
            // MY_HMI_print("*P2N01%4d!",((int)Radar.Location_Y + _Drone_Location_begin/1000)*1000+(_Drone_Location_begin%1000 - (int)Radar.Location_X));

        Print_Gstation();
        vTaskDelay(100);
    }
}


/**
  * 函数作用：  5ms周期定时器回调函数
  * 参数1：     定时器句柄
  * 返回值：    无
  * 
  * 备注：      基于RTOS的定时器
  * 更新日期：   2025-11-27
  */

// static s16 location_out = 0;
// static s16 width_out = 0;

void user_200Hz(TimerHandle_t xTimer)
{
    if(User_times_count++ == 200)//1000ms
    {
        User_times_count = 0;
        // MAP_updata(1);

        // MY_Lazer_2_ON(2);
    }
    if(User_times_count % 20 == 0)//100ms
    {
//		Print_Gstation();

        ANO_DT_Send_MY_DATA(0xF2,5, ano_of.of1_dx,
                                    ano_of.of1_dy,
                                    ano_of.of_alt_cm,
                                    (int)Radar.Location_Z - ano_of.of_alt_cm + 7,
                                    position_target_z
                            );	    
    }

    if(ANO_work[2] > 0.09f && flag == 0)
    {
        GPIO_SetBits(Radar_GPIO_Work, Radar_Pin_Work);
        ANO_DT_SendString(1,"Radar_open");
        // Printf_uart2("Radar_open\n");
        flag = 1;
    }
    if(flag == 0)
    {
        if(led.brightness[1] > 0 && green_last == 0)//绿灯由灭到亮，记录工作时间
        {
            ANO_work[0] = Work_time;
        }
        else if(led.brightness[1] == 0 && green_last > 0)//
        {
            ANO_work[1] = Work_time;
            ANO_work[2] = ANO_work[1] - ANO_work[0];
        }
        green_last = led.brightness[1];
    }
}

