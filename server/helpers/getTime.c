#include <time.h>
#include <stdlib.h>

char* getActualTime() {
    time_t tiempoActual = time(NULL);
    struct tm* fechaActual = gmtime(&tiempoActual);

    static char fechaHoraFormato[100];
    strftime(fechaHoraFormato, sizeof(fechaHoraFormato), "%a, %d %b %Y %H:%M:%S %Z", fechaActual);

    return fechaHoraFormato;
}