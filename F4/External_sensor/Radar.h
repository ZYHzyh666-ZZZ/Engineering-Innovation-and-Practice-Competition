#ifndef __RADAR_H
#define __RADAR_H

#include "ALL_include.h"

#define Radar_RCC_Work RCC_AHB1Periph_GPIOA
#define Radar_GPIO_Work GPIOA
#define Radar_Pin_Work GPIO_Pin_8


typedef struct
{
    double Location_X;		//雷达位置X数据 cm
    double Location_Y;		//雷达位置Y数据 cm
    double Location_Z;		//雷达位置Z数据 cm

    double Velocity_X;		//雷达速度X数据 cm/s
    double Velocity_Y;		//雷达速度Y数据 cm/s
    double Velocity_Z;		//雷达速度Z数据 cm/s

    double YAW;         //雷达航向角数据

    u8      work_state;         //工作状态      0 失联  1 正常  2 飘飞
    u16     work_ms_cnt;        //工作时间
    u8      update_cnt;         //计数  
}Radar_struct;

extern Radar_struct Radar;

void Radar_init(void);              //初始化
void Radar_GetOneByte(u8 data);
void Radar_work_judgment(double s);

#endif
