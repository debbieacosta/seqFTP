#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 512
#define cmd_USER "USER"
#define cmd_PASS "PASS"
#define cmd_PORT "PORT"
#define cmd_RETR "RETR"
#define cmd_QUIT "QUIT"

#define CODE_200 200 //Command OK
#define CODE_220 220 //Version
#define CODE_225 225 //Data connection
#define CODE_331 331 //Pedir Pass
#define CODE_230 230 // Loggin Ok
#define CODE_530 530 // Longin Incorrect
#define CODE_221 221 //Goodbye
#define CODE_550 550 //NO file
#define CODE_299 299 //File Size
#define CODE_226 226 //Transfer Complete

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
void get(int sd, char *file_name, struct sockaddr_in *local_structClient);
void quit(int sd);
void operate(int sd, struct sockaddr_in *local_structClient);

int create_socket(char *ip, struct sockaddr_in *local_address, char *port);
int connection_Server(int sd, struct sockaddr_in *server_struct);


#endif // FUNCTIONS_H_INCLUDED
