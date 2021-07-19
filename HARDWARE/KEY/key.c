#include "key.h"
#include "delay.h"
#include "stm32f10x.h"

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能GPIOD
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);    //GPIOA15
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init(GPIOC,&GPIO_InitStructure);   //GPIOC5
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStructure);      //GPIOA0
	
}

u8 KEY_Scan(u8 mode)
{
	static u8 key_up=1;   //松开标志
	
	if(mode)
		key_up=1;     //支持连按
	
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);   //去抖动
		key_up=0;
		if(KEY0==0)
			return KEY0_PRES;
		else if(KEY1==0)
			return KEY1_PRES;
		else if(WK_UP==1)
			return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)
		key_up=1;
	return 0;
}
