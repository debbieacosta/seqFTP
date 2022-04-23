#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdarg.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <err.h>

#include <netinet/in.h>

#define BUFSIZE 512
#define CMDSIZE 4
#define PARSIZE 100
#define DATASIZE 512

#define cmd_USER "USER"
#define cmd_PASS "PASS"
#define cmd_PORT "PORT"
#define cmd_RETR "RETR"


#define MSG_200 "200 Command Recieve\r\n"
#define MSG_202 "202 Command not implemented\r\n"
#define MSG_220 "220 srvFtp version 1.0\r\n"
#define MSG_331 "331 Password required for %s\r\n"
#define MSG_225 "225 Data connection open\r\n"
#define MSG_425 "425 Cant open data connection\r\n"
#define MSG_426 "426 Transfer aborted\r\n"
#define MSG_230 "230 User %s logged in\r\n"
#define MSG_530 "530 Login incorrect\r\n"
#define MSG_221 "221 Goodbye\r\n"
#define MSG_550 "550 %s: no such file or directory\r\n"
#define MSG_299 "299 File %s size %ld bytes\r\n"
#define MSG_226 "226 Transfer complete\r\n"


/**
 * function: receive the commands from the client
 * sd: socket descriptor
 * operation: \0 if you want to know the operation received
 *            OP if you want to check an especific operation
 *            ex: recv_cmd(sd, "USER", param)
 * param: parameters for the operation involve
 * return: only usefull if you want to check an operation
 *         ex: for login you need the seq USER PASS
 *             you can check if you receive first USER
 *             and then check if you receive PASS
 **/

bool recv_cmd(int sd, char *operation, char *param) {
   // printf("Entrooooooooo a leer\n");
    
    char buffer[BUFSIZE], *token;
    char aux[CMDSIZE];
    int recv_s;
    memset(buffer,0,BUFSIZE);

    // receive the command in the buffer and check for errors

    read(sd, buffer, BUFSIZE);
    printf("BUFFER = %s\n",buffer);

    // expunge the terminator characters from the buffer
    buffer[strcspn(buffer, "\r\n")] = 0;

    // complex parsing of the buffer
    // extract command receive in operation if not set \0
    // extract parameters of the operation in param if it needed
    token = strtok(buffer, " ");

    if (token == NULL || strlen(token) < 4) {
        warn("not valid ftp command");
        return false;
        } 
   
    else {
        if (operation[0] == '\0') {
        strcpy(operation, token);
        }
        if (strcmp(operation, token)) {
            warn("abnormal client flow: did not send %s command", operation);
            return false;
        }

        token = strtok(NULL, " ");
         
         if (token != NULL) 
         strcpy(param, token); 
         }
    return true;
}

/** VEEEEER PERROR
 * function: send answer to the client
 * sd: file descriptor
 * message: formatting string in printf format
 * ...: variable arguments for economics of formats
 * return: true if not problem arise or else
 * notes: the MSG_x have preformated for these use
 **/
bool send_ans(int sd, char *message, ...){
    char buffer[BUFSIZE];
    va_list args;

    va_start(args, message);

    vsprintf(buffer, message, args);

    va_end(args);

    // send answer preformated and check errors
    if(write(sd, buffer, sizeof(buffer))< 0){
            //perror("ERROR writing socket");
                return false;
    }
    else  return true;
}
void ip_PortClient(char* buffer, char *ip, int *port){
    char ip1[4], ip2[4], ip3[4], ip4[4];
    char firstByte[10], secondByte[10];
    int port_firstByte, port_secondByte;
    char *token;

     memset(ip1, 0, sizeof(ip1));
     memset(ip2, 0, sizeof(ip2));
     memset(ip3, 0, sizeof(ip3));
     memset(ip4, 0, sizeof(ip4));
     memset(firstByte, 0, sizeof(firstByte));
     memset(secondByte, 0, sizeof(secondByte));
    
    token = strtok(buffer, ",");  //127,0,0,1,4,150
        if (token != NULL)
            strcpy(ip1, token);

    token = strtok(NULL, ",");//0,0,1,4,150
         if (token != NULL)
            strcpy(ip2, token);

    token = strtok(NULL, ",");//0,1,4,150
         if (token != NULL)
            strcpy(ip3, token);

    token = strtok(NULL, ",");//1,4,150
         if (token != NULL)
            strcpy(ip4, token);
    
    token = strtok(NULL, ",");//4,150
         if (token != NULL)
            strcpy(firstByte, token);
    
    token = strtok(NULL, ",");//150
         if (token != NULL)
            strcpy(secondByte, token);

sprintf(ip, "%s.%s.%s.%s", ip1,ip2,ip3,ip4);

port_firstByte = atoi(firstByte) << 8;
port_secondByte = atoi(secondByte);

*port = port_firstByte + port_secondByte;

//printf("Puerto = %d\n", *port);

}

int connection_Server(int sd, struct sockaddr_in *x) {
    int socket_;
  
     socket_ = connect(sd, (struct sockaddr *)x, sizeof(*x));
     if (socket_ < 0){
                perror("NO Connect");
                exit(EXIT_FAILURE);
                }
    return socket_;
}
/**
 * function: RETR operation
 * sd: socket descriptor
 * file_path: name of the RETR file
 **/

void retr(int sd, char *file_path) {
    FILE *file;
    int bread;
    long fsize;
    char buffer[BUFSIZE];
    char buffer_Data[BUFSIZE];

 

    //file = file_path;
    // check if file exists if not inform error to client
    file = fopen(file_path, "r");

     if (NULL == file) {
        printf("File doesn't exists or cannot be opened\n");
        if(send_ans(sd,MSG_550, file_path, fsize)){
            printf("File %s found...\n", file_path);
        }
        else{
            perror("ERROR writing socket");
             exit(1);
        }
    }
    
    // send a success message with the file length
    else{
        fseek(file, 0, SEEK_END);
        fsize = ftell(file);
        rewind(file);
        fclose(file); 
       
        if(send_ans(sd,MSG_299, file_path, fsize)){
            printf("File %s found...\n", file_path);
        }
        else{
             perror("ERROR writing socket");
              exit(1);
        }
    }
}

bool send_file(int sd, FILE * file_){
     
char buffer_Data[BUFSIZE];
fseek(file_, 0, SEEK_END);
int fsize = ftell(file_);
rewind(file_);


//Agarro el total del archivo
int transf_size = fsize;

memset(buffer_Data, 0, 512);
buffer_Data[512] = '\0';

while(transf_size >= 0){

    //VEO SI EL ARCHIVO ES MENOR A 512kb
    if(transf_size <= 512){ 
        while(fread(buffer_Data,1,transf_size,file_)){
           printf("data: %s", buffer_Data);

            if(write(sd,buffer_Data,transf_size) < 0){
                perror("Error sending file...\n");
                exit(1);
                return false;
            }

            memset(buffer_Data, 0, BUFSIZE);
            buffer_Data[512] = '\0';
             
        }
        
        //CAMBIO EL VALOR DE TRANSF_SIZE A -1 PARA ROMPER EL WHILE UNA VEZ TERMINADA LA TRASNFERENCIA
      transf_size = -1;
    }
//EL ARCHIVO ES MAYOR A 512kb
    else { 

        while(fread(buffer_Data,1,512,file_)){
            
            printf("sending data = %s\n", buffer_Data);

            if(write(sd,buffer_Data,BUFSIZE) < 0){
                perror("Error sending file...\n");
                exit(1);
             return false;
            }
            printf("todo ok sending.. +512\n");
        
        memset(buffer_Data, 0, BUFSIZE);
        buffer_Data[512] = '\0';
        

        }
        //RESTO AL TOTAL DEL ARCHIVO, LO YA LEIDO
        transf_size = transf_size - 512;

        if(transf_size < 0){
            transf_size = 512 + transf_size;
        }
        
    }
    return true;
    
}


}

void port(int sd, char *parame, char *filename){
    //Variables para canal de datos
    int sk_Data;
    char ip_skData[INET_ADDRSTRLEN];
    int port_Data;
    struct sockaddr_in server_data;

    //Archivo a enviar
    FILE *file;

   // printf("Entre a PORT param = %s\n", parame);

    //FORMATEO RESPUESTA DEL CLIENTE PARA CREAR EL SOCKET PARA DATA
    ip_PortClient(parame,ip_skData,&port_Data);

    //SETEO LOS DATOS
        server_data.sin_family = AF_INET;
        server_data.sin_addr.s_addr = inet_addr(ip_skData);
        server_data.sin_port = htons(port_Data);

    //CREO SOCKET PARA DATOS
        sk_Data = socket(AF_INET, SOCK_STREAM,0);
        if(sk_Data < 0) {
            perror("Error Creating socket");
            exit(1);
        }

        //CONECTO CON EL CLIENTE(AHORA SERVIDOR)
        if(connect(sk_Data, (struct sockaddr *)(&server_data), sizeof(server_data)) < 0){
                perror("NO Connect to Server Data");
                exit(EXIT_FAILURE);
                }

            if (send_ans(sk_Data,MSG_225)){
                   printf("Connection Data Open..\n");
                   }
            else {
                perror("Error writing socket");
                exit(1);
                }


    //EMPIEZO TRANSMISION DEL ARCHIVO
    // printf("Fileee %s\n", filename);
    file = fopen(filename, "rb");

     if (NULL == file) {
        printf("File doesn't exists or cannot be opened\n");
    }

    if(send_file(sk_Data, file)){
        if (send_ans(sd,MSG_226)){
                printf("file sent successfully...\n");
                   }
            else {
                perror("Error writing socket");
                exit(1);
                }
    }
    else{
        if (send_ans(sd,MSG_426)){
                printf("Tansfer Incorrect...\n");
                   }
            else {
                perror("Error writing socket");
                exit(1);
                }
        }

    fclose(file);
}


   

/**
 * funcion: check valid credentials in ftpusers file
 * user: login user name
 * pass: user password
 * return: true if found or false if not
 **/
bool check_credentials(char *user, char *pass) {
    FILE *file;
    char *path = "./ftpusers.txt", *line = NULL, cred[100];
    size_t len = 0;
    bool found = false;


    // make the credential string
    strcpy(cred,user);
    strcat(cred,":");
    strcat(cred,pass);
    strcat(cred,"\n");

    // check if ftpusers file it's present
    file = fopen(path, "r");

     if (NULL == file) {
        printf("File doesn't exists or cannot be opened\n");
    }

    // search for credential string
    while ((len = getline(&line, &len, file)) != -1) {
         if(strcmp(line, cred) == 0){
             fclose(file);
                return true;
             }
         }

    // close file and release any pointes if necessary
    fclose(file);

    // return search status
        return false;
}

/**
 * function: login process management
 * sd: socket descriptor
 * return: true if login is succesfully, false if not
 **/
bool authenticate(int sd) {
    char user[PARSIZE], pass[PARSIZE];

        memset(user,0,PARSIZE);
        memset(pass,0,PARSIZE);

    // wait to receive USER action
    if (recv_cmd(sd,cmd_USER,user)<0){
            perror("Message could not be read\n");
                exit(1);
       }

    // ask for password 
    if (send_ans(sd,MSG_331,user)){
        printf("Waiting Password from Client: %s...\n", user);
        }
    else {
        perror("ERROR al escribir en el socket");
        exit(1);
        }

    // wait to receive PASS action
    if (recv_cmd(sd,cmd_PASS,pass)<0){
            perror("Message could not be read\n");
       }

    // if credentials don't check denied login
    if (check_credentials(user,pass)!=true){
    
            if (send_ans(sd,MSG_530,user)){
                 printf("Credential Incorrect...\n");
            }
            else {
                perror("ERROR writing socket");
                exit(1);

            }
        
         return false;
    }

    // confirm login
    else { 
           if (send_ans(sd,MSG_230,user)){
                 printf("Credential OK...\n");
            }
            else {
                perror("ERROR writing socket");
                exit(1);
            }
        return true;
    }
}

/**
 *  function: execute all commands (RETR|QUIT)
 *  sd: socket descriptor
 **/

void operate(int sd) {
//char op[CMDSIZE], param[PARSIZE];
char  *op = malloc(4 * sizeof(char));
char *param = malloc(100 * sizeof(char));
char file_name[32];

//memset(op, 0, CMDSIZE);
//memset(param, 0,PARSIZE);

    while (true) {
        memset(op, 0, CMDSIZE);
        memset(param, 0,PARSIZE);
        //memset(file_name, 0,32);

        op[0] = param[0] = '\0';
        // check for commands send by the client if not inform and exit

        if(recv_cmd(sd,op,param) != true) perror("ERROR socket");
        
        //printf("\nAca empezaria la funcion de........\n");
        //printf("ope: %s\n",op);
        //printf("param: %s\n",param);
            
        if (strcmp(op, "RETR") == 0) {
                retr(sd, param);
                strcpy(file_name, param);
                //printf("Sali de retr\n");

        } 
        if (strcmp(op, "PORT") == 0) {
               // retr(sd, param);
                port(sd,param,file_name);
                printf("Sali de port\n");
            } 
        else
            if (strcmp(op, "QUIT") == 0) {

            // send goodbye and close connection
                if (send_ans(sd,MSG_221)){
                    printf("Client says quit..\n");
                    exit(1);
                }
                else{
                    perror("ERROR writing socket");
                    exit(1);
                }
                    break;

        } else {   
            // invalid command

            
            // furute use
        }
    }
}


int creat_socket(char *ip, struct sockaddr_in *adress_struct, char *port){

    int sd;
    sd = socket(AF_INET, SOCK_STREAM,0);
        if(sd < 0) {
            perror("Socket creation failed...\nt");
            exit(1);
        }
        else printf("Socket successfully created..\n");

        //Assign ip and port
        (*adress_struct).sin_family = AF_INET;
        (*adress_struct).sin_addr.s_addr = inet_addr(ip);
        (*adress_struct).sin_port = htons(atoi(port));

    //**************   BIND *******************
        if((bind (sd, (struct sockaddr *)adress_struct, sizeof(*adress_struct))) < 0){
        perror("Socket bind failed...\n");
        exit(EXIT_FAILURE);
        }
        else printf("Socket successfully binded..\n");

     //**************   LISTEN *******************
    if ((listen(sd, 1)) < 0){
        perror("Listen failed...\n");
        exit(EXIT_FAILURE);
        }
    else printf("Server listening..\n");

    
    return sd;
}
int accept_client(int sd, struct sockaddr_in *adress_struct, int *len_adress_struct) {
    int socket_;
  
     socket_ = accept(sd, (struct sockaddr *)adress_struct, (socklen_t*)len_adress_struct);
     if (socket_ < 0){
                perror("NO accept...");
                exit(EXIT_FAILURE);
                }
    return socket_;
}


/** Run --> ./mysrv <SERVER_PORT> **/
int main (int argc, char *argv[]) {
     int op = 1;

    // arguments checking
    if (argc<2) {
        printf("You did not enter the port of the server to connect\n");
        exit(1);
    }
    // reserve sockets and variables space
    int sd, new_Client;
    struct sockaddr_in address_local, client;
    int len_address = sizeof(address_local);


    sd = creat_socket("127.0.0.1", &address_local, argv[1]);
    
    // main loop
   
    while(op){
        printf("Waiting Clients..\n");
       
        // accept connectiones sequentially and check errors
        new_Client= accept_client(sd,&client, &len_address);
                

        // send hello
         if (send_ans(new_Client,MSG_220)){
            printf("New Client Connected..\n");
            }
        else{
            perror("Message could not be sent...\n");
            exit(1);
        }
        
//*********************************************************************
//                            CLIENT DATA                           //
char ip [INET_ADDRSTRLEN];
inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
printf("Connection established with IP: %s y PORT: %d\n", ip, ntohs(client.sin_port));
//*********************************************************************

//*******************************************************************

        // operate only if authenticate is true
        printf("Waiting Autentication..\n");

        if(authenticate(new_Client)==true){ 
          operate(new_Client);
          break;
        }

        else { 
        close(new_Client);
            }

    // close server socket
   
}

 //close(sd);
    return 0;


}