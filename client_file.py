import socket

client = socket.socket()

ip_port = ('192.168.1.101', 8888)

client.connect(ip_port)

with open("C:/opencv/files/manip.txt", "r", encoding='utf-8') as f:  #打开文本
    datamanip = f.read()
    print(datamanip)
while True:

    data = client.recv(1024)

    print(data.decode())
    msg_input = datamanip
    client.send(msg_input.encode())
    break