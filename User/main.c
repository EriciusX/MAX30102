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
	
	//Oled_Display_String(, 0, buf); //��ʾ�ַ���
	//Oled_Display_String(2, 0, buf1); //��ʾ�ַ���
	Oled_Display_String(0, 0, buf2); //��ʾ�ַ���
	Oled_Display_String(4, 0, buf3);
	//Oled_Display_Pic(128,64,0,0,pic);
	 
    //uint8_t IIC_Flag=1;//IICͨ��״̬��0Ϊ�ɹ�1Ϊʧ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
    IIC_Init();
    
    
    if(!maxim_max30102_reset())//��λ MAX30102
        printf("max30102_reset failed!\r\n");
    if(!maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy))//��ȡ�����״̬�Ĵ���
        printf("read_reg REG_INTR_STATUS_1 failed!\r\n");
    if(!maxim_max30102_init())//��ʼ��MAX30102
        printf("max30102_init failed!\r\n");
    
    //printf("ָʾ��������\r\n");
    
    n_brightness = 0;
    un_min = 0x3FFFF;
    un_max = 0;
    
    n_ir_buffer_length = 500; //����������Ϊ100�洢��100sps���е�5������
    
    printf("�ɼ�500������\r\n");
    //��ȡǰ500����������ȷ���źŷ�Χ
    for(i = 0; i < n_ir_buffer_length; i++)
    {
        while(max30102_INTPin == 1);   //�ȴ�MAX30102�ж���������

        maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));  //��MAX30102 FIFO��ȡ
            
        if(un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i];    //�����ź���Сֵ
        if(un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i];    //�����ź����ֵ
            
        printf("red=");
        printf("%i", aun_red_buffer[i]);
        printf(", ir=");
        printf("%i\r\n", aun_ir_buffer[i]);

    }
    un_prev_data = aun_red_buffer[i];
    
    
    //����ǰ500������������ʺ�Ѫ�����Ͷȣ�������ǰ5�룩
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
    
    //��MAX30102����ȡ����ÿ1�����һ�����ʺ�Ѫ�����Ͷ�
    while(1)
    {
        i = 0;
        un_min = 0x3FFFF;
        un_max = 0;
        
        //ת���ڴ��е�ǰ100���������������400�������Ƶ�����
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
        
        //�ڼ�������֮ǰ�ɼ�100��������
        for(i = 400; i < 500; i++)
        {
            un_prev_data = aun_red_buffer[i-1];
            while(max30102_INTPin == 1);   //�ȴ�MAX30102�ж���������
            
            maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));
        
            if(aun_red_buffer[i] > un_prev_data)//������������AD���ݵ�ƫ����ȷ��LED������
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


            //ͨ��UART���ն˳����������ͼ�����
            printf("red=");
            printf("%i", aun_red_buffer[i]);
            printf(", ir=");
            printf("%i", aun_ir_buffer[i]);
            printf(", HR=%i, ", n_heart_rate); 
            printf("HRvalid=%i, ", ch_hr_valid);
            printf("SpO2=%i, ", n_sp02);
			
            printf("SPO2Valid=%i\r\n", ch_spo2_valid);
        }
		//OLED_ShowInt(0, 60, aun_red_buffer[i]); //��ʾ����
		//OLED_ShowInt(2, 60, aun_ir_buffer[i]); //��ʾ����
		OLED_ShowInt(0, 60, n_heart_rate); //��ʾ����
		OLED_ShowInt(4, 60, n_sp02);  //��ʾ����
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


//	Oled_Display_Char(0,0,'A');//��ʾ�����ַ�
//There is no luck.There is only work.�����ǲ����ڵģ�Ŭ������Ӳ����
//Oled_Display_String(0,0,buf); //��ʾ�ַ���
//Oled_Display_String(2,0,buf1); //��ʾ�ַ���
//Oled_Display_String(4,0,buf2); //��ʾ�ַ���
//Oled_Display_String(6,80,author);
//	Oled_Display_Pic(128,64,0,0,pic);//��ʾͼƬ
//	Oled_Display_Pic(32,32,2,48,chain);//��ʾ����

*/


