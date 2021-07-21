# Max30102模块
---
## 环境与相关引脚：
#### 该程序基于*STM32F103*，其中SCL为PC12，SDA为PC11，MAX30102的INT引脚为PB5
## 功能：
#### 可测量 脉搏(HR) 与 血氧(Sp02) ，并且通过0.96寸OLED屏幕每秒显示数据
## 说明：
#### 数据算法为官方算法，每秒采集100次数据后计算得出
## 不足：
#### 数据开始计算前官方算法会先收集500个数据作为参考，目前我认为采集的参考数据应该是手指的红外数据，如果大家有其他见解，可以提出来
