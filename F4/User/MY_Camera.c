#include "ALL_include.h"

#define X_POSITION 320 
#define Y_POSITION 240 
#define Z_POSITION 0

#define Width_0_baseline 12
#define Width_1_baseline 23
#define Location_0_baseline 200
#define Location_1_baseline 320

char camera_data[20] = {0};
u8 len = 0, count_data = 0;

u8 camera_flag = 0;
u8 camera_choose[2] = {0};
u8 camera_goods = 0;
u8 camera_person = 0;

u8 onekey_flag = 0;

void Camera_Data(void) //处理接收的数据              
{	
	switch(camera_data[0])
	{
		case 0x0A:                  //第一个障碍物识别
		{
            camera_choose[0] = camera_data[1];

			break;
		}
		case 0x0B:                  //危险物识别
		{
			camera_goods = camera_data[1];

			break;
		}
		case 0x0C:                  //第二个障碍物识别
        {
            camera_choose[1] = camera_data[1];

            break;
        }
        case 0x0D:                  //人物识别
        {
            camera_person = camera_data[1];

            break;
        }
        case 0x0F:                  //起飞指令
		{
            if(camera_data[1] == 0x01)
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
	}
}

/*接收Camera传过来的数据（转移寄存器）*/
void Camera_Receive_Data(uint8_t data) 
{
    static u8 state = 0;
    static u8 sum_data = 0;
		// ANO_DT_Send_MY_DATA(0xF2,1,data);

//    DrvUart2SendBuf(&data,1);
    if (state == 0 && data == _Camera_head)     //帧头1
    {
        state = 1;
    }
    
	else if (state == 1)                //数据长度      
    {
        sum_data = 0;
        len = data;
        count_data = 0;
        state = 2;
        // ANO_DT_Send_MY_DATA(0xF2,1,data);
    }

    else if (state == 2)                //数据内容
    {	            
        camera_data[count_data] = data; 
        sum_data += data;
        count_data++;       
        if(count_data >= len)
            state = 3;
    }                 

	else if (state == 3)               //校验和      
    {
        if(data == sum_data)
            state = 4;
        else
            state = 0;
    }
    else if (state == 4 && data == _Camera_head)
    {
        // MY_Camera_print("ok\n");

        Camera_Data();
        state = 0;
    }
	else 
	{		
		state = 0;	
	}
}

void Camera_2_Receive_Data(uint8_t data) 
{
    static u8 state = 0;
    static u8 sum_data = 0;
		// ANO_DT_Send_MY_DATA(0xF2,1,data);

//    DrvUart6SendBuf(&data,1);
    if (state == 0 && data == 0xEF)     //帧头1
    {
        state = 1;
    }
    
	else if (state == 1)                //数据长度      
    {
        sum_data = 0;
        len = data;
        count_data = 0;
        state = 2;
        // ANO_DT_Send_MY_DATA(0xF2,1,data);

    }

    else if (state == 2)                //数据内容
    {	            
        camera_data[count_data] = data; 
        sum_data += data;
        count_data++;       
        if(count_data >= len)
            state = 3;
    }                 

	else if (state == 3)               //校验和      
    {
//        if(data == sum_data)
            state = 4;
//        else
//            state = 0;
    }
    else if (state == 4 && data == 0xFE)
    {
        // MY_Camera_print("ok\n");

        Camera_Data();
        state = 0;
    }
	else 
	{		
		state = 0;	
	}
}
