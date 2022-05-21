#ifndef FUNCTIONSSERVER_H_INCLUDED
#define FUNCTIONSSERVER_H_INCLUDED




bool recv_cmd(int sd, char *operation, char *param);
bool send_ans(int sd, char *message, ...);
void ip_PortClient(char* buffer, char *ip, int *port);
int connection_Server(int sd, struct sockaddr_in *x);
void retr(int sd, char *file_path);
bool send_file(int sd, FILE * file_);
void port(int sd, char *parame, char *filename);
bool check_credentials(char *user, char *pass);
bool authenticate(int sd);
int getFileSize(char *filename);
void dir(int sd, char* directory);
void cd_dir(int sd, char* directory);
void mk_dir(int sd, char* directory);
void rm_dir(int sd, char* directory);
void operate(int sd);
int creat_socket(char *ip, struct sockaddr_in *adress_struct, char *port);
int accept_client(int sd, struct sockaddr_in *adress_struct, int *len_adress_struct);


#endif // FUNCTIONSSERVER_H_INCLUDED
