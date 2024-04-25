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

#include "helpers/getTime.c"
#include "helpers/getFileSize.c"

int main(int argc, char *argv[])
{
    // Asegúrate de que se proporcionaron los argumentos necesarios
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s client_socket filename\n", argv[0]);
        exit(1);
    }

    // Convierte el argumento de client_socket de nuevo a un entero
    int client_socket = atoi(argv[1]);

    // El segundo argumento es el nombre del archivo
    char *filename = argv[2];

    printf("Archivo solicitado: %s\n", filename);
    char baseDir[] = "/home/maximiliano/Descargas/";
    strcat(baseDir, filename);
    printf("Ruta del archivo: %s\n", baseDir);

    // abrir el archivo solicitado
    FILE *file = fopen(baseDir, "r");

    char *ext = strtok(filename,"."); 
    ext = strtok(NULL,"."); 
    printf("Extension: %s\n", ext);

    /*PREGUNTO SI SE PUEDE ABRIR EL ARCHIVO*/
    if (file == NULL)
    {
        printf("No se pudo abrir el archivo\n");
        char response[4096];
        sprintf(response,
                "HTTP/1.1 404 Not Found\r\n"
                "Server: ServidorSejin/1.0\r\n"
                "Content-Length: 0\r\n"
                "Date: %s\r\n"
                "Connection: close\r\n\r\n",
                getActualTime());

        write(client_socket, response, strlen(response));
        close(client_socket);
        exit(0);
    }
    else
    {
        if(strcmp(ext,"html") == 0){
        // Mover el puntero al final del archivo
        fseek(file, 0, SEEK_END);
        // Obtener la longitud del archivo
        long file_length = ftell(file);
        // Mover el puntero de nuevo al inicio del archivo
        fseek(file, 0, SEEK_SET);

        char response[4096];
        sprintf(response,
                "HTTP/1.1 200 OK\r\n"
                "Server: ServidorSejin/1.0\r\n"
                "Content-Length: %ld\r\n"
                "Connection: close\r\n"
                "Date: %s\r\n"
                "Content-Type: text/html\r\n\r\n",
                getFileSize(baseDir), getActualTime());
        write(client_socket, response, strlen(response));

        fclose(file);
        close(client_socket);
        return 0;
        // }else if(strcmp(ext,"jpg") == 0){
        //     // Preparar la respuesta HTTP
        //     char reply[44] = 
        //     "HTTP/1.1 200 OK\n"
        //     "Content-Type: image/jpeg\n"
        //     "\n";
        //     // Enviar la cabecera HTTP al cliente
        //     send(client_socket, reply, strlen(reply), 0);

        //     // Preparar el buffer para leer el archivo
        //     char buffer[1024];
        //     int count;

        //     // Leer el archivo y enviarlo al cliente
        //     while (!feof(file)) {
        //         count = fread(buffer, sizeof(char), sizeof(buffer), file);
        //         send(client_socket, buffer, count, 0);
        //         memset(buffer, 0, sizeof(buffer));
        //     }

        //     // Cerrar el archivo y el socket
        //     fclose(file);
        //     close(client_socket);
        //     return 0;
        }else{
            char response[4096];
            sprintf(response,
                    "HTTP/1.1 501 Not Implemented\r\n"
                    "Server: ServidorSejin/1.0\r\n"
                    "Date: %s\r\n"
                    "Content-Length: 0\r\n"
                    "Connection: close\r\n\r\n",
                    getActualTime());

            write(client_socket, response, strlen(response));
            close(client_socket);
            exit(0);
        }
        
    }
}