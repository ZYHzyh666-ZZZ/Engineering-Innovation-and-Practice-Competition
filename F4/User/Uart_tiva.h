#ifndef _UART_TIVA_H_
#define _UART_TIVA_H_

#include "SysConfig.h"

typedef struct
{
	s16 Roll_100;
	s16 Pitch_100;
	s16 Yaw_100;
	
	s16 Yaw_begin_100;
}__N100_stu;

typedef struct 
{
	s16 x;
	s16 y;
}__Camera_stu;

typedef struct 
{
    u8 num;
    s16 distanse;
    s16 yaw_180;
    u8 speed;
    u8 mod;
    u8 start_flag;

    s16 distanse_start;
}__Car_stu;


#define _Camera_middle_X	105
#define	_Camera_middle_Y	175

extern __N100_stu N100;
extern __Camera_stu Camera;
extern __Car_stu	Car;

void Tiva_Data(void); //处理接收的数据              
void Tiva_Receive_Data(uint8_t data); 

void Tiva_postion_print(s16 X, s16 Y, s16 Z);


#endif
