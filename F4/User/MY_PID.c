#include "ALL_include.h"

_PID_parameter	PID_HIGH;
_PID_parameter	PID_YAW;

_PID_parameter  PID_1;    //X
_PID_parameter  PID_2;    //Y
_PID_parameter  PID_3;

_PID_parameter  PID_to_move;
_PID_parameter  PID_to_find;
_PID_parameter  PID_follow;
_PID_parameter  PID_back;

_PID_parameter  PID_Oblique;
/**
  * 函数作用：  初始化PID配置
  * 参数1：     （_PID_parameter）PID名称
  * 参数2：     （float）P值
  * 参数3：     （float）I值
  * 参数4：     （float）D值
  * 参数5：     （float）out_max值
  * 参数6：     （float）out_min值

  * 返回值：     void
  * 
  * 备注：      无
  * 更新日期：   2025-12-7
  */
void _PID_Init(_PID_parameter* PID, float P, float I, float D, float out_max, float out_min)
{
    PID->kp = P;
    PID->ki = I;
    PID->kd = D;

    PID->err_current = 0.0f;
    PID->err_last = 0.0f;
    PID->err_prev = 0.0f;              

    PID->limit_max = out_max;
    PID->limit_min = out_min;
}

/**
  * 函数作用：  传统PID计算
  * 参数1：     （_PID_parameter）PID名称
  * 参数2：     （float）exp目标误差

  * 返回值：     (int）PID输出
  * 
  * 备注：      无
  * 更新日期：   2025-12-7
  *             2026-1-13 修改PID计算位置
  */
int _PID_Traditional_computing(_PID_parameter PID,float exp)//传统PID计算
{
    /*误差计算*/
    PID.err_current = exp;

    /*误差微分*/
    PID.err_prev += PID.err_last;

    // /*误差限幅*/
    // if(ABS(PID.err_current) < 5) 
    //     PID.out = 0;

    /*积分限幅*/
    if (PID.err_prev > 20.0f)
      PID.err_prev = 0;
    else if (PID.err_prev < -20.0f)
      PID.err_prev = 0;

      
    /*PID计算*/
    PID.out = PID.kp * PID.err_current
              + PID.ki * PID.err_prev
              + PID.kd * (PID.err_current - PID.err_last); 

    PID.err_last = PID.err_current;

	return limit_pidout(1.0f,PID.out, PID.limit_max, PID.limit_min);
}

/**
  * 函数作用：  增量式PID计算
  * 参数1：     （_PID_parameter）PID名称
  * 参数2：     （float）exp目标误差

  * 返回值：     (int）PID输出
  * 
  * 备注：      无
  * 更新日期：   2026-1-13
  */
int _PID_Increment_computing(_PID_parameter PID,float exp)//增量式PID计算
{
    /*误差计算*/
    PID.err_current = exp;

    /*误差微分*/
    PID.err_prev += PID.err_last;

    /*误差限幅*/
    if(ABS(PID.err_current) < 5) 
        PID.out = 0;

    /*积分限幅*/
    if (PID.err_prev > 20.0f)
      PID.err_prev = 0;
    else if (PID.err_prev < -20.0f)
      PID.err_prev = 0;

    /*PID计算*/
    PID.out += PID.kp * PID.err_current
              + PID.ki * PID.err_prev
              + PID.kd * (PID.err_current - PID.err_last); 

    PID.err_last = PID.err_current;

	return limit_pidout(1.0f,PID.out, PID.limit_max, PID.limit_min);
}

/**
  * 函数作用：  PID输出限幅
  * 参数1：     （float）增幅倍数
  * 参数2：     （float）PID输出值
  * 参数3：     （float）输出上限
  * 参数4：     （float）输出下限
  * 返回值：    （int）PID输出
  * 
  * 备注：      一般增幅倍数为1.0f
  * 更新日期：   2025-11-27
  */
int limit_pidout(float pid_k, float pidout, float max, float min)
{
    pidout = pid_k * pidout;
    if (pidout > max)
        pidout = max;
    else if (pidout < min)
        pidout = min;   
    return (int)pidout;
}

/*-----------------------------------------------------------------------------------------------*/
/*-------------------------------------应用层----------------------------------------------------*/


/**
  * 函数作用：  光流闭环速度定高
  * 参数1：     （float）目标高度
  * 返回值：    （int）PID输出
  * 
  * 备注：      配合匿名光流使用
  * 更新日期：   2025-11-27
  */
int PID_calculate_user_high(float exp)//高度PID计算
{
    /*高度数据只取光流高度*/
    PID_HIGH.err_current = exp - Radar.Location_Z;

    /*计算PID*/
	  PID_HIGH.out = PID_HIGH.kp * PID_HIGH.err_current
                 + PID_HIGH.ki * PID_HIGH.err_prev
                 + PID_HIGH.kd * (PID_HIGH.err_current - PID_HIGH.err_last);

    /*误差微分*/
    PID_HIGH.err_last = PID_HIGH.err_current;

    /*误差积分*/
    PID_HIGH.err_prev += PID_HIGH.err_current;

    /*积分限幅*/
    if (PID_HIGH.err_prev > 20.0f)
      PID_HIGH.err_prev = 0;
    if (PID_HIGH.err_prev < -20.0f)
      PID_HIGH.err_prev = 0;
    
    /*降落死区*/
    if(begin_land == 1)
    {
        if(Radar.Location_Z < 100 && Radar.Location_Z >= 60)
        {
            PID_HIGH.limit_max = 15.0f;
            PID_HIGH.limit_min = -15.0f;
        }
        else if(Radar.Location_Z < 60 && Radar.Location_Z >= 35)
        {
            PID_HIGH.limit_max = 28.0f;
            PID_HIGH.limit_min = -28.0f;
        }
        else if(Radar.Location_Z < 35 && Radar.Location_Z > 0)
        {
            PID_HIGH.limit_max = 40.0f;
            PID_HIGH.limit_min = -40.0f;
        }
        else
        {
            PID_HIGH.limit_max = 30.0f;
            PID_HIGH.limit_min = -30.0f;
        }
    }
    else
    {
        PID_HIGH.limit_max = 40.0f;
        PID_HIGH.limit_min = -40.0f;
    }

    return limit_pidout(1.3f,PID_HIGH.out, PID_HIGH.limit_max, PID_HIGH.limit_min);
}

/**
  * 函数作用：  PID1更新函数
  * 参数1：     （float）目标误差
  * 返回值：    （int）PID输出
  * 
  * 备注：      
  * 更新日期：   2025-11-29
  *             2025-12-7 重构,使用底层传统PID计算函数
  */
int PID1_updata(float exp)
{
  float e_exp = 0;
  if(exp > -2 && exp < 2)
    e_exp = 0;
  else
    e_exp = exp;

  return _PID_Traditional_computing(PID_1, e_exp);//增量式PID
}

/**
  * 函数作用：  PID2更新函数
  * 参数1：     （float）目标误差
  * 返回值：    （int）PID输出
  * 
  * 备注：      
  * 更新日期：   2025-11-29
  *             2025-12-7 重构,使用底层传统PID计算函数
  */
int PID2_updata(float exp)
{
  float e_exp = 0;
  if(exp > -2 && exp < 2)
    e_exp = 0;
  else
    e_exp = exp;

  return _PID_Traditional_computing(PID_2, e_exp);//传统式PID
}

/**
  * 函数作用：  PID3更新函数
  * 参数1：     （float）目标误差
  * 返回值：    （int）PID输出
  * 
  * 备注：      
  * 更新日期：   2025-11-29
  *             2025-12-7 重构,使用底层传统PID计算函数
  */
int PID3_updata(float exp)
{
  float e_exp = 0;
  // if(exp > -40 && exp < 40)
  //   e_exp = 0;
  // else
    e_exp = exp;
  return _PID_Traditional_computing(PID_3, e_exp);//传统式PID
}
/**
  * 函数作用：  PID_Oblique更新函数
  * 参数1：     （float）目标误差
  * 返回值：    （int）PID输出
  * 
  * 备注：      
  * 更新日期：   2026-7-24 创建
  */
int PID_Oblique_updata(float exp)
{
  float e_exp = 0;
  // if(exp > -40 && exp < 40)
  //   e_exp = 0;
  // else
    e_exp = exp;
  return _PID_Traditional_computing(PID_Oblique, e_exp);//传统式PID
}

/**
  * 函数作用：  传统PID + 最小限幅
  * 参数1：     （_PID_parameter）PID名称
  * 参数2：     （float）exp目标误差
  * 参数3：     （float）最小限幅

  * 返回值：     (int）PID输出
  * 
  * 备注：      无
  * 更新日期：   2025-12-7
  *             2026-1-13 修改PID计算位置
  */
int _PID_Traditional_and_Dead_Zone(_PID_parameter PID, float exp, float min_out)//传统PID计算
{
    /*误差计算*/
    PID.err_current = exp;

    /*误差微分*/
    PID.err_prev += PID.err_last;

    // /*误差限幅*/
    // if(ABS(PID.err_current) < 5) 
    //     PID.out = 0;

    /*积分限幅*/
    if (PID.err_prev > 20.0f)
      PID.err_prev = 0;
    else if (PID.err_prev < -20.0f)
      PID.err_prev = 0;

      
    /*PID计算*/
    PID.out = PID.kp * PID.err_current
              + PID.ki * PID.err_prev
              + PID.kd * (PID.err_current - PID.err_last); 

    PID.err_last = PID.err_current;

    /*最小限幅*/
    if(abs(PID.out) < min_out)
    {
      if(PID.out < 0)
        PID.out = -min_out;
      else if(PID.out > 0)
        PID.out = min_out;
    }

	return limit_pidout(1.0f,PID.out, PID.limit_max, PID.limit_min);
}

/**
  * 函数作用：  N100闭环航向角
  * 参数1：     （float）目标航向角
  * 返回值：    （int）PID输出
  * 
  * 备注：      配合N100使用
  * 更新日期：   2025-11-29
  *             2025-12-7 重构,调用底层传统PID计算函数
  */
int PID_N100_Hold_Yaw(float exp)//航向角PID计算
{
	/*方向最优转化*/
	if((N100.Yaw_100 > exp))	
	{
		if(N100.Yaw_100 - exp < 18000)	//逆时针 负
			PID_YAW.err_current = exp - ((double)N100.Yaw_100);
		else							//顺时针 正
			PID_YAW.err_current = 36000 - ((double)N100.Yaw_100) + exp;
	}
	else
	{
		if(exp - N100.Yaw_100 < 18000)	//顺时针	 正
			PID_YAW.err_current = exp - ((double)N100.Yaw_100);
		else							//逆时针	 负
			PID_YAW.err_current = -(((double)N100.Yaw_100) + 36000 - exp);
	}
	
  return _PID_Traditional_computing(PID_YAW, PID_YAW.err_current);
}



/*PID参数初始化*/
void MY_PID_Init(void)
{
  /*定高PID参数初始化*/
    _PID_Init(&PID_HIGH, 0.55f, 0.0f, 0.2f, 40.0f, -40.0f);

  /*航向角PID参数初始化*/  
    _PID_Init(&PID_YAW, 0.008f, 0.0f, 0.001f, 22.0f, -22.0f);
	
  /*PID参数初始化*/
    _PID_Init(&PID_1, 0.04f, 0.0f, 0.01f, 20.0f, -20.0f);      //X环
    _PID_Init(&PID_2, 0.04f, 0.0f, 0.01f, 20.0f, -20.0f);      //Y环
    _PID_Init(&PID_3, 0.011f, 0.0f, 0.001f, 18.0f, -18.0f);    //锁定

    /*跟随投放*/
    _PID_Init(&PID_follow,0.12f,0.0f,0.01f,20.0f,-20.0f);

    /*返航*/
    _PID_Init(&PID_back,0.2f,0.0f,0.02f,30.0f,-30.0f);

    /*寻找移动*/
    _PID_Init(&PID_to_move,0.12f,0.0f,0.01f,27.0f,-27.0f);
}

