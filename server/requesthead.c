#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

#include "helpers/gettime.c"
#include "helpers/getfilesize.c"

int main(int argc, char *argv[]) {
    // Asegúrate de que se proporcionaron los argumentos necesarios
    if (argc < 3) {
        fprintf(stderr, "Uso: %s client_socket filename\n", argv[0]);
        exit(1);
    }

    // Convierte el argumento de client_socket de nuevo a un entero
    int client_socket = atoi(argv[1]);

    // El segundo argumento es el nombre del archivo
    char *filename = argv[2];

    printf("Archivo solicitado: %s\n", filename);
    char baseDir[] = "../";
    strcat(baseDir, filename);
    printf("Ruta del archivo: %s\n", baseDir);

    // abrir el archivo solicitado
    FILE *file = fopen(baseDir, "r");

    char *ext = strtok(filename, ".");
    ext = strtok(NULL, ".");
    printf("Extension: %s\n", ext);

    /*PREGUNTO SI SE PUEDE ABRIR EL ARCHIVO*/
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        char response[4096];
        sprintf(response,
                "HTTP/1.1 404 Not Found\r\n"
                "Server: ServidorSejin/1.0\r\n"
                "Content-Length: 0\r\n"
                "Date: %s\r\n"
                "Connection: close\r\n\r\n",
                getActualTime());

        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(0);
    } else {
        if (strcmp(ext, "html") == 0) {
            // Preparar la respuesta HTTP
            char response[4096];
            sprintf(response,
                    "HTTP/1.1 200 OK\r\n"
                    "Server: ServidorSejin/1.0\r\n"
                    "Content-Length: %ld\r\n"
                    "Connection: close\r\n"
                    "Date: %s\r\n"
                    "Content-Type: text/html\r\n\r\n",
                    getFileSize(baseDir), getActualTime());
            send(client_socket, response, strlen(response), 0);

            fclose(file);
            close(client_socket);
            return 0;
        } else if (strcmp(ext, "jpg") == 0) {
            // Preparar la respuesta HTTP
            char response[1024];
            sprintf(response,
                    "HTTP/1.1 200 OK\r\n"
                    "Server: ServidorSejin/1.0\r\n"
                    "Content-Length: %ld\r\n"
                    "Connection: close\r\n"
                    "Date: %s\r\n"
                    "Content-Type: image/jpeg\r\n\r\n",
                    getFileSize(baseDir), getActualTime());
            send(client_socket, response, strlen(response), 0);

            // Cerrar el archivo y el socket
            fclose(file);
            close(client_socket);
            return 0;
        } else {
            char response[4096];
            sprintf(response,
                    "HTTP/1.1 501 Not Implemented\r\n"
                    "Server: ServidorSejin/1.0\r\n"
                    "Date: %s\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n\r\n",
                    getActualTime());

            send(client_socket, response, strlen(response), 0);
            close(client_socket);
            exit(0);
        }
    }
}