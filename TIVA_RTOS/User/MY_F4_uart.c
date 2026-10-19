#include "MY_F4_uart.h"
#include "Drv_Uart.h"
#include "ANO_Uart_V8.h"
#include "N100.h"

_postion_def Fly_postion;

u8 F4_data[50] = {0};
u8 F4_rxstate = 0;
u8 _data_len = 0, _data_cnt = 0, _data_index = 0;

u8 HMI_data[50] = {0};
u8 HMI_count = 0;

u8 Car_data[50] = {0};
u8 Car_count = 0;
u8 Car_rxstate = 0;

void F4_get_onebyte(u8 data)
{
//	DrvUart1SendBuf(&F4_rxstate,1);

	if (F4_rxstate == 0)				//帧头
	{
		if(data == 0xFE)		//分支1
		{
			F4_rxstate++;
			_data_len = 0;
		}
		else if (data == '*')
		{
			F4_rxstate = 0xFF;
			HMI_count = 0;
			HMI_data[HMI_count++] = data;
		}
	}
	else if (F4_rxstate == 1)							//长度
	{
		_data_len = data;
		F4_rxstate++;		
		_data_cnt = 0;
		_data_index = 0;
	}
	else if (F4_rxstate == 2)							//数据
	{
		if(_data_index < _data_len - 1)
		{
			F4_data[_data_index++] = data;
			_data_cnt += data;
		}
		else
			F4_rxstate++;
	}
	else if (F4_rxstate == 3 && data == _data_cnt)		//校验
	{
		F4_rxstate++;
	}
	else if (F4_rxstate == 4 && data == 0xEF)			//帧尾
	{
		F4_rxstate = 0;
		F4_data_handle();
	}
	else if (F4_rxstate == 0xFF)
	{
		HMI_data[HMI_count++] = data;
		if (data == '!')
		{
			F4_rxstate = 0;
			DrvUart5SendBuf(HMI_data,HMI_count);
		}
	}
	else
	{		
		F4_rxstate = 0;
	}
}

double AHRS_yaw_change;

void F4_data_handle(void)	//数据处理函数
{
	Fly_postion.x = F4_data[0] + (F4_data[1] << 8);
	Fly_postion.y = F4_data[2] + (F4_data[3] << 8);
	Fly_postion.z = F4_data[4] + (F4_data[5] << 8);
	
	Fly_postion.x *= 1;
	Fly_postion.y *= 1;
	
	AHRS_yaw_change = MY_N100.AHRS.Yaw-90 < -180?(MY_N100.AHRS.Yaw-90 + 360):MY_N100.AHRS.Yaw-90;
//	ANO_Send_Data(0x32,3,(int)Fly_postion.x,(int)Fly_postion.y,(int)Fly_postion.z);
	ANO_Send_Data(0xF1,5,(int)Fly_postion.x,(int)Fly_postion.y,(int)Fly_postion.z,(int)(AHRS_yaw_change),0x01);
	ANO_Send_Data(0x03,4,(int)(MY_N100.AHRS.Roll*100),(int)(MY_N100.AHRS.Pitch*100),(int)((AHRS_yaw_change)*100),0x01);
	
//	print_u1("F4:%d/%d/%d\r\n",Fly_postion.x,Fly_postion.y,Fly_postion.z);
}

u8 HMI_rxstate, HMI_count, HMI_data[50];

void HMI_to_F4(void)
{
	// u8 p_data[50], p_count, i;
	// p_data[p_count++] = '@';
	// p_data[p_count++] = HMI_count;
	// p_data[p_count++] = 0x03;
	// while(HMI_count-- != 0)
	// 	p_data[p_count++] = HMI_data[i++];

	// p_data[p_count++] = '*';

//	print_F4("@%c%c%s*",HMI_count,0x02,HMI_data);
}

void HMI_get_onebyte(u8 data)
{
	DrvUart2SendBuf(&data,1);
}

void Car_data_handle(void)
{
	u8 Num, speed, mod, flag;
	s16 distanse, yaw_180;
	
	Num = Car_data[0];
	distanse = ((s16)Car_data[1]<<8) | Car_data[2];
	yaw_180 =  ((s16)Car_data[3]<<8) | Car_data[4];
	speed = Car_data[5];
	mod = Car_data[6];
	flag = Car_data[7];
	
	print_F4("@%c%c%c%3d%3d%2d%1d%1d*",0x0C,0x05,Num+1,distanse,yaw_180,speed,mod,flag);
}

void Car_get_onebyte(u8 data)
{
	if (Car_rxstate == 0 && data == '*')				//帧头
	{
		Car_rxstate = 1;
		Car_count = 0;
	}
	else if (Car_rxstate == 1)			//帧尾
	{
		if(data == '!' && Car_count == 8)
		{
			Car_data_handle();
			Car_rxstate = 0;
			return;
		}
		if(Car_count > 10)
			Car_rxstate = 0;
		Car_data[Car_count++] = data;
	}
	else
	{		
		Car_rxstate = 0;
	}
}
