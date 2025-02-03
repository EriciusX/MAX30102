# Max30102 Module
English | [简体中文](README_zh.md)

---
## Environment and Relevant Pins：
#### This program is based on *STM32F103*, with *SCL* pin connected to PC12, *SDA* pin to PC11, and the *INT* pin of MAX30102 connected to PB5.

## Functionality：
#### It can measure heart rate (HR) and blood oxygen saturation (SpO2) and display the data on a 0.96-inch OLED screen every second.

## Description：
#### The data is processed using the official algorithm, which calculates the results after collecting 100 data samples per second.

## Limitation：
#### Before starting the calculation, the official algorithm collects 500 reference data samples. I currently believe that these reference samples should be based on the finger's infrared data. If anyone has other insights, feel free to share.
