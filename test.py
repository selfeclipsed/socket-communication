# coding=utf-8
import time
import serial

if __name__ == '__main__':
    port = '/dev/ttyACM0'  # 定义串口通信usb端口
    baud_rate = 9600
    ser = serial.Serial(port, baud_rate, timeout=1)  # 配置串口通信参数
    ser.flushInput()  # 清空输入数据
    ser.flushOutput()
    time.sleep(1)

    while True:
        cmd = raw_input('请输入命令，或输入q退出: ')
        if cmd == 'q':
            break
        try:        
	    cmd = int(cmd)
            if 0 <= cmd <= 10: 
                ser.write(str(cmd).encode())  # encode
            # 接受arduino通过串口发来的信息并打印在屏幕上
            msg = ser.readline().decode()
            if msg:
                print(msg)
        except:
            pass
        
