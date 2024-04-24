#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define REQUEST "POST / HTTP/1.1\r\nHost: localhost\r\n\r\n"

int main() {
    int sock;
    struct sockaddr_in server;
    char server_reply[4096];
    double total_response_time = 0.0;

    // Crear el socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error al crear el socket");
        exit(1);
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error al conectar al servidor");
        exit(1);
    }

    // Enviar 10 solicitudes HTTP GET
    for (int i = 0; i < 10; i++) {
        clock_t start_time = clock();

        // Enviar la solicitud al servidor
        if (send(sock, REQUEST, strlen(REQUEST), 0) < 0) {
            perror("Error al enviar la solicitud");
            exit(1);
        }

        // Recibir la respuesta del servidor
        if (recv(sock, server_reply, sizeof(server_reply), 0) < 0) {
            perror("Error al recibir la respuesta");
            exit(1);
        }

        clock_t end_time = clock();
        double response_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        total_response_time += response_time;

        printf("Respuesta %d recibida en %.2f segundos\n", i+1, response_time);
        printf("Respuesta:\n%s\n", server_reply);
    }

    double average_response_time = total_response_time / 10;
    printf("Tiempo de respuesta promedio: %.2f segundos\n", average_response_time);

    // Cerrar el socket
    close(sock);

    return 0;
}