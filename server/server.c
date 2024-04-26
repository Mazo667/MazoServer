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

#define PORT 8000

// Creo la funcion sig_chld para que el proceso padre pueda esperar a que los hijos terminen
void sig_chld(int /*signal*/){
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf(" Hijo %d ha terminado\n", pid);
    return;
}

int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[4096];

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }
    // Permitir la reutilizion del PORT de escucha
    int option;
    option = SO_REUSEADDR;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Configurar la estructura de dirección del servidor
    memset((char *)&server_addr, 0, sizeof(server_addr)); //bzero despreciado

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("ERROR en bind");
    }

    listen(server_socket, 5);

    printf("Servidor listo para recibir conexiones en el PORT %d\n", PORT);

    signal(SIGCHLD, sig_chld);

    while (1){
        client_len = sizeof(client_addr);
        // Acepto la conexión del cliente
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0){
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }
        // Eliminar basura del buffer
        memset(buffer, 0, sizeof(buffer));

        printf("Conexión aceptada\n");

        // Creó el proceso hijo para atender al cliente
        if (fork() == 0) {
            int pid = getpid();
            printf("Proceso hijo creado %d id\n", pid);

            int n = read(client_socket, buffer, sizeof(buffer) - 1);

            if (n < 0){
                perror("Error en leyendo el socket");
            }

            printf("Tamaño en bytes del mensaje del cliente: (%d)\n", n);
            printf("Mensaje: %s\n", buffer);

            char *method = strtok(buffer, " "); // GET o HEAD
            char *filename = strtok(NULL, " "); // Nombre del archivo solicitado
            printf("filename: %s\n", filename);
            
            if (strcmp(method, "GET") == 0){
                        // Pregunto si solicito un archivo
                         if (filename == NULL || (strstr(filename, ".") == NULL)){
                            printf("No se solicitó un archivo\n");
                            char response[4096];
                            sprintf(response,
                                    "HTTP/1.1 403 Forbidden\r\n"
                                    "Server: ServidorSejin/1.0\r\n"
                                    "Content-Length: 0\r\n"
                                    "Date: %s\r\n"
                                    "Connection: close\r\n\r\n",
                                    getActualTime());
                            send(client_socket, response, strlen(response), 0);
                            close(client_socket);
                            exit(0);
                        }else{
                            char client_socket_str[12];
                            // Convierto el descriptor de archivo a una cadena
                            sprintf(client_socket_str, "%d", client_socket); 
                            char *args[] = {"./requestget", client_socket_str, filename, NULL};
                            execv(args[0], args);
                            exit(0);
                        }

            } else if (strcmp(method, "HEAD") == 0) {
                        // Pregunto si solicito un archivo
                        if (filename == NULL || (strstr(filename, ".") == NULL)){
                            printf("No se solicitó un archivo\n");
                            char response[4096];
                            sprintf(response,
                                    "HTTP/1.1 403 Forbidden\r\n"
                                    "Server: ServidorSejin/1.0\r\n"
                                    "Content-Length: 0\r\n"
                                    "Date: %s\r\n"
                                    "Connection: close\r\n\r\n",
                                    getActualTime());
                            // Envío la respuesta HTTP
                            send(client_socket, response, strlen(response), 0);
                            // Cierro la conexión
                            close(client_socket);
                            exit(0);
                        }else{
                            char client_socket_str[12];
                            sprintf(client_socket_str, "%d", client_socket); // Convertir el descriptor de archivo a una cadena
                            char *args[] = {"./requesthead", client_socket_str, filename, NULL};
                            execv(args[0], args);
                            exit(0);
                        }
            } else {
                printf("Metodo no soportado\n");
                char response[4096];
                sprintf(response,
                        "HTTP/1.1 501 Not Implemented\r\n"
                        "Server: ServidorSejin/1.0\r\n"
                        "Date: %s\r\n"
                        "Content-Length: 0\r\n"
                        "Connection: close\r\n\r\n",
                        getActualTime());
                // Envío la respuesta HTTP
                send(client_socket, response, strlen(response), 0);
                close(client_socket);
                exit(0);
            }

            // Cerrar el socket del cliente
            close(client_socket);
            exit(0);
        }
    }
    // Cerrar el socket del servidor
    close(server_socket);
    return 0;
}
