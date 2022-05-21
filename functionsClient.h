#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED


bool recv_msg(int sd, int code, char *text);
void send_msg(int sd, char *operation, char *param);
char * read_input();
void authenticate(int sd);
int creat_SocketData(struct sockaddr_in *local_structClient);
void get_info(int sd, char *ip, int *port, struct sockaddr_in *local_structClient);
int accept_client(int sd, struct sockaddr_in *server_address, int *len_server_address);
void recv_data(int sd, FILE *file, int f_size);
void ipPort(char *bufferOut, char *ip, int port);
void send_cmdPort(int sd, char *buffer, char *ip, int *port);
void total_socket_data(int sd, struct sockaddr_in *local_structClient, int f_size, char *file_name);
void get(int sd, char *file_name, struct sockaddr_in *local_structClient);
void dir(int sd, struct sockaddr_in *local_structClient, char *dir_name);
void cd_dir(int sd, struct sockaddr_in *local_structClient, char *dir_name);
void mkd(int sd, struct sockaddr_in *local_structClient, char *dir_name);
void rmd(int sd, struct sockaddr_in *local_structClient, char *dir_name);
void quit(int sd);
void operate(int sd, struct sockaddr_in *local_structClient);
int create_socket(char *ip, struct sockaddr_in *local_address, char *port);
int connection_Server(int sd, struct sockaddr_in *server_struct);



#endif // FUNCTIONS_H_INCLUDED
