import socket
import select
import queue
port=8080
host="127.0.0.1"
server=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
# server is a file descriptor (if looked deep inside)

server.bind((host,port))
# listen function creates a queue to store requests, the max capped at 5 in my case
server.listen(5)
# client_conn_socket, client_addr=s.accept()
read_fd=[server]
write_fd=[]
while True:
    fd_ready_for_reading,fd_ready_for_writing,_ =select.select(read_fd,write_fd)
    client_data={}
    for s in fd_ready_for_reading:
        if s is server: # "is" and "is not" checks if both the python objects point to the same memory address or not
            client_conn_socket, client_addr=server.accept() # this is a system call but the kernel will return instantaenously, creating a new socket
            print ("accepted new connection and created a new client socket fd: "+client_conn_socket)
            read_fd.append(client_conn_socket)
            client_data[client_conn_socket]=queue.Queue(-1)
        else:
            bytes_data=s.recv(1024) # this call will now copy the data from kernel recieve buffer to user space buffer, this will be done by the kernel
            print("recieved "+bytes_data.decode("utf-8")+" from: "+s)
            client_data[s]=client_data[s].put(bytes_data)
            # had i sent the data here, I might have got blocked
            # the socket.write call is blocking if the sendBuffer for kernel socket is full
            # this could be due to network congestion, in other words, data has got transferred from userspace to kernel-space but still could not be sent over the wire
            # hence kernel send buffer for this socket is still full
            # so i will send data only to those sockets which are in fd_ready_for_writing
    
    for s in fd_ready_for_writing:
        if s is not server:
            if client_data[s].empty():
                print("there is nothing to be sent for socket: "+s+" i will probably not monitor it anymore")
            else:
                s.send(client_data[s].get())








