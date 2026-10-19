#include "Radar.h"

Radar_struct Radar;

#define Radar_datadel(X) (X==0?1:-1)

static u8 state = 0, data_count = 0, data_sum = 0, data_len = 0;
static u8 buf[100] = {0};

void Radar_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(Radar_RCC_Work, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Radar_Pin_Work;
	GPIO_Init(Radar_GPIO_Work, &GPIO_InitStructure);

	GPIO_ResetBits(Radar_GPIO_Work, Radar_Pin_Work);

    Radar.Location_X = 0;
    Radar.Location_Y = 0;
    Radar.Location_Z = 0;
    Radar.Velocity_X = 0;
    Radar.Velocity_Y = 0;
    Radar.Velocity_Z = 0;
    Radar.YAW = 0.0f;
}

void Radar_Updata(void)
{
    switch(buf[0])
    {
        case 0x0A://雷达数据
        {
            Radar.Location_Y = Radar_datadel(buf[1]) * (buf[2] << 8 | buf[3]);
            Radar.Location_X = Radar_datadel(buf[4]) * (buf[5] << 8 | buf[6]);
            Radar.Location_Z = Radar_datadel(buf[7]) * (buf[8] << 8 | buf[9]);

            Radar.Location_X /= 10.0f;
            Radar.Location_Y /= 10.0f;
            Radar.Location_Z /= 10.0f;

            Radar.Location_X *= -1;
            Radar.Location_Y *= 1;
            Radar.Location_Z *= 1;

            Radar.Location_X += 500;

            Radar.Velocity_Y = Radar_datadel(buf[10]) * (buf[11] << 8 | buf[12]);
            Radar.Velocity_X = Radar_datadel(buf[13]) * (buf[14] << 8 | buf[15]);
            Radar.Velocity_Z = Radar_datadel(buf[16]) * (buf[17] << 8 | buf[18]);

            Radar.Velocity_X /= 10.0f;
            Radar.Velocity_Y /= 10.0f;
            Radar.Velocity_Z /= 10.0f;

            Radar.Velocity_X *= -1;
            Radar.Velocity_Y *= 1;
            Radar.Velocity_Z *= 1;

            break;
        }
        case 0x0B://IMU速度数据
        {

            break;
        }
        default:
            break;
    }

    if(buf[0] == 0x0A)
    {
        Radar.work_ms_cnt = 0;
        if(ABS((int)Radar.Location_X) > 1000 || ABS((int)Radar.Location_Y) > 1000 || ABS((int)Radar.Velocity_X) > 100 || ABS((int)Radar.Velocity_Y) > 100)
        {
            Radar.work_state = 2;
        }
        else
        {
            Radar.work_state = 1;
            Radar.update_cnt++;
        }
		ANO_DT_Send_MY_DATA(0xF7,6, (int)(Radar.Location_X),
                                    (int)(Radar.Location_Y),
                                    (int)(Radar.Location_Z),
                                    (int)(Radar.Velocity_X),
                                    (int)(Radar.Velocity_Y),
                                    (int)(Radar.Velocity_Z)
                            );

        // fly_velocity_updata();
        // Printf_uart2("%f/%f/%f\n",Radar.Location_X*100,Radar.Location_Y*100,Radar.Location_Z*100);

    }
    else if(buf[0] == 0x0B)
    {
        // ANO_DT_Send_MY_DATA(0xF7,3,(int)(Radar.Velocity_X),(int)(Radar.Velocity_Y),(int)(Radar.Velocity_Z));
    }
}

void Radar_GetOneByte(u8 data)
{
    if(state == 0 && data == 0xEF)		//帧头
    {
        data_len = 0;
        state = 1;
    }
    else if(state == 1)
    {
        data_len = data;		//数据长度
        data_sum = 0;
        data_count = 0;
        state = 2;
    }
    else if(state == 2 && data_count < data_len)
    {
        buf[data_count++] = data;	//数据内容
        data_sum += data;
        if(data_count >= data_len)
            state = 3;
    }
    else if(state == 3 && data == data_sum)	//校验和
    {
        state = 4;
    }
    else if(state == 4 && data == 0xFE)		//帧尾
    {
        Radar_Updata();
        state = 0;
    }
    else
    {
        state = 0;		//异常，重新接收
    }
}

void Radar_work_judgment(double s)
{
    static u16 error_ms = 0;

    Radar.work_ms_cnt += (int)(s*1000);
    if(Radar.work_ms_cnt >= 100) //100ms无响应
    {
        Radar.work_state = 0;
        if(Radar.work_ms_cnt >= 3100)   //1s返回上位机消息
        {
            Radar.work_ms_cnt = 100;
            ANO_DT_SendString(1,"  Radar lost!!");
        }
    }

    if(Radar.work_state == 2)
    {
        if(error_ms++ >= 1000)
        {
            ANO_DT_SendString(1,"  Radar error!!");
            error_ms = 0;
        }
    }
    else
        error_ms = 0;
}
