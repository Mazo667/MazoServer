#include <stdio.h>
#include <stdlib.h>

int main() {
    // Encabezado HTTP de error
    printf("HTTP/1.1 501 Not Implemented\r\n");
    printf("Content-Type: text/html\r\n");
    printf("\r\n");
    
    // Cuerpo del mensaje de error
    printf("<html><body><h1>Error 501: Not Implemented</h1></body></html>");
    
    return 0;
}
