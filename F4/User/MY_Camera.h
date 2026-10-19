#ifndef _MY_CAMERA_H_
#define _MY_CAMERA_H_

#include "SysConfig.h"

enum 
{
    _Camera_head = 0xEF,
    _Camera_tail = 0xFE,
};

// extern u8 camera_flag[2];

// extern s16 width[2];
// extern s16 location[2];
extern u8 camera_choose[2];

void Camera_Data(void); //处理接收的数据              
void Camera_Receive_Data(uint8_t data); 
void Camera_2_Receive_Data(uint8_t data); 


#endif
