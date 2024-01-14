import cv2
import cv2.aruco as aruco
import numpy as np
import math
import time

# 配置socket
import socket
client = socket.socket()
ip_port = ('192.168.1.106', 8888)
client.connect(ip_port)
# 路径点
path = [[550, 550], [450, 550], [450, 450], [450, 350], [550, 350], [550, 250], [550, 150], [450, 150], [350, 150], [250, 150], [250, 250], [250, 350], [250, 450], [150, 450], [50, 450], [50, 350], [50, 250], [50, 150], [50, 50]]
location = [50, 50]

# 创建空白图像，用于输出
output_size = (600, 600, 3)
output_image = np.zeros(output_size, dtype=np.uint8)

h=600
w=600

# 四个待选取的点坐标
src_points = np.float32([[185, 60], [575, 20], [220, 410], [570, 400]])
dst_points = np.float32([[0.0, 0.0], [w, 0.0], [0.0, h], [w, h]])
# 初始化摄像头
cap = cv2.VideoCapture(1)

# 设置 ArUco 字典
aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_4X4_250)

# 创建 ArUco 参数
parameters = cv2.aruco.DetectorParameters_create()
 
# 读取摄像头帧
ret, frame = cap.read()
    
matrix = cv2.getPerspectiveTransform(src_points, dst_points)
    
warped_image = cv2.warpPerspective(frame, matrix, (w, h))

# 将仿射变换后的图像复制到输出图像中
output_image[:warped_image.shape[0], :warped_image.shape[1]] = warped_image

# 调整输出图像的大小
output_image_resized = cv2.resize(output_image, (600, 600))
    
gray = cv2.cvtColor(output_image_resized, cv2.COLOR_BGR2GRAY)

# 检测 ArUco 标记
corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

# 在图像上绘制检测到的标记
if ids is not None:
    cv2.aruco.drawDetectedMarkers(output_image_resized, corners)
    for i in range(len(ids)):
        # 获取标记的中心坐标
        center_x = int(np.mean(corners[i][0][:, 0]))
        center_y = int(np.mean(corners[i][0][:, 1]))

        # 在图像上绘制中心点
        cv2.circle(output_image_resized, (center_x, center_y), 5, (0, 255, 0), -1)

        # 在图像上显示坐标信息
        cv2.putText(output_image_resized, f"Center: ({center_x}, {center_y})", (center_x, center_y - 10),
            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

while abs(center_x-location[0]>10) or abs(center_y-location[1]>10):
    # 读取摄像头帧
    ret, frame = cap.read()
    print("get new frame")
    matrix = cv2.getPerspectiveTransform(src_points, dst_points)
    
    warped_image = cv2.warpPerspective(frame, matrix, (w, h))

    # 将仿射变换后的图像复制到输出图像中
    output_image[:warped_image.shape[0], :warped_image.shape[1]] = warped_image

    # 调整输出图像的大小
    output_image_resized = cv2.resize(output_image, (600, 600))
    
    gray = cv2.cvtColor(output_image_resized, cv2.COLOR_BGR2GRAY)

    # 检测 ArUco 标记
    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

    # 在图像上绘制检测到的标记
    if ids is not None:
        cv2.aruco.drawDetectedMarkers(output_image_resized, corners)
        for i in range(len(ids)):
            # 获取标记的中心坐标
            center_x = int(np.mean(corners[i][0][:, 0]))
            center_y = int(np.mean(corners[i][0][:, 1]))

            # 在图像上绘制中心点
            cv2.circle(output_image_resized, (center_x, center_y), 5, (0, 255, 0), -1)

            # 在图像上显示坐标信息
            cv2.putText(output_image_resized, f"Center: ({center_x}, {center_y})", (center_x, center_y - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
    if(abs(center_x-location[0])>abs(center_y-location[1])):
        if(center_x-location[0]<0):
            datamanip='5'
            print('5')
        else:
            datamanip='1'
            print('1')
    if(abs(center_x-location[0])<=abs(center_y-location[1])):
        if(center_y-location[1]<0):
            datamanip='3'
            print('3')
        else:
            datamanip='7'
            print('7')
        # 发送命令
        msg_input = datamanip
        client.send(msg_input.encode())

        # 设置接收超时为1秒
        client.settimeout(1.0)

        try:
            # 尝试接收数据
            data = client.recv(1024)
            print(data.decode())
        except socket.timeout:
            # 如果超时，继续执行
            pass

        # 恢复默认的无限超时
        client.settimeout(None)

        # 延迟一段时间
        time.sleep(3)