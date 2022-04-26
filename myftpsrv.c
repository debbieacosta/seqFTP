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
#include "functionsServer.h"
#include "functionsServer.c"

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
          //break;
        }

        else {
        close(new_Client);
            }

    // close server socket

}

 close(sd);
    return 0;


}
