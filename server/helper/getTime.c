#include <time.h>

char* getActualTime() {
    struct tm fechaActual;
    time_t tiempoActual = time(NULL);
    localtime_r(&tiempoActual, &fechaActual);

    static char fechaHoraFormato[100];
    strftime(fechaHoraFormato, sizeof(fechaHoraFormato), "%a, %d %b %Y %H:%M:%S %Z", &fechaActual);

    return fechaHoraFormato;
}