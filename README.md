# ship
a test for my GarbageShip  
这个代码库并不是很全，由于时间关系，model.py,p_test1.py和模型参数并未上传，stm32端只上传了main函数，全部上传意义不大，别的电脑上调试不一定能成功。看一下stm32端的主函数就可以。不过一些关键代码已经上传。  
#函数  
mqtt.py:负责mqtt协议  
rpi.py：负责获取计算机信息上传到IOT  
ALilink,py：连接到阿里云平台  
iot.py：iot的主程序   
serial1.py： 串口处理程序  
main.c：stm32端主程序  
