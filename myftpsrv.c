#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <err.h>

#include <netinet/in.h>

#define BUFSIZE 512
#define CMDSIZE 4
#define PARSIZE 100

#define MSG_220 "220 srvFtp version 1.0\r\n"
#define MSG_331 "331 Password required for %s\r\n"
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
    char buffer[BUFSIZE], *token;
    int recv_s;

    // receive the command in the buffer and check for errors



    // expunge the terminator characters from the buffer
    buffer[strcspn(buffer, "\r\n")] = 0;

    // complex parsing of the buffer
    // extract command receive in operation if not set \0
    // extract parameters of the operation in param if it needed
    token = strtok(buffer, " ");
    if (token == NULL || strlen(token) < 4) {
        warn("not valid ftp command");
        return false;
    } else {
        if (operation[0] == '\0') strcpy(operation, token);
        if (strcmp(operation, token)) {
            warn("abnormal client flow: did not send %s command", operation);
            return false;
        }
        token = strtok(NULL, " ");
        if (token != NULL) strcpy(param, token);
    }
    return true;
}

/**
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
    if(send(sd, buffer, strlen(buffer)+1, 0) < 0){
        perror("RROR al escribir en el socket");
            return false;
    }
        else {
            return true;}
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

    // check if file exists if not inform error to client

    // send a success message with the file length

    // important delay for avoid problems with buffer size
    sleep(1);

    // send the file

    // close the file

    // send a completed transfer message
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
        printf("No se pudo abrir el archivo\n");
    }

    // search for credential string
    int a;

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

     if (recv(sd, user, sizeof(user),0) < 0){
            perror("No se pudo leer el mensaje");
        }

    // ask for password
     if (send_ans(sd,MSG_331,user) < 0){
                perror("ERROR al escribir en el socket");
                exit(1);
                }

    // wait to receive PASS action
    if (recv(sd, pass, sizeof(pass),0) < 0){
            perror("No se pudo leer el mensaje");
        }

    // if credentials don't check denied login
    if (check_credentials(user,pass)!=true){

        printf("Credencial Incorrecta, se procede a desconectar\n");
            if ( send_ans(sd,MSG_530,user) < 0){
                perror("ERROR al escribir en el socket");
                exit(1);
                }
                return false;
    }

    // confirm login
    else {
        printf("Credencial Correcta\n");
            if (send_ans(sd,MSG_230,user) < 0){
                perror("ERROR in writing to socket");
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
    char op[CMDSIZE], param[PARSIZE];

    while (true) {
        op[0] = param[0] = '\0';

        // check for commands send by the client if not inform and exit

         if (recv(sd, op, CMDSIZE, 0) < 0){
            perror("ERROR socket");
            exit(1);
        }
        if (strcmp(op, "RETR") == 0) {
            retr(sd, param);
        } else if (strcmp(op, "QUIT") == 0) {
            // send goodbye and close connection

            if (send_ans(sd,MSG_221) < 0){
                perror("ERROR al enviar respuesta");
                exit(1);
                }
                    break;

        } else {
            // invalid command
            // furute use
        }
    }
}


/** Run --> ./mysrv <SERVER_PORT> **/
int main (int argc, char *argv[]) {

    // arguments checking

    if (argc<2) {
        printf("Ingrese el puerto del servidor a conectar\n");
        exit(1);
    }
    // reserve sockets and variables space
    int sd, new_socket;
    struct sockaddr_in server;
    int serverlen = sizeof(server);


    // create server socket and check errors
      sd = socket(AF_INET, SOCK_STREAM,0);
        if(sd < 0) {
            perror("No se puede crear el socket");
            exit(1);
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(atoi(argv[1]));

    // bind master socket and check errors
    if(bind (sd, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Bind fallo");
        exit(EXIT_FAILURE);
        }

    // make it listen
    if (listen(sd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
        }

  char buffer[4];

    // main loop
    while (true) {

        // accept connectiones sequentially and check errors
        if ((new_socket = accept(sd, (struct sockaddr *)&server, (socklen_t*)&serverlen))<0){
                perror("accept");
                exit(EXIT_FAILURE);
                }

        // send hello
        else{
                if (send_ans(new_socket,MSG_220) < 0){
                  perror("No se puede enviar la respuesta");
                  exit(1);
            }

        // operate only if authenticate is true
        printf("Esperando Autenticacion..\n");

        if(authenticate(new_socket)==true){
          operate(new_socket);
          break;
        }

        else {
        close(new_socket);
            break;
        }

    }
    }
    // close server socket
    close(sd);

    return 0;
}
