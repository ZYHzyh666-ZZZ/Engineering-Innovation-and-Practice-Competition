#include "MY_Fun.h"

/**
  * 函数作用：  判断是否到达指定位置
  * 参数：      s16 Position_X, s16 Position_Y 目标点坐标
  * 返回值：    u8 到达标志位 1到达 0未到达
  * 
  * 备注：      无
  * 更新日期：   2026-7-30
  */
u8 Judgment_Position(s16 Position_X, s16 Position_Y)
{
    if((ABS(Radar.Location_X - Position_X) < 10) && (ABS(Radar.Location_Y - Position_Y) < 10))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


