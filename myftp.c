#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "functionsClient.h"
#include "functionsClient.c"


/** Run with -->./myftp <SERVER_IP> <SERVER_PORT> **/

int main (int argc, char *argv[]) {

    int sd, sd_Serv, socket_data;
    struct sockaddr_in connect_server, client_local;
    int len_serv = sizeof(connect_server);
    char buffer[BUFSIZE];
    int flag_data;

    int client_ = sizeof(client_local);

//**********************************************
//             arguments checking              //
    if (argc<3) {
        printf("You did not enter the IP of the server to connect and the PORT number\n");
        exit(1);
    }

//**********************************************
//     create socket and check for errors     //
    sd = create_socket( argv[1], &connect_server, argv[2]);

//**********************************************
//          To get local information           //
    client_local.sin_family = AF_INET;
    client_local.sin_addr.s_addr = htons(0);
    client_local.sin_port = INADDR_ANY;

//**********************************************
//          Connect and check errors           //
    sd_Serv = connection_Server(sd, &connect_server);


    getsockname(sd, (struct sockaddr*) &client_local, &client_);
    printf("My port is: %d\n", ntohs(client_local.sin_port));


//**********************************************
//         Read response from server           //

            if(recv_msg(sd, CODE_220, buffer)) {
                printf("Server Response: %s", buffer);
            }
            else{
                perror("Message could not be read");
                exit(1);
           }


// if receive hello proceed with authenticate and operate if not warning
    authenticate(sd);
    operate(sd, &client_local);


//**********************************************
//                 Close socket              //

    close(sd);

    return 0;

}
