#include "timer.h"
#include "key.h"
#include "delay.h"
#include "stm32f10x.h"
 
void TIM3_PWM_Init(u16 arr,u16 psc)
{
 GPIO_InitTypeDef GPIO_InitStructure;//定义数组
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //使能GPIO外设时钟
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形 GPIOA.7
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;     //LED0-->PB.4 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);      //根据设定参数初始化GPIOB.4
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);       //Pa.4 输出低
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;     //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);      //根据设定参数初始化GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_5);       //Pa.5 输出低

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟 
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                            //定义两个数组作为初始化入口参数
TIM_OCInitTypeDef  TIM_OCInitStructure;
 //初始化TIM3
TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
//初始化TIM3 Channel2 PWM模式  
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
TIM_Cmd(TIM3, ENABLE);  //使能TIM3

u16 pwm=350;
}
//外部中断0服务程序
void EXTIX_Init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
 NVIC_InitTypeDef NVIC_InitStructure;
 KEY_Init();  // 按键端口初始化，此函数在这里不做讲解，具体代码可通过百度云下载
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //使能复用功能时钟
  
     //GPIOE.2 中断线以及中断初始化配置   下降沿触发
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
   EXTI_InitStructure.EXTI_Line=EXTI_Line2; //KEY2
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);   //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
   
   //GPIOE.3   中断线以及中断初始化配置 下降沿触发 //KEY1
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
   EXTI_InitStructure.EXTI_Line=EXTI_Line3;
   EXTI_Init(&EXTI_InitStructure);    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
   
   //GPIOE.4   中断线以及中断初始化配置  下降沿触发 //KEY0
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
   EXTI_InitStructure.EXTI_Line=EXTI_Line4;
   EXTI_Init(&EXTI_InitStructure);    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;   //使能按键KEY0所在的外部中断通道
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2， 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;     //子优先级2
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能外部中断通道
   NVIC_Init(&NVIC_InitStructure);
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;   //使能按键KEY1所在的外部中断通道
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;     //子优先级1 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能外部中断通道
   NVIC_Init(&NVIC_InitStructure);     //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;   //使能按键KEY2所在的外部中断通道
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;     //子优先级0 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //使能外部中断通道
   NVIC_Init(&NVIC_InitStructure);     //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
 delay_ms(10);//消抖
 if(KEY2==0)   //按键KEY2
 {  
  TIM_SetCompare2(TIM3,pwm); 
  GPIO_SetBits(GPIOA,GPIO_Pin_4); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);   //前进
 }   
 EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位  
}

//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
 delay_ms(10);//消抖
 if(KEY1==0)  //按键KEY1
 {     
  TIM_SetCompare2(TIM3,pwm); 
  GPIO_SetBits(GPIOA,GPIO_Pin_5); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_4);   //后退
 }   
 EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
 delay_ms(10);//消抖
 if(KEY0==0)  //按键KEY0
 {
  TIM_SetCompare2(TIM3,0);
  GPIO_ResetBits(GPIOA,GPIO_Pin_4); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);      //刹车
 }   
 EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}
