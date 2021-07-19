#include "timer.h"
#include "key.h"
#include "delay.h"
#include "stm32f10x.h"
 
void TIM3_PWM_Init(u16 arr,u16 psc)
{
 GPIO_InitTypeDef GPIO_InitStructure;//��������
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //ʹ��GPIO����ʱ��
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨�� GPIOA.7
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;     //LED0-->PB.4 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);      //�����趨������ʼ��GPIOB.4
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);       //Pa.4 �����
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;     //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);      //�����趨������ʼ��GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_5);       //Pa.5 �����

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ�ܶ�ʱ��3ʱ�� 
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;                            //��������������Ϊ��ʼ����ڲ���
TIM_OCInitTypeDef  TIM_OCInitStructure;
 //��ʼ��TIM3
TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
//��ʼ��TIM3 Channel2 PWMģʽ  
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

u16 pwm=350;
}
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
 NVIC_InitTypeDef NVIC_InitStructure;
 KEY_Init();  // �����˿ڳ�ʼ�����˺��������ﲻ�����⣬��������ͨ���ٶ�������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ�ܸ��ù���ʱ��
  
     //GPIOE.2 �ж����Լ��жϳ�ʼ������   �½��ش���
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
   EXTI_InitStructure.EXTI_Line=EXTI_Line2; //KEY2
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);   //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
   
   //GPIOE.3   �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY1
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
   EXTI_InitStructure.EXTI_Line=EXTI_Line3;
   EXTI_Init(&EXTI_InitStructure);    //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
   
   //GPIOE.4   �ж����Լ��жϳ�ʼ������  �½��ش��� //KEY0
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
   EXTI_InitStructure.EXTI_Line=EXTI_Line4;
   EXTI_Init(&EXTI_InitStructure);    //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;   //ʹ�ܰ���KEY0���ڵ��ⲿ�ж�ͨ��
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�2�� 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;     //�����ȼ�2
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //ʹ���ⲿ�ж�ͨ��
   NVIC_Init(&NVIC_InitStructure);
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;   //ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�2 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;     //�����ȼ�1 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //ʹ���ⲿ�ж�ͨ��
   NVIC_Init(&NVIC_InitStructure);     //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
   
   NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;   //ʹ�ܰ���KEY2���ڵ��ⲿ�ж�ͨ��
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�2 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;     //�����ȼ�0 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //ʹ���ⲿ�ж�ͨ��
   NVIC_Init(&NVIC_InitStructure);     //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
 delay_ms(10);//����
 if(KEY2==0)   //����KEY2
 {  
  TIM_SetCompare2(TIM3,pwm); 
  GPIO_SetBits(GPIOA,GPIO_Pin_4); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);   //ǰ��
 }   
 EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ  
}

//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
 delay_ms(10);//����
 if(KEY1==0)  //����KEY1
 {     
  TIM_SetCompare2(TIM3,pwm); 
  GPIO_SetBits(GPIOA,GPIO_Pin_5); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_4);   //����
 }   
 EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{
 delay_ms(10);//����
 if(KEY0==0)  //����KEY0
 {
  TIM_SetCompare2(TIM3,0);
  GPIO_ResetBits(GPIOA,GPIO_Pin_4); 
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);      //ɲ��
 }   
 EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}
