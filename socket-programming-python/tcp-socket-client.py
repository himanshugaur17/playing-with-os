import socket
import sys

client_messages=['My message 1', 'My message 2', 'My message 3']
server_address='localhost'
server_port=8080

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
    s.connect((server_address,server_port)) # this is a system call! it tries to make connection with the server
    print("connection successful")# i will place a breakpoint here and i will observe what's happening in the network (wireshark)
except socket.error as msg:
    raise(f"Failed to connect: {msg}")


for message in client_messages:
    s.send(message.encode('utf-8'))
    data=s.recv(1024)
    print(f"data reieved: {data} from server")
s.close() # one more sys call




