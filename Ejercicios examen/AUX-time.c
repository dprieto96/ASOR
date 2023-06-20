#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){

    time_t hora;
    hora=time(NULL);
    struct tm *horaActual=localtime(&hora);

    char chora[500];
    strftime(chora, sizeof(chora), "%H:%M:%S\n",horaActual);
    printf(chora);

    return 0;
}