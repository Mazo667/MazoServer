#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Encabezados de la respuesta HTTP
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Length: 0\r\n"); // Indicamos que no hay contenido en el cuerpo de la respuesta
    printf("\r\n"); // Línea en blanco para indicar el final de los encabezados
    
    return 0;
}
