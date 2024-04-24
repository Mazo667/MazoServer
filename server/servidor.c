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
	pid_t pid;
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
    //Permitir la reutilizion del puerto de escucha
    int option;
    option = SO_REUSEADDR;
    setsockopt(servidor_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Configurar la estructura de dirección del servidor
    bzero((char*) &servidor_addr,sizeof(servidor_addr));

    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(PUERTO);
    servidor_addr.sin_addr.s_addr = htonl(INADDR_ANY);


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

    bzero(buffer,1000);

    printf("Conexión aceptada\n");
    
    if(fork()==0){
    int pid = getpid();
    printf("Proceso hijo creado %d id\n",pid);

    int n = read(cliente_socket, buffer, sizeof(buffer) - 1);

    if(n < 0) perror("Error en leyendo el socket");

    printf("Status Code: (%d)\n", n);
    printf("Mensaje: %s\n", buffer);

   char *method = strtok(buffer, " "); //GET o HEAD   

    if (strcmp(method, "GET") == 0) {
    char *filename = strtok(NULL, " "); //nombre del archivo solicitado
    filename = strtok(filename, "/"); //nombre del archivo solicitado 

        //pregunto si solicito un archivo
        if(filename == NULL){
            printf("No se solicitó un archivo\n");
            char *response = 
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n\r\n";
            write(cliente_socket, response, strlen(response));
            close(cliente_socket);
            exit(0);
        }else{
            printf("Archivo solicitado: %s\n", filename);
            char baseDir[] = "/home/maximiliano/Descargas/";
            strcat(baseDir, filename);
            printf("Ruta del archivo: %s\n", baseDir);
            //abrir el archivo solicitado
            FILE *file = fopen(baseDir, "r");

            /*PREGUNTO SI SE PUEDE ABRIR EL ARCHIVO*/
            if (file == NULL) {
                printf("No se pudo abrir el archivo\n");
                char *response = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n\r\n";
            
                write(cliente_socket, response, strlen(response));
                close(cliente_socket);
                exit(0);
            }

            else {
                /* PARA HACER CON UN ARCHIVO
                // Mover el puntero al final del archivo
                fseek(file, 0, SEEK_END);
                // Obtener la longitud del archivo
                long file_length = ftell(file);
                // Mover el puntero de nuevo al inicio del archivo
                fseek(file, 0, SEEK_SET);
            
                char response[4096];
                sprintf(response, 
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %ld\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n\r\n", file_length);
                write(cliente_socket, response, strlen(response));

                // Enviar el contenido del archivo al cliente
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    write(cliente_socket, buffer, strlen(buffer));
                }
                fclose(file);
                */
                /*ENVIO EL ARCHIVO AL CLIENTE*/
                char *body = "<html><body><h1>HOLA</h1></body></html>\r\n";
                int body_length = strlen(body);

                char response[4096];
                sprintf(response, 
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: %d\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "%s", body_length, body);
            
                write(cliente_socket, response, strlen(response));
                close(cliente_socket);
                exit(0);
            }
        }
    }
    else if (strcmp(method, "HEAD") == 0) {
    printf("Se recibio el metodo HEAD\n");    
    }
    else {
        printf("Metodo no soportado\n");
        char *response = 
        "HTTP/1.1 501 Not Implemented\r\n"
        "Server: ServidorHTTP/1.0\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n\r\n";
        
        write(cliente_socket, response, strlen(response));
        close(cliente_socket);
        exit(0);
    }

    /*
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
    */
    
    // Cerrar el socket del cliente
    close(cliente_socket);
    exit(0);
    }
}
    // Cerrar el socket del servidor
    close(servidor_socket);
    return 0;
}