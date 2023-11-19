import socket

sk = socket.socket()

ip_port = ('192.168.1.101', 8888)

sk.bind(ip_port)

sk.listen(5)

while True:

    print("waiting......")

    conn, address = sk.accept()

    msg = "socket connected"

    conn.send(msg.encode())

    while True:

        data = conn.recv(1024)
        print(data.decode())
	break
    conn.close()
