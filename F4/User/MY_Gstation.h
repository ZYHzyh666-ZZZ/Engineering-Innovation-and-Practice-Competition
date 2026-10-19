#ifndef _MY_Gstation_H_
#define _MY_Gstation_H_

#include "SysConfig.h"

#define Drone_Initial   110436
#define Car_Initial     155360

enum __Car_coordinates_  //坐标枚举
{
    _Car_Location_A = 155360,
    _Car_Location_B = 155180,
    _Car_Location_C = 335130,
    _Car_Location_D = 335310,
};

enum __Drone_coordinates_
{
    _Drone_Location_begin = 110440,

};

typedef struct 
{
    s16 X;
    s16 Y;
    s16 Z;

}HMI_OUT_LOCATION;                      //输出的坐标结构体 


typedef struct
{
    u8 data_in[40];
    u8 HMI_key[5];

    u8 HMI_position_WTS;
    u8 HMI_flash_position_WTS;
    u8 HMI_work_WTS;
    u8 HMI_other_WTS;

    u32 HMI_time;
    HMI_OUT_LOCATION position[10];
    HMI_OUT_LOCATION NOW_position;
    HMI_OUT_LOCATION TARGET_position;

}MY_Gatestation;                        //地面站结构体

extern MY_Gatestation MY_HMI;

void HMI_Data_Print(void);
void HMI_Receive_Data(u8 data);         //接收地面站数据

//void HMI_position_updata(int len);      //更新存储的位置
//void HMI_other_updata(void);            //更新其他数据
//void HMI_other_init(void);


void Print_Gstation(void);

#endif
