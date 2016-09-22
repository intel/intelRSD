import socket,select

host = "192.168.1.11"
port = 50000

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((host,port))

s.send("coming from select client")
s.close()
