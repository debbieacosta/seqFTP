#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BUFSIZE 512
#define cmd_USER "USER"
#define cmd_PASS "PASS"
#define cmd_PORT "PORT"
#define cmd_RETR "RETR"
#define cmd_QUIT "QUIT"

#define cmd_DIR "NLST"
#define cmd_CD "CWDR"
#define cmd_MKDIR "MKDR"
#define cmd_RMDIR "RMDR"
#define FILENAME_DIR "list.tmp"

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

/**
 * function: receive and analize the answer from the server
 * sd: socket descriptor
 * code: three leter numerical code to check if received
 * text: normally NULL but if a pointer if received as parameter
 *       then a copy of the optional message from the response
 *       is copied
 * return: result of code checking
 **/
bool recv_msg(int sd, int code, char *text) {
    char buffer[BUFSIZE], message[BUFSIZE], *token;
    int recv_s, recv_code;

    memset(buffer, 0, sizeof(buffer));
    memset(message, 0, sizeof(message));
    memset(text, 0, sizeof(text));

    // receive the answer
    recv_s = read(sd, buffer, sizeof(buffer));

    // error checking
    if (recv_s < 0) warn("error receiving data");
    if (recv_s == 0) errx(1, "connection closed by host");

    // parsing the code and message receive from the answer
    sscanf(buffer, "%d %[^\r\n]\r\n", &recv_code, message);
    //printf("%d %s\n", recv_code, message);

    // optional copy of parameters
    if(text) strcpy(text, buffer);

    // boolean test for the code
    return (code == recv_code) ? true : false;
}

/**
 * function: send command formated to the server
 * sd: socket descriptor
 * operation: four letters command
 * param: command parameters
 **/
void send_msg(int sd, char *operation, char *param) {
    char buffer[BUFSIZE] = "";

    // command formating
    if (param != NULL)
        sprintf(buffer, "%s %s\r\n", operation, param);
    else
        sprintf(buffer, "%s\r\n", operation);

    // send command and check for errors

 if(write(sd,buffer,sizeof(buffer)) < 0) {
            perror("ERROR writing socket");
                exit(1);
    }
}

/**
 * function: simple input from keyboard
 * return: input without ENTER key
 **/
char * read_input() {
    char *input = malloc(BUFSIZE);
char *read;
    if (fgets(input, BUFSIZE, stdin)) {
        return strtok(input, "\n");
    }
    return NULL;
}

/**
 * function: login process from the client side
 * sd: socket descriptor
 **/
void authenticate(int sd) {
    char *input, desc[100];
    int code;

    //Pide el usuario
    printf("username: ");
    input = read_input();

    // Envia el comando al servidor
    send_msg(sd,cmd_USER,input);

    // Libera Memoria
    free(input);

    //Espera respuesta para pedir password
    code = CODE_331;
    if (recv_msg(sd, code, desc)) {
        printf("Server Response: %s", desc);
        printf("passwd: ");
         input = read_input();

     send_msg(sd,cmd_PASS,input);

     free(input);

    code = CODE_230;
        if(recv_msg(sd,code,desc)){
            printf("Server Response: %s", desc);
            }
        else{
             printf("Server Response: %s", desc);
                close(sd);
                exit(1);
             }
        }
}
int creat_SocketData(struct sockaddr_in *local_structClient){
    int socketData, myPort;

    myPort = ntohs((*local_structClient).sin_port);
    (*local_structClient).sin_port = htons(myPort+1);

    socketData = socket(AF_INET, SOCK_STREAM,0);
    if(socketData < 0) {
           perror("Socket_Data creation failed...\n");
            exit(1);
        }

     if((bind (socketData, (struct sockaddr *)local_structClient, sizeof(*local_structClient))) < 0){
        perror("Socket_Data bind failed...\n");
        //asigno otro puerto a conectar
        (*local_structClient).sin_port = ntohs(0);

        if((bind (socketData, (struct sockaddr *)local_structClient, sizeof(*local_structClient))) < 0){
             perror("Socket_Data bind failed...\n");
             exit(EXIT_FAILURE);
    }

    else printf("Socket_Data successfully binded..\n");
    }

    else printf("Socket_Data successfully binded..\n");

    if ((listen(socketData, 1)) < 0){
        perror("Socket_Data Listen failed...\n");
        exit(EXIT_FAILURE);
        }
    else printf("Client is the new Server for file transfer and now listening..\n");

    return socketData;
}
void get_info(int sd, char *ip, int *port, struct sockaddr_in *local_structClient){

    int client_ = sizeof(*local_structClient);

    getsockname(sd, (struct sockaddr*)local_structClient, &client_);
    //printf("My port is: %d\n", ntohs((*local_structClient).sin_port));
    inet_ntop(AF_INET, &(local_structClient->sin_addr), ip, INET_ADDRSTRLEN);
    (*port) =  ntohs((*local_structClient).sin_port);

}

int accept_client(int sd, struct sockaddr_in *server_address, int *len_server_address) {
    int socket_;

     socket_ = accept(sd, (struct sockaddr *)server_address, (socklen_t*)len_server_address);
     if (socket_ < 0){
                perror("NO accept");
                exit(EXIT_FAILURE);
                }
    return socket_;
}
void recv_data(int sd, FILE *file, int f_size){
    int count;
    char buffer_data[BUFSIZE];
    memset(buffer_data, 0, BUFSIZE);
    buffer_data[512] = '\0';

    //receive the file
    while(1){
        //si el archivo es mayor a 512, leo de a 512
        if(f_size >= 512){
            count = read(sd, buffer_data,512);
           // printf("data: %s", buffer_data);
            fwrite(buffer_data, 1,512,file);

        }
        else{   //sino, es menor y leo el tama???o total
            if(f_size!=0){
                count = read(sd, buffer_data, f_size);
                //printf("data: %s", buffer_data);
                fwrite(buffer_data, 1,f_size,file);

            }
        }
        //si hubo error en la lectura del if(>512) o del else(<512)
        if(count == -1){
            printf("Data transfer failed!!...\n");
                    break;
        }

        //Si la lectura fue bien, escribo el archvivo

        memset(buffer_data, 0, BUFSIZE);
        buffer_data[512] = '\0';

        f_size = f_size - count;

        if(f_size<=0){
            break;
        }
     }
    }
void ipPort(char *bufferOut, char *ip, int port){
     char ip1[4], ip2[4], ip3[4], ip4[4];
     int firstByte, secondByte;
     char *token;

     memset(ip1, 0, sizeof(ip1));
     memset(ip2, 0, sizeof(ip2));
     memset(ip3, 0, sizeof(ip3));
     memset(ip4, 0, sizeof(ip4));

    token = strtok(ip, ".");
        strcpy(ip1, token);

    token = strtok(NULL, ".");
        if (token != NULL)
            strcpy(ip2, token);

    token = strtok(NULL, ".");
         if (token != NULL)
            strcpy(ip3, token);

    token = strtok(NULL, ".");
         if (token != NULL)
            strcpy(ip4, token);

    //printf("1: %s - 2: %s - 3: %s - 4: %s\n",ip1,ip2,ip3,ip4);

    firstByte = port >> 8;
    secondByte = port & 0x0FF;


    sprintf(bufferOut, "%s,%s,%s,%s,%d,%d", ip1,ip2,ip3,ip4,firstByte,secondByte);

 }
void send_cmdPort(int sd, char *buffer, char *ip, int *port){
        memset(buffer, 0, BUFSIZE);
        ipPort(buffer, ip, *port);
        //printf("Buffer: %s\n", buffer);

        send_msg(sd, cmd_PORT, buffer);

    }
/**
 * function: operation get
 * sd: socket descriptor
 * file_name: file name to get from the server
 **/
void total_socket_data(int sd, struct sockaddr_in *local_structClient, int f_size, char *file_name){

	int data_transfer = 0;
    struct sockaddr_in clienteServer_Data;
  
    int len_address = sizeof(*local_structClient);
    FILE *file;

	//***********************************************
	int sk_data, sock_serverData;
	char ip [INET_ADDRSTRLEN];
    int port;  
	 char desc[BUFSIZE], buffer[BUFSIZE];

	sk_data = creat_SocketData(local_structClient);

    //Get info and send to the server through command PORT
    get_info(sk_data,ip,&port,local_structClient);
    printf("Connection established with IP: %s y PORT: %d\n", ip, port);
    send_cmdPort(sd,desc,ip,&port); //send: PORT 127,0,0,1,4,150

	 //Accept Server as Client to transfer the data
    sock_serverData = accept_client(sk_data,&clienteServer_Data, &len_address);

     if(recv_msg(sock_serverData,CODE_225,buffer)) {
             printf("Server Response: %s", buffer);
        }

    // open the file to write

    file = fopen(file_name, "wb");
        if (NULL == file) {
            printf("File doesn't exists or cannot be opened\n");
    }

	
    //Recieve data from Server
    recv_data(sock_serverData,file,f_size);

    memset(buffer,0,BUFSIZE);
    if(recv_msg(sd,CODE_226,buffer)) { //Transfer OK
        printf("Server Response: %s", buffer);

    }
    else {
        printf("Server Response: %s", buffer);
    }

   

    // close the file and sockets
        fclose(file);
        close(sk_data);
        close(sock_serverData);

}

void get(int sd, char *file_name, struct sockaddr_in *local_structClient) {
    char desc[BUFSIZE], buffer[BUFSIZE];
    int f_size, recv_s, r_size = BUFSIZE;
    FILE *file;
    char *token;

    //variables for file transfer
    int data_transfer = 0;
    struct sockaddr_in clienteServer_Data;
    int sk_data, sock_serverData;
    int len_address = sizeof(*local_structClient);
    char ip [INET_ADDRSTRLEN];
    int port;

    //sprintf(desc, "%s %s\r\n",retr, file_name);

    sprintf(desc, "%s\r\n", file_name);

    // send the RETR command to the server
    send_msg(sd,cmd_RETR,desc);

    // check for the response
    memset(buffer, 0, BUFSIZE);

    if(recv_msg(sd,CODE_299,buffer)) { //File Found...
        printf("Server Response: %s", buffer);
        data_transfer = 1;
    }
    else {
        printf("Server Response: %s", buffer);

    }

    // parsing the file size from the answer received
    // "File %s size %ld bytes"
    token = strtok(buffer, " ");
    token = strtok(NULL, "\r\n");

    //get the size of the file
    sscanf(token, " File %*s size %d bytes", &f_size);
    //printf("ZISE %d\n", f_size);

//*****************************************************************************
//                                  TRANSFER DATA                           //

 while (data_transfer) {

    total_socket_data(sd, local_structClient, f_size, file_name);
 data_transfer = 0;
    }
}

/**
 * function: operation quit
 * sd: socket descriptor
 **/
void dir(int sd, struct sockaddr_in *local_structClient, char *dir_name){
    char buffer[BUFSIZE];
    char directory[BUFSIZE];
    int data = 0;
    int list = 0;
    char file_name[20];
    int f_size;
    memset(buffer,0,BUFSIZE);

    sprintf(directory, "%s\r\n", dir_name);

    send_msg(sd,cmd_DIR,directory);

    // Recibe la respuesta del Servidor
    if(recv_msg(sd, CODE_299, buffer)) {
                printf("Server Response: %s", buffer);
                data =1;
                list = 1;

    //get the size of the file
    sscanf(buffer, "299 File %s size %d bytes", file_name, &f_size);
    //printf("nombreee %s\n", file_name);
   
    while (data){
        total_socket_data(sd,local_structClient, f_size, file_name);
        data = 0;
    }
    
}
    
else{printf("Server Response: %s", buffer);}

if(list == 1){
    FILE *file;
    struct stat st;
    char line[BUFSIZE];

    
    file = fopen(file_name, "r");

  while(fgets(line, sizeof(line), file) != NULL) {
        fputs(line, stdout);
     }
 

fclose(file);

//Delete file...
  if(remove(file_name)!= 0){
      perror("Error deleting file\n");
       }

  }

}

void cd_dir(int sd, struct sockaddr_in *local_structClient, char *dir_name){
    char buffer[BUFSIZE];
    char directory[BUFSIZE];
    int data = 1;

    memset(buffer,0,BUFSIZE);
    memset(directory,0,BUFSIZE);

    sprintf(directory, "%s\r\n", dir_name);

    send_msg(sd,cmd_CD,directory);

     if(recv_msg(sd, CODE_200, buffer)) {
                printf("Server Response: %s", buffer);
    }
           else{
            printf("Server Response: %s", buffer);
        } 
}

void mkd(int sd, struct sockaddr_in *local_structClient, char *dir_name){
    char buffer[BUFSIZE];
    char directory[BUFSIZE];
    int data = 1;

    memset(buffer,0,BUFSIZE);
    memset(directory,0,BUFSIZE);

    sprintf(directory, "%s\r\n", dir_name);

    send_msg(sd,cmd_MKDIR,directory);

     if(recv_msg(sd, CODE_200, buffer)) {
             printf("Server Response: %s", buffer);
    }
    else{
            printf("Server Response: %s", buffer);
           }
}
void rmd(int sd, struct sockaddr_in *local_structClient, char *dir_name){
    char buffer[BUFSIZE];
    char directory[BUFSIZE];
    int data = 1;

    memset(buffer,0,BUFSIZE);
    memset(directory,0,BUFSIZE);

    sprintf(directory, "%s\r\n", dir_name);

    send_msg(sd,cmd_RMDIR,directory);

     if(recv_msg(sd, CODE_200, buffer)) {
                printf("Server Response: %s", buffer);
                }
        else{
            printf("Server Response: %s", buffer);
        }  
    
}

void quit(int sd) {

    // Send cmd QUIT to server
    char buffer[BUFSIZE];
    memset(buffer,0,BUFSIZE);

    send_msg(sd,cmd_QUIT,NULL);

    // Recibe la respuesta del Servidor
    if(recv_msg(sd,CODE_221,buffer)){
        printf("Server Response: %s", buffer);
    }
    else{
        printf("Server Response: %s", buffer);
    }
}

/**
 * function: make all operations (get|quit)
 * sd: socket descriptor
 **/
void operate(int sd, struct sockaddr_in *local_structClient) {
    char *input, *op, *param;

    while (true) {
        printf("Operation: ");
        input = read_input();

         if (input == NULL)
            continue; // avoid empty input
        op = strtok(input, " ");

    if (strcmp(op, "quit") != 0){
        if (strcmp(op, "get") == 0) {
            param = strtok(NULL, " ");
            get(sd, param, local_structClient);
        }

        if (strcmp(op, "dir") == 0) {
            param = strtok(NULL, " ");
            dir(sd,local_structClient,param);
        }

        if (strcmp(op, "cd") == 0) {
            param = strtok(NULL, " ");
            cd_dir(sd,local_structClient,param);
        }

        if (strcmp(op, "mkdir") == 0) {
            param = strtok(NULL, " ");
            mkd(sd,local_structClient,param);
        }

        if (strcmp(op, "rmdir") == 0) {
            param = strtok(NULL, " ");
            rmd(sd,local_structClient,param);
        }
    }
        else {
            if (strcmp(op, "quit") == 0) {
            quit(sd);
            break;
        }
        
        // Verificar si el comando es QUIT
        else {
            // new operations in the future
            printf("TODO: unexpected command\n");
        }
        }
        free(input);
        
}
                // free(input);
     free(input);
       
}

int create_socket(char *ip, struct sockaddr_in *local_address, char *port){

    int sd;
    sd = socket(AF_INET, SOCK_STREAM,0);
        if(sd < 0) {
            perror("Socket creation failed...\n");
            exit(1);
        }

        else printf("Socket successfully created..\n");

        //Assign IP & Port
        (*local_address).sin_family = AF_INET;
        (*local_address).sin_addr.s_addr = inet_addr(ip);
        (*local_address).sin_port = htons(atoi(port));

    return sd;
}

int connection_Server(int sd, struct sockaddr_in *server_struct) {
    int socket_;

     socket_ = connect(sd, (struct sockaddr *)server_struct, sizeof(*server_struct));
     if (socket_ < 0){
                perror("NO Connect");
                exit(EXIT_FAILURE);
                }
    return socket_;
}
