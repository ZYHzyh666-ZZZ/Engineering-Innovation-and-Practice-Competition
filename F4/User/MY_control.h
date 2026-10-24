#ifndef _MY_CONTROL_H_
#define _MY_CONTROL_H_

#include "ALL_RTOS.h"
#include "LX_FC_State.h"
#include "LX_FC_Fun.h"
#include "Drv_AnoOf.h"

typedef struct
{
	void(*task_mission)(void);	//任务函数名
	float time;               	//任务执行时间
}My_mission_task;				//分任务结构体

typedef struct
{
	u8 finish;					//任务完成标志位
	u8 step;					//任务进度
	u8 task_num;				//任务个数
	float mission_times; 		//任务执行时间
	My_mission_task* mission;	//分任务
}My_task_state;					//任务状态结构体

extern u8 start_task1_flag;		//任务开始标志
extern My_task_state task_1;	//任务结构体1

extern u8 start_task2_flag;		//任务开始标志
extern My_task_state task_2;	//任务结构体2
/*-----------------------------------------------------*/

#define _time_ok(task)	(task.mission_times > task.mission[task.step].time)
#define abs(X) ((X)>0?(X):-(X))

void create_task_1(void);		//创建任务
void create_task_2(void);		//创建任务

void task_run(u16 ms);			//任务运行

void mission_1_1(void);
void mission_1_2(void);
void mission_1_3(void);
void mission_1_4(void);
void mission_1_5(void);
void mission_1_6(void);
void mission_1_7(void);
void mission_1_8(void);
void mission_1_9(void);
void mission_1_10(void);
void mission_1_11(void);
void mission_1_12(void);
void mission_1_13(void);
// void mission_1_14(void);
// void mission_1_15(void);

void mission_2_1(void);
void mission_2_2(void);
void mission_2_3(void);
void mission_2_4(void);
void mission_2_5(void);
void mission_2_6(void);
void mission_2_7(void);
void mission_2_8(void);
void mission_2_9(void);
void mission_2_10(void);
void mission_2_11(void);
void mission_2_12(void);
void mission_2_13(void);

#define Default_height 110			//默认高度cm

enum __location_enum		//地图右X正，上Y正
{
	_HOME_X = 165,
	_HOME_Y = 65,

	_P1_Waiting_X	= 335,
	_P1_Waiting_Y	= 65,			//随机障碍1扫描点

	_P1_choose_1_X	= 320,
	_P1_choose_1_Y	= 65,			//随机障碍1选择点1

	_P1_choose_2_X	= 385,
	_P1_choose_2_Y	= 65,			//随机障碍1选择点2

	_P1_back_Y	= 65,				//随机障碍1返回点
	_P1_move_Y	= 205,				//随机障碍1移动目标点

	_P_goods_1_X	= 165,
	_P_goods_1_Y	= 205,			//危险品任务点1

	_P_goods_2_X	= 215,
	_P_goods_2_Y	= 205,			//危险品任务点2

/*----------------------------------------------------------------*/

	_P2_Waiting_X	= 274,
	_P2_Waiting_Y	= 205,			//随机障碍2扫描点

	_P2_choose_1_X	= 305,
	_P2_choose_1_Y	= 205,			//随机障碍2选择点1

	_P2_choose_2_X	= 380,
	_P2_choose_2_Y	= 205,			//随机障碍2选择点2

	_P2_back_Y	= 205,				//随机障碍2返回点
	_P2_move_Y	= 335,				//随机障碍2移动目标点

	_P_person_1_X	= 265,
	_P_person_1_Y	= 335,			//人物任务点1

	_P_person_2_X	= 350,
	_P_person_2_Y	= 335,			//人物任务点2

	_P_person_3_X	= 440,
	_P_person_3_Y	= 335,			//人物任务点3
};

extern u8 PID1_flag;
extern u8 PID2_flag;
extern s16 position_begin_x;			//记录起飞坐标_X
extern s16 position_begin_y;			//记录起飞坐标_Y

extern float Yaw_target;
extern float X_target;
extern float Y_target;

extern s16 position_target_x, velocity_target_x;
extern s16 position_target_y, velocity_target_y;
extern s16 position_target_z;

#endif
