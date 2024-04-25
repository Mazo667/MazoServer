#include <time.h>
#include <stdlib.h>

char* getActualTime() {
    time_t actualTime = time(NULL);
    struct tm* actualDate = gmtime(&actualTime);

    static char dateFormat[100];
    strftime(dateFormat, sizeof(dateFormat), "%a, %d %b %Y %H:%M:%S %Z", actualDate);

    return dateFormat;
}