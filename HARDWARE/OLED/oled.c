#include "oled.h"
#include "IIC.h"

/*******************************************************************************
*函数的原型：void OLED_Send_Command(u8 com)
*函数的功能：写指令
*函数的参数：
	@ u8 com：指令
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void OLED_Send_Command(u8 com)
{
	IIC_Start();
	IIC_Send_Byte(0x78);//地址
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);//写指令 第6位控制写数据/命令
	IIC_Wait_Ack();
	IIC_Send_Byte(com);//指令
	IIC_Wait_Ack();
	IIC_Stop();
}

/*******************************************************************************
*函数的原型：void Oled_Write_Data(u8 data)
*函数的功能：写数据
*函数的参数：
	@ u8 data：数据
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void Oled_Write_Data(u8 data)
{
	IIC_Start();
	IIC_Send_Byte(0x78);//地址
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);//写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(data);//指令
	IIC_Wait_Ack();
	IIC_Stop();
}

/*******************************************************************************
*函数的原型：void OLED_Clear(u8 clear_dat)
*函数的功能：
	@ u8 clear_dat：清屏数据
*函数的参数：None
*函数返回值：None
*函数的说明：0x00：灭 0xff：亮
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void OLED_Clear(u8 clear_dat)
{ 
	u8 i = 0, j = 0;
	
	for(i = 0; i < 8; i++)
	{
		OLED_Send_Command(0xB0 + i);
		OLED_Send_Command(0X00);  //低列地址
		OLED_Send_Command(0X10);  //高列地址
		for(j = 0; j < 128; j++)
		{
			Oled_Write_Data(clear_dat);
		}
	}

}

/*******************************************************************************
*函数的原型：void Oled_Address(u8 row,u8 col)
*函数的功能：显示位置
*函数的参数：
	@ u8 row：行
    @ u8 col：列
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void Oled_Address(u8 row,u8 col)
{
	OLED_Send_Command(0xB0 + row);
	OLED_Send_Command(0X10 + ((col & 0xf0) >> 4));  //高列地址
	OLED_Send_Command(0X00 + (col & 0x0f)); 
}

/*******************************************************************************
*函数的原型：void Oled_Display_Char(u8 page,u8 col,char ch)
*函数的功能：显示单个字符
*函数的参数：
	@ u8 page  ：页位置
	@ u8 col   ：列位置	
	@ u8 ch    ：字符
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void Oled_Display_Char(u8 page,u8 col,char ch)
{
	u8 loca = ch - ' ';
	u8 i = 0;
	
	//页地址
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
	//列地址
	//写数据 上部分  下部分
}

/*******************************************************************************
*函数的原型：void Oled_Display_String(u8 page,u8 col,char *str)
*函数的功能：显示字符串
*函数的参数：
	@ u8 page  ：页位置
	@ u8 col   ：列位置	
	@ u8 *str  ：字符串数组首地址
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
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
*函数的原型：void Oled_Display_Pic(u8 wight,u8 high,u8 page,u8 col,u8 *str)
*函数的功能：显示图片或单个汉字
*函数的参数：
	@ u8 width ：图片或字符宽度   
	@ u8 high：图片或字符高度
	@ u8 page  ：页位置
	@ u8 col   ：列位置	
	@ u8 *pic  ：图片或汉字数组首地址
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
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
*函数的原型：void OLED_ShowInt(u8 page,u8 col, int Data)
*函数的功能：显示6位数字
*函数的参数：None
*函数返回值：None
*函数的说明：
*函数编写者：豪猪
*函数编写日期：2021/7/18
*函数的版本号：V1.0
********************************************************************************/
//如需改变位数，将下面temp = Data/1000000后得10000000改成所需要的位数，并且将下面的各个位计算修改一下即可

void OLED_ShowInt(u8 page,u8 col, int Data)
{
	unsigned char temp;
	Oled_Address(page, col);
	if(Data < 0)  //判定如果是复数，前面加负号
	{
	  Oled_Display_Char(page, col, '-');
	  col += 8;
	  Data = -Data;
	}
	//接下来要显示正数，清空上一次显示负数的个位
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
*函数的原型：void Oled_Init(void)
*函数的功能：OLED引脚初始化
*函数的参数：None
*函数返回值：None
*函数的说明：
*函数编写者: 豪猪
*函数编写日期：2021/7/19
*函数的版本号：V1.0
********************************************************************************/
void Oled_Init(void)
{
	IIC_Init();//iic引脚初始化
	
	OLED_Send_Command(0xAE);//--turn off oled panel
	OLED_Send_Command(0x02);//---SET low column address
	OLED_Send_Command(0x10);//---SET high column address
	OLED_Send_Command(0x40);//--SET start line address  SET Mapping RAM Display Start Line (0x00~0x3F)
	OLED_Send_Command(0x81);//--SET contrast control register
	OLED_Send_Command(0xCF); // SET SEG Output Current Brightness
	OLED_Send_Command(0xA1);//--SET SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_Send_Command(0xC8);//SET COM/Row Scan Direction   0xc0上下反置 0xc8正常
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
	
	OLED_Clear(0x00); //清屏
}


