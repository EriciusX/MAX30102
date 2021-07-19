#include "oled.h"
#include "IIC.h"

/*******************************************************************************
*������ԭ�ͣ�void OLED_Send_Command(u8 com)
*�����Ĺ��ܣ�дָ��
*�����Ĳ�����
	@ u8 com��ָ��
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void OLED_Send_Command(u8 com)
{
	IIC_Start();
	IIC_Send_Byte(0x78);//��ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);//дָ�� ��6λ����д����/����
	IIC_Wait_Ack();
	IIC_Send_Byte(com);//ָ��
	IIC_Wait_Ack();
	IIC_Stop();
}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Write_Data(u8 data)
*�����Ĺ��ܣ�д����
*�����Ĳ�����
	@ u8 data������
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Write_Data(u8 data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);//��ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);//д����
	IIC_Wait_Ack();
	IIC_Send_Byte(data);//ָ��
	IIC_Wait_Ack();
	IIC_Stop();
}

/*******************************************************************************
*������ԭ�ͣ�void OLED_Clear(u8 clear_dat)
*�����Ĺ��ܣ�
	@ u8 clear_dat����������
*�����Ĳ�����None
*��������ֵ��None
*������˵����0x00���� 0xff����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void OLED_Clear(u8 clear_dat)
{ 
	u8 i = 0, j = 0;
	
	for(i = 0; i < 8; i++)
	{
		OLED_Send_Command(0xB0 + i);
		OLED_Send_Command(0X00);  //���е�ַ
		OLED_Send_Command(0X10);  //���е�ַ
		for(j = 0; j < 128; j++)
		{
			Oled_Write_Data(clear_dat);
		}
	}

}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Address(u8 row,u8 col)
*�����Ĺ��ܣ���ʾλ��
*�����Ĳ�����
	@ u8 row����
    @ u8 col����
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Address(u8 row,u8 col)
{
	OLED_Send_Command(0xB0 + row);
	OLED_Send_Command(0X10 + ((col & 0xf0) >> 4));  //���е�ַ
	OLED_Send_Command(0X00 + (col & 0x0f)); 
}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Display_Char(u8 page,u8 col,char ch)
*�����Ĺ��ܣ���ʾ�����ַ�
*�����Ĳ�����
	@ u8 page  ��ҳλ��
	@ u8 col   ����λ��	
	@ u8 ch    ���ַ�
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Display_Char(u8 page,u8 col,char ch)
{
	u8 loca = ch - ' ';
	u8 i = 0;
	
	//ҳ��ַ
	Oled_Address(page,col);
	for(i = 0; i < 8; i++)
	{
		Oled_Write_Data(Aciss_8X16[loca * 16 + i]);
	}
	Oled_Address(page + 1,col);
	for(i = 0; i < 8; i++)
	{
		Oled_Write_Data(Aciss_8X16[loca * 16 + 8 + i]);
	}
	//�е�ַ
	//д���� �ϲ���  �²���
}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Display_String(u8 page,u8 col,char *str)
*�����Ĺ��ܣ���ʾ�ַ���
*�����Ĳ�����
	@ u8 page  ��ҳλ��
	@ u8 col   ����λ��	
	@ u8 *str  ���ַ��������׵�ַ
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Display_String(u8 page,u8 col,char *str)
{
	while(*str)
	{
		Oled_Display_Char(page,col,*str);
		col += 8;
		str++;
	}
}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Display_Pic(u8 wight,u8 high,u8 page,u8 col,u8 *str)
*�����Ĺ��ܣ���ʾͼƬ�򵥸�����
*�����Ĳ�����
	@ u8 width ��ͼƬ���ַ����   
	@ u8 high��ͼƬ���ַ��߶�
	@ u8 page  ��ҳλ��
	@ u8 col   ����λ��	
	@ u8 *pic  ��ͼƬ���������׵�ַ
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Display_Pic(u8 wight,u8 high,u8 page,u8 col,u8 *str)
{
	u8 i = 0, j = 0;
	
	for(i = 0; i < high / 8; i++)
	{
		Oled_Address(page + i,col);
		for(j = 0; j < wight;j++)
		{
			Oled_Write_Data(str[wight * i + j]);
		}
	}
}
/*******************************************************************************
*������ԭ�ͣ�void OLED_ShowInt(u8 page,u8 col, int Data)
*�����Ĺ��ܣ���ʾ6λ����
*�����Ĳ�����None
*��������ֵ��None
*������˵����
*������д�ߣ�����
*������д���ڣ�2021/7/18
*�����İ汾�ţ�V1.0
********************************************************************************/
//����ı�λ����������temp = Data/1000000���10000000�ĳ�����Ҫ��λ�������ҽ�����ĸ���λ�����޸�һ�¼���

void OLED_ShowInt(u8 page,u8 col, int Data)
{
	unsigned char temp;
	Oled_Address(page, col);
	if(Data < 0)  //�ж�����Ǹ�����ǰ��Ӹ���
	{
	  Oled_Display_Char(page, col, '-');
	  col += 8;
	  Data = -Data;
	}
	//������Ҫ��ʾ�����������һ����ʾ�����ĸ�λ
	Oled_Display_Char(page, col + 56, ' ');

	temp = Data/1000000;
	Oled_Display_Char(page, col, (temp+'0'));
	
	Data %= 1000000;
	temp = Data/100000;
	Oled_Display_Char(page, col+ 8, (temp+'0'));

	Data %= 100000;
	temp = Data/10000;
	Oled_Display_Char(page, col+ 16, (temp+'0'));
	
	Data %= 10000;
	temp = Data/1000;
	Oled_Display_Char(page, col+ 24, (temp+'0'));

	Data %= 1000;
	temp = Data/100;
	Oled_Display_Char(page, col + 32, (temp+'0'));
	
	Data %= 100;
	temp = Data/10;
	Oled_Display_Char(page, col+ 40, (temp+'0'));

	Data %= 10;
	temp=Data;
	Oled_Display_Char(page, col + 48, (temp+'0'));
}

/*******************************************************************************
*������ԭ�ͣ�void Oled_Init(void)
*�����Ĺ��ܣ�OLED���ų�ʼ��
*�����Ĳ�����None
*��������ֵ��None
*������˵����
*������д��: ����
*������д���ڣ�2021/7/19
*�����İ汾�ţ�V1.0
********************************************************************************/
void Oled_Init(void)
{
	IIC_Init();//iic���ų�ʼ��
	
	OLED_Send_Command(0xAE);//--turn off oled panel
	OLED_Send_Command(0x02);//---SET low column address
	OLED_Send_Command(0x10);//---SET high column address
	OLED_Send_Command(0x40);//--SET start line address  SET Mapping RAM Display Start Line (0x00~0x3F)
	OLED_Send_Command(0x81);//--SET contrast control register
	OLED_Send_Command(0xCF); // SET SEG Output Current Brightness
	OLED_Send_Command(0xA1);//--SET SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_Send_Command(0xC8);//SET COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_Send_Command(0xA6);//--SET normal display
	OLED_Send_Command(0xA8);//--SET multiplex ratio(1 to 64)
	OLED_Send_Command(0x3f);//--1/64 duty
	OLED_Send_Command(0xD3);//-SET display offSET	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_Send_Command(0x00);//-not offSET
	OLED_Send_Command(0xd5);//--SET display clock divide ratio/oscillator frequency
	OLED_Send_Command(0x80);//--SET divide ratio, SET Clock as 100 Frames/Sec
	OLED_Send_Command(0xD9);//--SET pre-charge period
	OLED_Send_Command(0xF1);//SET Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_Send_Command(0xDA);//--SET com pins hardware configuration
	OLED_Send_Command(0x12);
	OLED_Send_Command(0xDB);//--SET vcomh
	OLED_Send_Command(0x40);//SET VCOM Deselect Level
	OLED_Send_Command(0x20);//-SET Page Addressing Mode (0x00/0x01/0x02)
	OLED_Send_Command(0x02);//
	OLED_Send_Command(0x8D);//--SET Charge Pump enable/disable
	OLED_Send_Command(0x14);//--SET(0x10) disable
	OLED_Send_Command(0xA4);// Disable Entire Display On (0xa4/0xa5)
	OLED_Send_Command(0xA6);// Disable Inverse Display On (0xa6/a7) 
	OLED_Send_Command(0xAF);//--turn on oled panel
	OLED_Send_Command(0xAF); /*display ON*/ 
	
	OLED_Clear(0x00); //����
}


