#include "ALL_include.h"

float Yaw_target = 0;
float X_target = 0;
float Y_target = 0;

//抛投任务
My_mission_task task_mission_1[]=
{
    {mission_1_1,2.0f},			//起飞
    {mission_1_2,3.0f},			//悬停3s
    {mission_1_3,2.0f},			//寻找
	{mission_1_4,2.0f},			//锁定（A->B）
	{mission_1_5,2.0f},			//锁定（B->C）
	{mission_1_6,2.0f},			//锁定（C->D）
	{mission_1_7,2.0f},			//返程
	{mission_1_8,3.0f},			//降落
	// {mission_1_9,2.0f},			//
	// {mission_1_10,2.0f},//2
	// {mission_1_11,2.0f},//1
	// {mission_1_12,2.0f},//降落点
	// {mission_1_13,2.0f},//降落
    // {mission_1_14,2.0f},//降落
	// {mission_1_15,2.0f},//降落
	// {mission_1_13,2.0f},//降落
};
My_task_state task_1;
u8 start_task1_flag = 0;		//任务开始标志

//动态起降
My_mission_task task_mission_2[]=
{
	{mission_2_1,2.0f},//起飞
	{mission_2_2,3.0f},//旋转
	{mission_2_3,2.0f},//上下
	{mission_2_4,3.0f},//起飞
	{mission_2_5,3.0f},//前后
	{mission_2_6,2.0f},//识别
	// {mission_2_7,4.0f},//打击目标
	// {mission_2_8,2.0f},//打击
};
My_task_state task_2;
u8 start_task2_flag = 0;		//任务开始标志

u8 takeoff_once = 0;			//任务开始第一次标志位

/*-----------------------------*/

s16 postion_begin_x = 0;			//记录起飞坐标_X
s16 postion_begin_y = 0;			//记录起飞坐标_Y

s16 work_flag = 0;
/**
  * 函数作用：  任务状态检测
  * 参数1：     （u16）系统时间ms
  * 返回值：     无
  * 
  * 备注：      放置任务中调用检测
  * 更新日期：   2025-11-27：无
  * 
  */
void task_check(u16 ms)
{
	if(task_1.finish == 1 || task_2.finish == 1)//完成一次任务音效提示
	{
		MY_BEEP_ON(300);
	}
	if(task_1.finish == 1)
	{
		task_1.finish = 0;
		task_1.mission_times = 0;
		task_1.step++;
	}
	else if(start_task1_flag == 1)				//无锁桨检测
	// else if(start_task1_flag == 1 && fc_sta.unlock_sta == 1)	
		task_1.mission_times = task_1.mission_times + (float)ms/1000;//记录时间

	if(task_2.finish == 1)
	{
		task_2.finish = 0;
		task_2.mission_times = 0;
		task_2.step++;
	}
	else if(start_task2_flag == 1)
		task_2.mission_times = task_2.mission_times + (float)ms/1000;//记录时间
}

/**
  * 函数作用：  任务运行函数
  * 参数1：     （u16）系统时间ms
  * 返回值：     无
  * 
  * 备注：      放置在5ms定时器中调用
  * 更新日期：   2025-11-27：无
  * 
  */
void task_run(u16 ms)//任务运行
{
	task_check(ms);
	if(start_task1_flag == 1)			//任务1开始标志
	{
		if(task_1.step < task_1.task_num)
			task_1.mission[task_1.step].task_mission();
		else
			task_1.step = task_1.task_num;//任务结束
	}

	if(start_task2_flag == 1)			//任务2开始标志
	{
		if(task_2.step < task_2.task_num)
			task_2.mission[task_2.step].task_mission();
		else
			task_2.step = task_2.task_num;//任务结束
	}
}

/**
  * 函数作用：  任务1创建函数
  * 参数1：     无
  * 返回值：    无
  * 
  * 备注：      无
  * 更新日期：   2025-11-27：无
  * 
  */
void create_task_1(void)//创建任务
{
	task_1.mission = task_mission_1;

	task_1.finish = 0;
	task_1.step = 0;
	task_1.task_num = sizeof(task_mission_1)/sizeof(task_mission_1[0]);
	task_1.mission_times = 0;
}

/**
  * 函数作用：  任务2创建函数
  * 参数1：     无
  * 返回值：    无
  * 
  * 备注：      无
  * 更新日期：   2025-11-27：无
  * 
  */
void create_task_2(void)//创建任务
{
	task_2.mission = task_mission_2;

	task_2.finish = 0;
	task_2.step = 0;
	task_2.task_num = sizeof(task_mission_2)/sizeof(task_mission_2[0]);
	task_2.mission_times = 0;
}

void mission_1_1(void)//起飞
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
		Yaw_target = N100.Yaw_100;
		postion_begin_x = Radar.Location_X;
		postion_begin_y = Radar.Location_Y;
		ANO_DT_SendString(1,"mod1_begin");
	}
	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);		

	if(_time_ok(task_1) && ano_of.of_alt_cm > Default_height - 2)//起飞到145cm
	{
		task_1.finish = 1;
		takeoff_once = 0;
//		MY_Camera_print("OK");
		ANO_DT_SendString(2,"145 OK!");
		Move_stop();
		return;
	}
}

void mission_1_2(void)				//悬停
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}
	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);			
	Move_stop();

	if(_time_ok(task_1))
	{
		Move_stop();

		task_1.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"3s OK!");

		return;
	}
}

void mission_1_3(void)						//寻找
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}
	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				

	control_to_B();
	if(Camera.x != 0 && Camera.y != 0 )
		work_flag = 1;

	if(work_flag && _time_ok(task_1))
	{
		task_1.finish = 1;
		takeoff_once = 0;
		Move_stop();
		ANO_DT_SendString(2,"find OK!");
		return;
	}
}

void mission_1_4(void)						//锁定跟随（A->B）
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				

	control_to_follow();

	if(abs(Camera.x - _Camera_middle_X) < 70 && abs(Camera.y - _Camera_middle_Y) < 70 && _time_ok(task_1))
	{
		Release_fun();
		work_flag = 1;
	}
	if(_time_ok(task_1)     || work_flag)
	{
		task_1.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"follow B OK!");
		return;
	}
}

void mission_1_5(void)							//锁定跟随（B->C）
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				

	control_to_follow();
	
	if((_time_ok(task_1) && Car.num == 'C') || work_flag)
	{
		task_1.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"follow C OK!");
		return;
	}
}

void mission_1_6(void)							//锁定跟随（C->D）+ 投放
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				

	control_to_follow();

	if(abs(Camera.x - _Camera_middle_X) < 30 && abs(Camera.y - _Camera_middle_Y) < 30)
		Release_fun();
	
	if((_time_ok(task_1) && Car.num == 'D') || work_flag)
	{
		task_1.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"follow B OK!");
		return;
	}
}

void mission_1_7(void)								//返程
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定

	control_back();
	if(_time_ok(task_1) && abs(_HOME_X - Radar.Location_X) < 10 && abs(_HOME_Y - Radar.Location_Y) < 10)
	{
		task_1.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"back OK!");
		return;
	}
}

void mission_1_8(void)//降落
{
	if(takeoff_once == 0)//
	{
		postion_target_z = 0;
		begin_land = 1;		
		takeoff_once = 1;
		Move_stop();
	}

	Set_High(postion_target_z);
	Program_Ctrl_User_Set_HXYcmps(0,0);
	if(ano_of.of_alt_cm < 10 && _time_ok(task_1))
	{
		task_2.finish = 1;
		takeoff_once = 0;
		FC_Lock();
		ANO_DT_SendString(2,"land OK!");
		return;
	}
}



// void mission_1_1(void)//起飞
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 		Yaw_target = N100.Yaw_100;
// 		postion_begin_x = Radar.Location_X;
// 		postion_begin_y = Radar.Location_Y;
// 		ANO_DT_SendString(1,"mod1_begin");
// 	}
// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);		

// 	if(_time_ok(task_1) && ano_of.of_alt_cm > Default_height - 2)//起飞到145cm
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// //		MY_Camera_print("OK");
// 		ANO_DT_SendString(2,"145 OK!");
// 		Move_stop();
// 		return;
// 	}
// }

// void mission_1_2(void)				//悬停
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}
// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);			
// 	Move_stop();

// 	if(_time_ok(task_1))
// 	{
// 		Move_stop();

// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		ANO_DT_SendString(2,"3s OK!");

// 		return;
// 	}
// }

// void mission_1_3(void)						//寻找
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}
// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);				

// 	control_to_B();
// 	if(Camera.x != 0 && Camera.y != 0 )
// 		work_flag = 1;

// 	if(work_flag && _time_ok(task_1))
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		Move_stop();
// 		ANO_DT_SendString(2,"find OK!");
// 		return;
// 	}
// }

// void mission_1_4(void)						//锁定跟随（A->B）
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}

// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);				

// 	control_to_follow();

// 	if(abs(Camera.x - _Camera_middle_X) < 30 && abs(Camera.y - _Camera_middle_Y) && Car.num == 'B' && _time_ok(task_1))
// 	{
// 		Release_fun();
// 		work_flag = 1;
// 	}
// 	if(_time_ok(task_1)     || work_flag)
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		ANO_DT_SendString(2,"follow B OK!");
// 		return;
// 	}
// }

// void mission_1_5(void)							//锁定跟随（B->C）
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}

// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);				

// 	control_to_follow();
	
// 	if((_time_ok(task_1) && Car.num == 'C') || work_flag)
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		ANO_DT_SendString(2,"follow C OK!");
// 		return;
// 	}
// }

// void mission_1_6(void)							//锁定跟随（C->D）+ 投放
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}

// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);				

// 	control_to_follow();

// 	if(abs(Camera.x - _Camera_middle_X) < 30 && abs(Camera.y - _Camera_middle_Y))
// 		Release_fun();
	
// 	if((_time_ok(task_1) && Car.num == 'D') || work_flag)
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		ANO_DT_SendString(2,"follow B OK!");
// 		return;
// 	}
// }

// void mission_1_7(void)								//返程
// {
// 	if(takeoff_once == 0)
// 	{
// 		postion_target_z = Default_height;
// 		takeoff_once = 1;
// 	}

// 	Set_High(postion_target_z);
// 	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定

// 	control_back();
// 	if(_time_ok(task_1) && abs(_HOME_X - Radar.Location_X) < 10 && abs(_HOME_Y - Radar.Location_Y) < 10)
// 	{
// 		task_1.finish = 1;
// 		takeoff_once = 0;
// 		ANO_DT_SendString(2,"zhuan OK!");
// 		return;
// 	}
// }

// void mission_1_8(void)//降落
// {
// 	if(takeoff_once == 0)//
// 	{
// 		postion_target_z = 0;
// 		begin_land = 1;		
// 		takeoff_once = 1;
// 		Move_stop();
// 	}

// 	Set_High(postion_target_z);
// 	Program_Ctrl_User_Set_HXYcmps(0,0);
// 	if(ano_of.of_alt_cm < 10 && _time_ok(task_1))
// 	{
// 		task_2.finish = 1;
// 		takeoff_once = 0;
// 		FC_Lock();
// 		ANO_DT_SendString(2,"land OK!");
// 		return;
// 	}
// }

/*---------------------------------------------------------*/
void mission_2_1(void)//起飞
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
		Yaw_target = N100.Yaw_100;
		postion_begin_x = Radar.Location_X;
		postion_begin_y = Radar.Location_Y;
		
		OneKey_Takeoff(100);

		ANO_DT_SendString(1,"mod2_begin");
	}
	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定

	if(_time_ok(task_2) && Radar.Location_Z > Default_height - 2)//起飞到150cm
	{
		task_2.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"FLAY OK!");
		Move_stop();
		return;
	}
}

 void mission_2_2(void)				//悬停
 {
 	if(takeoff_once == 0)
 	{
 		postion_target_z = Default_height;
 		takeoff_once = 1;
 	}
 	Set_High(postion_target_z);
 	mode_Hold_Yaw(Yaw_target);			
 	Move_stop();

 	if(_time_ok(task_2))
 	{
 		Move_stop();

 		task_2.finish = 1;
 		takeoff_once = 0;
 		ANO_DT_SendString(2,"3s OK!");

 		return;
 	}
 }

u8 work_2_flag = 0;

void mission_2_3(void)//B点行驶
{
	if(takeoff_once == 0)
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定
	control_to_B();

	if(Camera.x != 0 && Camera.y != 0 )
		work_2_flag = 1;
	if(abs(_B_X - Radar.Location_X) < 15 && abs(_B_Y - Radar.Location_Y) < 15)
		work_2_flag = 2;
		
	if(work_2_flag && _time_ok(task_2))
	{
		Move_stop();
		task_2.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"find OK!");

		return;
	}
}

void mission_2_4(void)//锁定
{
	if(takeoff_once == 0)
	{
		if(work_2_flag == 2)
		{
			task_2.finish = 1;
			return;
		}
		postion_target_z = 100;
		takeoff_once = 1;
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定

	control_to_follow();

	if(abs(_Camera_middle_X - Camera.x) < 40 && abs(_Camera_middle_Y - Camera.y) < 40 && _time_ok(task_2))
	{
		Move_stop();
		Release_fun();
		task_2.finish = 1;
		takeoff_once = 0;
		ANO_DT_SendString(2,"release OK!");
		ANO_DT_Send_MY_DATA(0xF9,2,Camera.x,Camera.y);

		return;
	}
}      

void mission_2_5(void)//返程
{
	if(takeoff_once == 0)//
	{
		postion_target_z = Default_height;
		takeoff_once = 1;
		Move_stop();
	}

	Set_High(postion_target_z);
	mode_Hold_Yaw(Yaw_target);				//保持航向角稳定
	control_back();

	if(abs(postion_begin_x - Radar.Location_X) < 20 && abs(postion_begin_y - Radar.Location_Y) < 20 && _time_ok(task_2))
	{
		task_2.finish = 1;
		takeoff_once = 0;
		Move_stop();
		ANO_DT_SendString(2,"back OK!");
		return;
	}
}

void mission_2_6(void)//降落
{
	if(takeoff_once == 0)//
	{
		postion_target_z = -10;
		begin_land = 1;		
		takeoff_once = 1;
		Move_stop();
	}

	Set_High(postion_target_z);
	Program_Ctrl_User_Set_HXYcmps(0,0);
	if(ano_of.of_alt_cm < 10 && _time_ok(task_2))
	{
		task_2.finish = 1;
		takeoff_once = 0;
		FC_Lock();
		ANO_DT_SendString(2,"land OK!");
		return;
	}
}
