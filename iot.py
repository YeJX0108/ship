#coding=utf-8
#!/usr/bin/python3
import aliLink,mqttd,rpi
import time,json
import os
#open twp pipe
p_IotRead = "./IotWrite"
p_IotWrite = "./IotRead"


if(os.access(p_IotRead, os.F_OK) == False):
    os.mkfifo(p_IotRead)
print("before open p_IotRead")
fp_r2 = os.open(p_IotRead, os.O_RDONLY)
print("open p_IotRead end")
# if(os.access(p_IotWrite, os.F_OK) == False):
#     os.mkfifo(p_IotWrite)
# print("before open p_IotWrite")
# fp_r1 = os.open(p_IotWrite, os.O_WRONLY)
# print("open p_IotWrite end")

ProductKey = 'gm9nbLl3yBa'
DeviceName = 'GarbageShip0001'
DeviceSecret = '88ec898bd965ac37995a21e90a15b176'
# topic (iot后台获取)
POST = '/sys/gm9nbLl3yBa/${deviceName}/thing/event/property/post'  # 上报消息到云
POST_REPLY = '/sys/gm9nbLl3yBa/${deviceName}/thing/event/property/post_reply'  
SET = '/sys/gm9nbLl3yBa/${deviceName}/thing/service/property/set'  # 订阅云端指令


# 消息回调（云端下发消息的回调函数）
def on_message(client, userdata, msg):
    #print(msg.payload)
    
    Msg = json.loads(msg.payload)
    switch = Msg['params']['PowerLed']
    rpi.powerLed(switch)
    print(msg.payload)  # 开关值
#     Msg = json.loads(msg.payload)
#     print(Msg)
#     os.write(fp_r1, Msg)

#连接回调（与阿里云建立链接后的回调函数）
def on_connect(client, userdata, flags, rc):
    pass



# 链接信息
Server,ClientId,userNmae,Password = aliLink.linkiot(DeviceName,ProductKey,DeviceSecret)

# mqtt链接
mqtt = mqttd.MQTT(Server,ClientId,userNmae,Password)
mqtt.subscribe(SET) # 订阅服务器下发消息topic
mqtt.begin(on_message,on_connect)

time.sleep(2)
# 信息获取上报，每10秒钟上报一次系统参数
while True:
    time.sleep(5)
    #获取指示灯状态
    power_stats=int(rpi.getLed())
    if(power_stats==0):
        power_LED = 0
    else:
        power_LED = 1

    # CPU 信息
    CPU_temp = float(rpi.getCPUtemperature())  # 温度   ℃
    CPU_usage = float(rpi.getCPUuse())         # 占用率 %
 
    # RAM 信息
    RAM_stats =rpi.getRAMinfo()
    RAM_total =round(int(RAM_stats[0]) /1000,1)    # 
    RAM_used =round(int(RAM_stats[1]) /1000,1)
    RAM_free =round(int(RAM_stats[2]) /1000,1)
 
    # Disk 信息
    DISK_stats =rpi.getDiskSpace()
    DISK_total = float(DISK_stats[0][:-1])
    DISK_used = float(DISK_stats[1][:-1])
    DISK_perc = float(DISK_stats[3][:-1])

    #ph and temperature
    msg = os.read(fp_r2,6)
    msg = str(msg,encoding = 'utf-8')
    Ship_ph = msg.split("$")[0]
    print(Ship_ph.encode("utf_8"))
    Ship_temperature  =msg.split("$")[1]
    print(Ship_temperature.encode("utf_8"))
    #print(type(msg))
    # 构建与云端模型一致的消息结构
    updateMsn = {
        'cpu_temperature':CPU_temp,
        'cpu_usage':CPU_usage,
        'ship_ph':Ship_ph,
        'ship_temperature':Ship_temperature
        #'RAM_total':RAM_total,
        #'RAM_used':RAM_used,
        #'RAM_free':RAM_free,
        #'DISK_total':DISK_total,
        #'DISK_used_space':DISK_used,
        #'DISK_used_percentage':DISK_perc,
        #'PowerLed':power_LED
    }
    JsonUpdataMsn = aliLink.Alink(updateMsn)
    print(JsonUpdataMsn)

    mqtt.push(POST,JsonUpdataMsn) # 定时向阿里云IOT推送我们构建好的Alink协议数据

