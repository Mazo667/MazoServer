#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *filename = strtok(NULL, " "); // nombre del archivo solicitado
    filename = strtok(filename, "/");   // nombre del archivo solicitado

    // pregunto si solicito un archivo
    if (filename == NULL)
    {
        printf("No se solicitó un archivo\n");
        char response[4096];
        sprintf(response,
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Length: 0\r\n"
                "Date: %s\r\n"
                "Connection: close\r\n\r\n",
                getActualTime());
        write(cliente_socket, response, strlen(response));
        close(cliente_socket);
        exit(0);
    }
    else
    {
        printf("Archivo solicitado: %s\n", filename);
        char baseDir[] = "/home/maximiliano/Descargas/";
        strcat(baseDir, filename);
        printf("Ruta del archivo: %s\n", baseDir);
        // abrir el archivo solicitado
        FILE *file = fopen(baseDir, "r");

        /*PREGUNTO SI SE PUEDE ABRIR EL ARCHIVO*/
        if (file == NULL)
        {
            printf("No se pudo abrir el archivo\n");
            char response[4096];
            sprintf(response,
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Length: 0\r\n"
                    "Date: %s\r\n"
                    "Connection: close\r\n\r\n",
                    getActualTime());

            write(cliente_socket, response, strlen(response));
            close(cliente_socket);
            exit(0);
        }

        else
        {
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
                    "Content-Type: text/html\r\n\r\n",
                    file_length);
            write(cliente_socket, response, strlen(response));
            close(cliente_socket);
            exit(0);
        }
    }

    return 0;
}
