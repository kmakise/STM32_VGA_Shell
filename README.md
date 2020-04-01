# STM32_VGA_Shell
使用STM32的IO驱动VGA显示器实现了一个简单的交互和电脑鼠调试工具可添加其他工具
## 说明
使用的是TIM1，2主从方式PWM输出的HV信号然后SPI DMA输出作为颜色信号，仿的VGA，分辨率是800*600 56HZ支持普通的大部分显示器。
测试使用C8T6不完全分辨率，最终版本是ZET6完全分辨率，核心超频到128MHz稳定运行。
## 图片
![1](https://github.com/kerisu/STM32_VGA_Shell/blob/master/photo/1.jpg)
![2](https://github.com/kerisu/STM32_VGA_Shell/blob/master/photo/2.jpg)
![3](https://github.com/kerisu/STM32_VGA_Shell/blob/master/photo/3.jpg)
![4](https://github.com/kerisu/STM32_VGA_Shell/blob/master/photo/4.jpg)
