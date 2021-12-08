import serial, time, os

ship = serial.Serial(port = "/dev/ttyAMA1", baudrate = 9600)
 
p_IotWrite = "./IotWrite"
# p_IotRead = "./IotRead"
p_tensorflow = "./tensorflow" #two pipe
# 
if(os.access(p_IotWrite, os.F_OK) == False):
    os.mkfifo(p_IotWrite)
print("before open p_IotWrite")
fp_r1 = os.open(p_IotWrite, os.O_WRONLY)
print("open p_IotWrite end")
# if(os.access(p_IotRead, os.F_OK) == False):
#     os.mkfifo(p_IotRead)
# print("before open p_IOtRead")
# fp_r2 = os.open(p_IotRead, os.O_RDONLY)
# print("open p_IotRead end")
if(os.access(p_tensorflow, os.F_OK) == False):
    os.mkfifo(p_tensorflow)
print("before open p_tensorflow")
fp_r3 = os.open(p_tensorflow, os.O_RDONLY)
print("open p_tensorflow end")
time.sleep(1)   #open tw pipe
while True:
    #ship.write("1\r\n".encode("gbk"))
    time.sleep(5) 
    msg = ship.read(6).decode("gbk")
    print(msg)#
#     time.sleep(10)
#     msg = "7.8$33.3"
    os.write(fp_r1, msg.encode("utf-8"))
#     msg = os.read(fp_r2,100)
#     if(msg != ""):
#         #ship.write(msg.encode("gbk"))
#         print("msg".encode("utf-8"))


    msg = os.read(fp_r3,1)
    print(type(msg))
    if(msg != ""):
        if(int(msg) == 1):
            print(msg)
            ship.write("1\r\n".encode("gbk"))
        if(int(msg) == 2):
            print(msg)
            ship.write("2\r\n".encode("gbk"))
        
        
        
# ship.close()
# p_IotWrite.close()
# p_IotRead.close()
#p_tensorflow.close()
    
    

