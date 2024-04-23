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

#define PUERTO 8000

//Creo la funcion sig_chld para que el proceso padre pueda esperar a que los hijos terminen
void sig_chld(int signo){
	pid_t	pid;
	int	stat;
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf(" Hijo %d ha terminado\n", pid);
	return;
}

int main() {
    int servidor_socket, cliente_socket;
    struct sockaddr_in servidor_addr, cliente_addr;
    socklen_t cliente_len;
    char buffer[4096];

    // Crear el socket del servidor
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }
    //Permitir la reutilizion de la direccion del socket
    int option;
    option = SO_REUSEADDR;
    setsockopt(servidor_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Configurar la estructura de dirección del servidor
    bzero((char*) &servidor_addr,sizeof(servidor_addr));

    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    servidor_addr.sin_port = htons(PUERTO);


      if (bind(servidor_socket, (struct sockaddr *) &servidor_addr, sizeof(servidor_addr)) < 0) //Bind the socket to the server address
              perror("ERROR on binding");

    listen(servidor_socket, 5);

    printf("Servidor listo para recibir conexiones en el puerto %d\n", PUERTO);
    
    signal(SIGCHLD,sig_chld);

    while (1) {
    cliente_len = sizeof(cliente_addr);
    
    cliente_socket = accept(servidor_socket, (struct sockaddr*) &cliente_addr, &cliente_len);
    if (cliente_socket < 0) {
        perror("Error al aceptar la conexión");
        exit(EXIT_FAILURE);
    }

    printf("Conexión aceptada\n");
    
    if(fork()==0){
    int pid = getpid();
    printf("Proceso hijo creado %d id\n",pid);
    // Cerrar el socket del servidor en el proceso hijo
    close(servidor_socket);

    // Leer la solicitud del cliente
    ssize_t bytes_recibidos = recv(cliente_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_recibidos < 0) {
        perror("Error al leer la solicitud del cliente");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_recibidos] = '\0'; // Agregar el carácter nulo al final del buffer

    //printf("Solicitud del cliente:\n%s\n", buffer);

    // Determinar qué programa ejecutar según la solicitud del cliente
    if (strstr(buffer, "GET") != NULL) {
        char *pedido_get = "./pedido_get"; // Ruta al programa que maneja las solicitudes GET
        char *argv[] = {pedido_get, NULL}; // Argumentos para el programa
        execv(pedido_get, argv); // Ejecutar el programa
        // Si execv() retorna, ha habido un error
        perror("Error al ejecutar el programa pedido_get para GET");
        exit(EXIT_FAILURE);
    } else if (strstr(buffer, "HEAD") != NULL) {
        char *pedido_head = "./pedido_head"; // Ruta al programa que maneja las solicitudes HEAD
        char *argv[] = {pedido_head, NULL}; // Argumentos para el programa
        execv(pedido_head, argv); // Ejecutar el programa
        // Si execv() retorna, ha habido un error
        perror("Error al ejecutar el programa pedido_head para HEAD");
        exit(EXIT_FAILURE);
    } else {
        char *pedido_error = "./pedido_error"; // Ruta al programa que maneja los errores
        char *argv[] = {pedido_error, NULL}; // Argumentos para el programa
        execv(pedido_error, argv); // Ejecutar el programa
        // Si execv() retorna, ha habido un error
        perror("Error al ejecutar el programa pedido_error para errores");
        exit(EXIT_FAILURE);
    }

    // Cerrar el socket del cliente
    close(cliente_socket);
    exit(0);
    }
    
    // Cerrar el socket del servidor
    close(servidor_socket);
}
    
    return 0;
}