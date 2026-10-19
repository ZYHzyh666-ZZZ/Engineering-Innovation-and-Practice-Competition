#include "ALL_include.h"

__N100_stu N100 = {0};
__Camera_stu Camera = {0};
__Car_stu	Car = {0};

u8 Tiva_data_in[100] = {0};

u8 N100_one_data = 0;

#define num(X) ((X==' ')?(0):(X-'0'))

s32 LX_to_N100(s32 X)
{
	if(X - N100.Yaw_begin_100 > 18000)
		return X - N100.Yaw_begin_100 - 36000;
	else if(X - N100.Yaw_begin_100 < -18000)
		return X - N100.Yaw_begin_100 + 36000;
	else
		return X - N100.Yaw_begin_100;
}

u8 page, type, num;
u8 onekey_flag = 0;

void Tiva_Data(void) //处理接收的数据              
{	
	switch (Tiva_data_in[0])
	{
		case 0x01:
		{

			N100.Pitch_100 = num(Tiva_data_in[1])*10000 + num(Tiva_data_in[2])*1000 + num(Tiva_data_in[3])*100 + num(Tiva_data_in[4])*10 + num(Tiva_data_in[5])*1 - 18000;
			N100.Roll_100 = num(Tiva_data_in[6])*10000 + num(Tiva_data_in[7])*1000 + num(Tiva_data_in[8])*100 + num(Tiva_data_in[9])*10 + num(Tiva_data_in[10])*1 - 9000;
			N100.Yaw_100 = num(Tiva_data_in[11])*10000 + num(Tiva_data_in[12])*1000 + num(Tiva_data_in[13])*100 + num(Tiva_data_in[14])*10 + num(Tiva_data_in[15])*1 - 18000;

			if(N100_one_data == 0 && N100.Yaw_100 != 0)//坐标系统一
			{
				N100_one_data = 1;
				N100.Yaw_begin_100 = N100.Yaw_100;
//				N100.Yaw_begin_100 = fc_att.st_data.yaw_x100;
			}	
			else
			{
				N100.Yaw_100 = LX_to_N100(N100.Yaw_100);
			}

		//	N100.Yaw_100 = N100.Yaw_100 > 18000?N100.Yaw_100 % 18000:N100.Yaw;
			N100.Pitch_100 = -N100.Pitch_100;
			
			// ANO_DT_Send_MY_DATA(0xF3,1,N100.Yaw_100);
			break;
		}
		case 0x04:
		{
			// MY_BEEP_ON(50); 660  370  640  360
						// MY_HMI_print("*P0N01%d!",num);
			Camera.y = num(Tiva_data_in[1])*100 + num(Tiva_data_in[2])*10 + num(Tiva_data_in[3])*1;
			Camera.x = num(Tiva_data_in[4])*100 + num(Tiva_data_in[5])*10 + num(Tiva_data_in[6])*1;

			ANO_DT_Send_MY_DATA(0xF6,2,Camera.x,Camera.y);
			break;
		}
		case 0x05:
		{
			Car.num = Tiva_data_in[1];
			Car.distanse = num(Tiva_data_in[2])*100 + num(Tiva_data_in[3])*10 + num(Tiva_data_in[4])*1;
			Car.yaw_180 = num(Tiva_data_in[5])*100 + num(Tiva_data_in[6])*10 + num(Tiva_data_in[7])*1;
			Car.speed = num(Tiva_data_in[8])*10 + num(Tiva_data_in[9])*1;
			Car.mod = num(Tiva_data_in[10]);
			Car.start_flag = num(Tiva_data_in[11]);
			ANO_DT_Send_MY_DATA(0xF4,6,Car.num,Car.distanse,Car.yaw_180,Car.speed,Car.mod,Car.start_flag);

			switch (Car.start_flag)
			{
				case 1:
				{
					if(onekey_flag == 0)
					{
						// if(rc_in.rc_ch.st_data.ch_[ch_7_aux3] > 1700 && rc_in.rc_ch.st_data.ch_[ch_5_aux1] < 1700)

						if(rc_in.rc_ch.st_data.ch_[ch_7_aux3] > 1700 && rc_in.rc_ch.st_data.ch_[ch_5_aux1] < 1700 && rc_in.rc_ch.st_data.ch_[ch_5_aux1] > 1200)
						{
							start_task2_flag = 1;
							FC_Unlock();
							onekey_flag = 1;							
						}
					}
					break;
				}
				case 2:
				{
					// start_task1_flag = 2;
					// FC_Unlock();
					break;
				}
				default:
					break;
			}

			break;
		}
		default:
			break;
	}

}

/*接收Tiva传过来的数据（转移寄存器）*/
void Tiva_Receive_Data(uint8_t data) 
{
    static u8 state = 0;
	static u8 data_len = 0, data_cnt = 0;;
    // DrvUart2SendBuf(&data,1);
    if (state == 0 && data == '@')      //帧头1
    {
        state = 1;
		data_len = 0;
    }
	else if (state == 1)               //数据长度     
    {
		data_len = data - 1;
		data_cnt = 0;
        state = 2;
    }
	else if (state == 2)               //数据接收   
    {
		if(data_cnt < data_len)
			Tiva_data_in[data_cnt++] = data;
		else if(data_cnt == data_len)
		{
			Tiva_data_in[data_cnt++] = data;
			state = 3;
		}
    }
    else if (state == 3 && data == '*')//帧尾
    {
        Tiva_Data();
        state = 0;
    }
	else 
	{		
		state = 0;	
	}
}

void Tiva_postion_print(s16 X, s16 Y, s16 Z)
{
	u8 _cnt = 2;
	u8 data[50] = {0};
	u8 data_sum = 0;
	
	data[0] = 0xFE;		//帧头
	data[1] = 0x01;		//长度
	
	data[_cnt++] = X;
	data[_cnt++] = X >> 8;
	data[_cnt++] = Y;
	data[_cnt++] = Y >> 8;
	data[_cnt++] = Z;
	data[_cnt++] = Z >> 8;
	
	for(u8 i = 2;i < _cnt;i++)
		data_sum += data[i];
	data[_cnt] = data_sum;
	data[_cnt+1] = 0xEF;
	
	data[1] = _cnt - 2;
	
	DrvUart1SendBuf(data,_cnt+2);
}
