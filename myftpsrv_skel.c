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
    int n;

    va_list args;
    va_start(args, message);


    vsprintf(buffer, message, args);


    va_end(args);
    // send answer preformated and check errors



  //  printf("The string is:  %s\n", buffer);

    n = write(sd, buffer, BUFSIZE);

    if (n < 0){
            perror("ERROR in writing to socket");
            return false;
        }

        else {
                return true;

}}

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
    char *path = "./ftpusers", *line = NULL, cred[100];
    size_t len = 0;
    bool found = false;

    // make the credential string

    // check if ftpusers file it's present

    // search for credential string

    // close file and release any pointes if necessary

    // return search status
}

/**
 * function: login process management
 * sd: socket descriptor
 * return: true if login is succesfully, false if not
 **/
bool authenticate(int sd) {
    char user[PARSIZE], pass[PARSIZE];

    // wait to receive USER action

    // ask for password

    // wait to receive PASS action

    // if credentials don't check denied login

    // confirm login
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

        sd = read( sd , op, 4);
         if (sd < 0){
            perror("ERROR in reading from socket");
            exit(1);
        }
        if (strcmp(op, "RETR") == 0) {
            retr(sd, param);
        } else if (strcmp(op, "quit") == 0) {
            // send goodbye and close connection
            send_ans(sd,MSG_221);
            //sd = write(sd, op, strlen(op));

        if (sd < 0){
            perror("ERROR in writing to socket");
            exit(1);
        }
            break;

        } else {
            // invalid command
            // furute use
        }
    }
}


/**
 * Run with
 *         ./mysrv <SERVER_PORT>
 **/
int main (int argc, char *argv[]) {

    // arguments checking

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
       // printf("Server::  About to set %hu as the port...\n", (unsigned short)strtoul(argv[1], NULL, 0));
        server.sin_port = htons(atoi(argv[1]));
    // bind master socket and check errors
    if(bind (sd, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("Bind fallo");
    exit(EXIT_FAILURE);
    }

 //   printf("Server::  About to listen on port %hu...\n", ntohs(server.sin_port));

    // make it listen
    if (listen(sd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

  char buffer[4];

    // main loop
    while (true) {
        // accept connectiones sequentially and check errors
        if ((new_socket = accept(sd, (struct sockaddr *)&server,
                       (socklen_t*)&serverlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else{
       // send(new_socket, MSG_220, sizeof(MSG_220),0);
       send_ans(new_socket,MSG_220);
    }

    operate(new_socket);
     break;

    }

        // send hello

        // operate only if authenticate is true


    // close server socket
    close(sd);

    return 0;
}
