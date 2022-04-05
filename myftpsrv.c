#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 512
#define MSG_530 "530 Login incorrect\r\n"

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
    char buffer[BUFSIZE], message[BUFSIZE];
    int recv_s, recv_code;

    // receive the answer


    // error checking
    if (recv_s < 0) warn("error receiving data");
    if (recv_s == 0) errx(1, "connection closed by host");

    // parsing the code and message receive from the answer
    sscanf(buffer, "%d %[^\r\n]\r\n", &recv_code, message);
    printf("%d %s\n", recv_code, message);
    // optional copy of parameters
    if(text) strcpy(text, message);
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
    code = send(sd, input, sizeof(input), 0);
    if (code < 0){
           perror("No se pudo enviar mensaje");
        }

    // Libera Memoria
    free(input);

    //Espera respuesta para pedir password
    code = read(sd, desc, sizeof(desc));
     if (code < 0){
           perror("No se pudo leer el mensaje");
        }

        printf("Servidor Responde: %s\n", desc);


    // Pide el password
    printf("passwd: ");
    input = read_input();

    // Envia el comando al servidor
     code = send(sd, input, strlen(input), 0);
     if (code < 0){
            perror("No se pudo enviar mensaje");
        }

    // Libera memoria
    free(input);

    // Espera la respuesta del servidor
   desc[0]='\0';

    code = recv(sd, desc, sizeof(desc), 0);
    if (code < 0){
            perror("No se pudo leer el mensaje");
        }

        printf("Servidor Responde: %s\n", desc);

    //si la autenticacion no es correcta, se cierra la conexion
        if(strcmp(desc,MSG_530)==0) {
            close(sd);
            exit(1);
        }

}

/** SIN USAR TODAVIA
 * function: operation get
 * sd: socket descriptor
 * file_name: file name to get from the server
 **/
void get(int sd, char *file_name) {
    char desc[BUFSIZE], buffer[BUFSIZE];
    int f_size, recv_s, r_size = BUFSIZE;
    FILE *file;

    // send the RETR command to the server

    // check for the response

    // parsing the file size from the answer received
    // "File %s size %ld bytes"
    sscanf(buffer, "File %*s size %d bytes", &f_size);

    // open the file to write
    file = fopen(file_name, "w");

    //receive the file



    // close the file
    fclose(file);

    // receive the OK from the server

}

/**
 * function: operation quit
 * sd: socket descriptor
 **/

void quit(int sd) {

    // Envia el comando QUIT al servidor
    char *buffer = "QUIT\r\n";
    char resp[BUFSIZE]; //Almacenar la respuesta del servidor y mostrarla

    if(send(sd,buffer,sizeof(buffer), 0) < 0) {
            perror("ERROR al escribir en el socket");
                exit(1);
    }

    // Recibe la respuesta del Servidor
    recv(sd, resp, sizeof(resp), 0);
        printf("Servidor Responde: %s", resp);
}

/**
 * function: make all operations (get|quit)
 * sd: socket descriptor
 **/
void operate(int sd) {
    char *input, *op, *param;

    while (true) {
        printf("Operation: ");
        input = read_input();

        if (input == NULL)
            continue; // avoid empty input
        op = strtok(input, " ");

                // free(input);
        if (strcmp(op, "get") == 0) {
            param = strtok(NULL, " ");
            get(sd, param);
        }
        // Verificar si el comando es QUIT
        else if (strcmp(op, "quit") == 0) {
            quit(sd);
            break;
        }
        else {
            // new operations in the future
            printf("TODO: unexpected command\n");
        }
        free(input);
    }
    free(input);
}


/** Run with -->./myftp <SERVER_IP> <SERVER_PORT> **/

int main (int argc, char *argv[]) {

    int sd,n;
    struct sockaddr_in client;
    char resp[BUFSIZE];

    // arguments checking
    if (argc<3) {
        printf("Ingrese la ip del servidor a conectar y el numero de puerto\n");
        exit(1);
    }

    // create socket and check for errors

    sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd < 0) {
            perror("No se pudo crear el Socket");
            exit(1);
        }

    // Establecer datos del socket

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(*(argv+1));
    client.sin_port = htons(atoi(argv[2]));

    // Conectar y verificar errores

    if(connect(sd, (struct sockaddr *)&client, sizeof(client)) < 0){
        perror("No se pudo conectar al servidor");
        exit(2);
    }

    // Si no hay errores, lee la entrada del servidor

    else { if(recv(sd,resp, sizeof(resp),0) < 0) {
            perror("No se pudo leer el mensaje");
            exit(1);
            }

        printf("Servidor responde: %s \n", resp);
        }

    // if receive hello proceed with authenticate and operate if not warning
    authenticate(sd);
    operate(sd);

    // close socket
    close(sd);

    return 0;
}
