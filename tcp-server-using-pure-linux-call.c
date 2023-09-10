#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_CONN 10
#define MY_SERVER_PORT 8080
#define DATA_BUFFER 5000

int create_socket_for_tcp_server_where_it_will_listen_for_incoming_conn()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    printf("Created a socket with file descriptor %d\n", fd);

    return fd;
}
void initialize_socket_addr_data_structure(struct sockaddr_in *saddr)
{
    (*saddr).sin_family = AF_INET;
    (*saddr).sin_port = htons(MY_SERVER_PORT);
    (*saddr).sin_addr.s_addr = INADDR_ANY;
}
/* this method ensures that traffic on port 8080 reaches this port
    by associating the socket(fd) with an address: http://localhost:8080
*/
int bind_socket_to_port(int fd, struct sockaddr_in *saddr)
{
    return bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
}
int listen_for_incoming_conn_by_creating_a_request_queue(int fd, int backlog)
{
    listen(fd, backlog);
}
/* function to create a tcp server and will bind it on a port */
int create_tcp_server()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    int backlog = 5;
    /* first I have to create a virtual file
    where the incoming connections will come for acceptance */
    fd = create_socket_for_tcp_server_where_it_will_listen_for_incoming_conn();
    initialize_socket_addr_data_structure(&saddr);
    bind_socket_to_port(fd, &saddr);
    listen_for_incoming_conn_by_creating_a_request_queue(fd, backlog);
    return fd;
}
void set_bit_for_all_those_fd_where_you_have_accepted_a_conn(int *all_conn, int *read_fd_set)
{
    for (int i = 0; i < MAX_CONN; i++)
        if (all_conn[i] >= 0)
            FD_SET(all_conn[i], read_fd_set);
}
void add_new_fd_for_new_conn_to_array(int *all_conn, int new_conn_fd)
{
    for (int i = 0; i < MAX_CONN; i++)
        if (all_conn[i] < 0)
        {
            all_conn[i] = new_conn_fd;
            break;
        }
}
int manage_packets_arriving_from_already_est_conn(int *all_conn, int *read_fd, char *buf)
{
    int ret_value;
    for (int i = 1; i < MAX_CONN; i++)
    {
        if (all_conn[i] > 0 && FD_ISSET(all_conn[i], read_fd))
        {
            ret_value = recv(all_conn[i], buf, DATA_BUFFER, 0);
            /* closing the connection */
            if (ret_value == 0)
            {
                printf("closing connection with client with fd as %d\n", all_conn[i]);
                close(all_conn[i]);
                all_conn[i] = -1;
            }
            else if (ret_value > 0)
            {
                printf("recieved data from the fd: %d is %s\n", all_conn[i], buf);
            }
            else
            {
                printf("recieve failed from the fd: %d with error [%s]\n", all_conn[i], strerror(errno));
            }
        }
    }
    return ret_value;
}
int main(int argc, char const *argv[])
{
    fd_set read_fd_set;
    int new_fd, i, ret_val;
    struct sockaddr_in new_addr;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    int all_conn[MAX_CONN];
    int server_fd = create_tcp_server();

    for (i = 0; i < MAX_CONN; i++)
        buf[i] = -1;
    buf[0] = server_fd;
    while (1)
    {
        FD_ZERO(&read_fd_set);
        set_bit_for_all_those_fd_where_you_have_accepted_a_conn(all_conn, &read_fd_set);
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        if (ret_val >= 0)
        {
            if (FD_ISSET(server_fd, &read_fd_set))
            {
                printf("there is a new connection as event on server fd: %d\n", server_fd);
                int new_conn_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                printf("accepted a new conn with fd: %d\n", new_conn_fd);
                add_new_fd_for_new_conn_to_array(all_conn, new_conn_fd);
                ret_val--;
                if (!ret_val)
                    continue;
            }
            int message_in_bytes_length = manage_packets_arriving_from_already_est_conn(all_conn, &read_fd_set, buf);
        }
    }

    return 0;
}
