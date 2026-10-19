#include "Cemera.h"
#include "ANO_uart_V8.h"
#include "Drv_Uart.h"

#define Radar_datadel(X) (X==0?1:-1)

s8 mod = 0;
s16 location_X = 0,location_Y = 0;

s16 location = 0;
s16 width = 0;
s16 number = 0;

u8 cemera_data_get[20] = {0};
u8 sum_data = 0;

u8 camera_flag;
s16 camera_x, camera_y;

void Cemera_DataAnl(u8* data, int data_len)
{
	switch(*data)
	{
		case 0x0A:
		{
//			width = *(data+1)*100 + *(data+2)*10 + *(data+3);
//			location = *(data+4)*100 + *(data+5)*10 + *(data+6);
			
//			ANO_Send_Data(0xF2,3,(int)width,(int)location,0x01);
		}
		case 0x0B:
		{

		}
		case 0x0C:
		{
			location = *(data+1) << 8 | *(data+2);
			width = *(data+3) << 8 | *(data+4);
			number = *(data+6);
			if(number == 0xFF)
				number = 0;
		}
		case 0x0D:
		{
			
		}
		case 0x01:
		{
			// camera_flag = *(data+1);
            camera_x = ((u16)*(data+1) << 8) | *(data+2);
            // if(camera_flag == 0)
            //     camera_x *= -1;

			// camera_flag = *(data+4);
            camera_y = ((u16)*(data+3) << 8) | *(data+4);
            // if(camera_flag == 0)
            //     camera_y *= -1;

		}
	}
	if(*data == 0x01)
	{
		ANO_Send_Data(0xF4,3,camera_x,camera_y,number,0x01);
		print_F4("@%c%c%3d%3d*",0x07,0x04,camera_x,camera_y);
	}
	
	if(*data == 0x0C)
	{
		ANO_Send_Data(0xF2,4,location,width,number,0x01);
		print_F4("@%c%c%3d%3d%1d*",0x08,0x02,location,width,number);
	}
}

void Cemera_get_onebyte(u8 data)
{
	static u8 _data_len = 0, _data_cnt = 0;
	static u8 rxstate = 0;

//	DrvUart6SendBuf(&data,1);
	if (rxstate == 0 && data == 0xEF)     //帧头1
    {
        rxstate = 1;
    }
    
	else if (rxstate == 1)                //数据长度      
    {
        sum_data = 0;
        _data_len = data;
        _data_cnt = 0;
        rxstate = 2;
    }

    else if (rxstate == 2)                //数据内容
    {	            
        cemera_data_get[_data_cnt] = data; 
        sum_data += data;
        _data_cnt++;       
        if(_data_cnt >= _data_len)
            rxstate = 3;
    }                 

	else if (rxstate == 3)               //校验和      
    {
//        if(data == sum_data)
            rxstate = 4;
//        else
//            rxstate = 0;
    }
    else if (rxstate == 4 && data == 0xFE)
    {
        Cemera_DataAnl(cemera_data_get,_data_len);
        rxstate = 0;
    }
	else 
	{		
		rxstate = 0;	
	}
}
