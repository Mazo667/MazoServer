#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Contenido del recurso solicitado
    char *contenido = "<html><body><h1>Hello, World!</h1></body></html>";
    
    // Encabezados de la respuesta HTTP
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Length: %lu\r\n", strlen(contenido));
    printf("Content-Type: text/html\r\n");
    printf("\r\n");
    
    // Cuerpo del recurso solicitado
    printf("%s", contenido);
    
    return 0;
}
