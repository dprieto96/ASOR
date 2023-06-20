/*
Ejercicio 3 (1,5 puntos). Escribe un programa que lea simultáneamente de dos tuberías con nombre
(tuberia1 y tuberia2). El programa mostrará el nombre de la tubería desde la que se leyó y los
datos leídos. El programa detectará cuándo se cierra el extremo de escritura de una tubería (read()
devuelve 0) para cerrarla y volver a abrirla.

COPIO:
    $man 2 select
    Añado algunas modificaciones
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define MAX_BUF 20




int main(int argc, char const *argv[])
{
    int sel;
    int fd1, fd2;
    fd_set readset;

    
    //mkfifo crea las tuberias 
    
    mkfifo("tuberia1", 0777);
    mkfifo("tuberia2", 0777);
    char buf[MAX_BUF];
    //abro las tuberias y me quedo con sus fileDescriptor
    fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);
    fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);

    //----------------MEMORIZAR
    int max = fd2;
    if(fd1 > fd2)
        max = fd1;
    //----------------MEMORIZAR

    while(1)
    {
        FD_ZERO(&readset);
        FD_SET(fd1, &readset);
        FD_SET(fd2, &readset);

        sel = select(max+1, &readset, NULL, NULL, NULL);


        int readfrom = 0;
        printf("Datos nuevos.\n");
         //----------------MEMORIZAR (lo mismo fd1 y fd2)
        if (FD_ISSET(fd1, &readset))
        {
            readfrom = 1;
            printf("He leido de la tubería 1:\n");
            int nbytes = read(fd1, buf, 500);
            buf[nbytes] = '\0';
            printf("Recivido: %s\n",buf );
        }
        else if (FD_ISSET(fd2, &readset))
        {
            readfrom = 2;
            printf("He leido de la tubería 2:\n");
            int nbytes = read(fd2, buf, 550);
            buf[nbytes] = '\0';
            printf("Recivido:  %s\n",buf );
        }
         //----------------MEMORIZAR

        if(readfrom == 1)
        {
            close(fd1);
            fd1 = open("tuberia1", O_RDONLY | O_NONBLOCK);

        }
        else if (readfrom == 2)
        {
            close(fd2);
            fd2 = open("tuberia2", O_RDONLY | O_NONBLOCK);
        }


    }

    return 0;
}