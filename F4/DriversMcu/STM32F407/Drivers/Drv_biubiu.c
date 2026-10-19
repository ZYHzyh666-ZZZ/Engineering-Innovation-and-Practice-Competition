#include "Drv_biubiu.h"

typedef struct {

	u16 lazer_times;//单次点亮时间
	u16 lazer_cnt;//时间计数

	u8 lazer_set_count;	//点亮次数
	u16 lazer_delay;		//点亮间隔

} _LAZER_st;

_LAZER_st MY_lazer_1;
_LAZER_st MY_lazer_2;

void Dvr_LAZER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(ANO_RCC_LAZER_1, ENABLE);
	RCC_AHB1PeriphClockCmd(ANO_RCC_LAZER_2, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ANO_Pin_LAZER_1;
	GPIO_Init(ANO_GPIO_LAZER_1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ANO_Pin_LAZER_2;
	GPIO_Init(ANO_GPIO_LAZER_2, &GPIO_InitStructure);
	
	GPIO_ResetBits(ANO_GPIO_LAZER_1, ANO_Pin_LAZER_1);
	GPIO_ResetBits(ANO_GPIO_LAZER_2, ANO_Pin_LAZER_2);

	MY_lazer_1.lazer_times = 150;
	MY_lazer_1.lazer_delay = 5000;
	MY_lazer_1.lazer_cnt = 0;
	MY_lazer_1.lazer_set_count = 0;

	MY_lazer_2.lazer_times = 1000;
	MY_lazer_2.lazer_cnt = 0;
	MY_lazer_2.lazer_set_count = 0;
}


/**
  * 函数作用：  设置led闪烁次数
  * 参数1：     闪烁次数
  * 返回值：    无
  * 
  * 备注：      依赖更新函数实现
  * 更新日期：   2026-5-1
  */
void MY_Lazer_1_ON(u16 times)
{
	MY_lazer_1.lazer_set_count = times;
	MY_lazer_1.lazer_cnt = 0;
}

/**
  * 函数作用：  led闪烁更新函数
  * 参数1：     时间间隔
  * 返回值：    无
  * 
  * 备注：      基于RTOS的任务
  * 更新日期：   2026-5-1
  */
void MY_Lazer_1_updata(u8 dT_ms)
{
	if(MY_lazer_1.lazer_set_count > 0)
	{
		MY_lazer_1.lazer_cnt += dT_ms;
		if(MY_lazer_1.lazer_cnt <= MY_lazer_1.lazer_delay)//大间隔5s
			LAZER_1_OFF;
		else if(MY_lazer_1.lazer_cnt < MY_lazer_1.lazer_delay + MY_lazer_1.lazer_times * (MY_lazer_1.lazer_set_count * 2 - 1))//闪烁时间
		{
			if((MY_lazer_1.lazer_cnt - MY_lazer_1.lazer_delay) / MY_lazer_1.lazer_times % 2 == 0)
				LAZER_1_ON;
			else
				LAZER_1_OFF;
		}
		else//完成一次闪烁
		{
			MY_lazer_1.lazer_cnt = 0;
			LAZER_1_OFF;
		}
	}
}

/**
  * 函数作用：  激光开始闪烁
  * 参数1：     闪烁次数
  * 返回值：    无
  * 
  * 备注：      依赖更新函数实现
  * 更新日期：   2026-5-1
  */
void MY_Lazer_2_ON(u16 times)
{
	MY_lazer_2.lazer_set_count = times;
	MY_lazer_2.lazer_cnt = 0;
}

/**
  * 函数作用：  激光更新函数
  * 参数1：     时间间隔
  * 返回值：    无
  * 
  * 备注：      基于RTOS的任务
  * 更新日期：   2026-5-1
  */
void MY_Lazer_2_updata(u8 dT_ms)
{
	MY_lazer_2.lazer_cnt += dT_ms;
	if(MY_lazer_2.lazer_cnt > MY_lazer_2.lazer_times * MY_lazer_2.lazer_set_count)
		MY_lazer_2.lazer_cnt = MY_lazer_2.lazer_times * MY_lazer_2.lazer_set_count;

	if(MY_lazer_2.lazer_cnt % MY_lazer_2.lazer_times < MY_lazer_2.lazer_times/2 && MY_lazer_2.lazer_cnt % MY_lazer_2.lazer_times != 0)
		LAZER_2_ON;
	else if(MY_lazer_2.lazer_cnt % MY_lazer_2.lazer_times < MY_lazer_2.lazer_times)
		LAZER_2_OFF;
	else
		LAZER_2_OFF;
			// MY_lazer_2.lazer_cnt = 0;
	
}