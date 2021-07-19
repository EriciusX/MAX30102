#include "stm32f10x.h"
#include "usart.h"
#include "oled.h"
#include "word.h"
#include "delay.h"
#include "max30102.h"
#include "30102_algorithm.h"
#include "IIC.h"
 
#define MAX_BRIGHTNESS 255

uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;

int32_t hr_buf[16];
int32_t hrSum;
int32_t hrAvg;
int32_t spo2_buf[16];
int32_t spo2Sum;
int32_t spo2Avg;
int32_t spo2BuffFilled;
int32_t hrBuffFilled;
int32_t hrValidCnt = 0;
int32_t spo2ValidCnt = 0;
int32_t hrThrowOutSamp = 0;
int32_t spo2ThrowOutSamp = 0;
int32_t spo2Timeout = 0;
int32_t hrTimeout = 0;

 int main(void)
 { 
//	char buf[] = {"Red = "};
	//char buf1[] = {"Ir = "};
	char buf2[] = {"HR = "};
	char buf3[] = {"SpO2 = "};
	//char buf[] = {"There is no luck"};
	//char buf1[] = {"There is only"};
	//char buf2[] = {"word."};
	//char author[] = {"Qing"};


	int i;
	float f_temp;
    uint32_t un_min, un_max, un_prev_data;  //variables to calculate the on-board LED brightness that reflects the heartbeats
    int32_t n_brightness;
	delay_init();


	Oled_Init();
	
	//Oled_Display_String(, 0, buf); //显示字符串
	//Oled_Display_String(2, 0, buf1); //显示字符串
	Oled_Display_String(0, 0, buf2); //显示字符串
	Oled_Display_String(4, 0, buf3);
	//Oled_Display_Pic(128,64,0,0,pic);
	 
    //uint8_t IIC_Flag=1;//IIC通信状态，0为成功1为失败
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为9600
    IIC_Init();
    
    
    if(!maxim_max30102_reset())//复位 MAX30102
        printf("max30102_reset failed!\r\n");
    if(!maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy))//读取并清除状态寄存器
        printf("read_reg REG_INTR_STATUS_1 failed!\r\n");
    if(!maxim_max30102_init())//初始化MAX30102
        printf("max30102_init failed!\r\n");
    
    //printf("指示灯亮了吗？\r\n");
    
    n_brightness = 0;
    un_min = 0x3FFFF;
    un_max = 0;
    
    n_ir_buffer_length = 500; //缓冲区长度为100存储以100sps运行的5秒样本
    
    printf("采集500个样本\r\n");
    //读取前500个样本，并确定信号范围
    for(i = 0; i < n_ir_buffer_length; i++)
    {
        while(max30102_INTPin == 1);   //等待MAX30102中断引脚拉低

        maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));  //从MAX30102 FIFO读取
            
        if(un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i];    //更新信号最小值
        if(un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i];    //更新信号最大值
            
        printf("red=");
        printf("%i", aun_red_buffer[i]);
        printf(", ir=");
        printf("%i\r\n", aun_ir_buffer[i]);

    }
    un_prev_data = aun_red_buffer[i];
    
    
    //计算前500个样本后的心率和血氧饱和度（样本的前5秒）
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
    
    //从MAX30102连续取样。每1秒计算一次心率和血氧饱和度
    while(1)
    {
        i = 0;
        un_min = 0x3FFFF;
        un_max = 0;
        
        //转储内存中的前100组样本，并将最后400组样本移到顶部
        for(i = 100; i < 500; i++)
        {
            aun_red_buffer[i-100] = aun_red_buffer[i];
            aun_ir_buffer[i-100] = aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i];
            if(un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i];
        }
        
        //在计算心率之前采集100组样本。
        for(i = 400; i < 500; i++)
        {
            un_prev_data = aun_red_buffer[i-1];
            while(max30102_INTPin == 1);   //等待MAX30102中断引脚拉低
            
            maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));
        
            if(aun_red_buffer[i] > un_prev_data)//根据相邻两个AD数据的偏差来确定LED的亮度
            {
                f_temp = aun_red_buffer[i] - un_prev_data;
                f_temp /= (un_max-un_min);
                f_temp *= MAX_BRIGHTNESS;
                n_brightness -= (int)f_temp;
                if(n_brightness < 0)
                    n_brightness = 0;
            }
            else
            {
                f_temp = un_prev_data-aun_red_buffer[i];
                f_temp /= (un_max-un_min);
                f_temp *= MAX_BRIGHTNESS;
                n_brightness += (int)f_temp;
                if(n_brightness > MAX_BRIGHTNESS)
                    n_brightness = MAX_BRIGHTNESS;
            }


            //通过UART向终端程序发送样本和计算结果
            printf("red=");
            printf("%i", aun_red_buffer[i]);
            printf(", ir=");
            printf("%i", aun_ir_buffer[i]);
            printf(", HR=%i, ", n_heart_rate); 
            printf("HRvalid=%i, ", ch_hr_valid);
            printf("SpO2=%i, ", n_sp02);
			
            printf("SPO2Valid=%i\r\n", ch_spo2_valid);
        }
		//OLED_ShowInt(0, 60, aun_red_buffer[i]); //显示数据
		//OLED_ShowInt(2, 60, aun_ir_buffer[i]); //显示数据
		OLED_ShowInt(0, 60, n_heart_rate); //显示数据
		OLED_ShowInt(4, 60, n_sp02);  //显示数据
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
	}

}

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
/*
int main(void)
{
	char buf[] = {"There is no luck"};
	char buf1[] = {"There is only"};
	char buf2[] = {"word."};
	char author[] = {"Qing"};
	
	delay_init();
	Oled_Init();

	while(1)
	{
		Oled_Display_Pic(128,64,0,0,pic);
	}
}


//	Oled_Display_Char(0,0,'A');//显示单个字符
//There is no luck.There is only work.幸运是不存在的，努力才是硬道理。
//Oled_Display_String(0,0,buf); //显示字符串
//Oled_Display_String(2,0,buf1); //显示字符串
//Oled_Display_String(4,0,buf2); //显示字符串
//Oled_Display_String(6,80,author);
//	Oled_Display_Pic(128,64,0,0,pic);//显示图片
//	Oled_Display_Pic(32,32,2,48,chain);//显示汉字

*/


