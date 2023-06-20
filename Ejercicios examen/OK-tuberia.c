/*
Ejercicio 3 (1,5 puntos). Escribe un programa que cree dos procesos y ejecute un comando en cada
uno, de forma que la salida del primer comando sea la entrada del segundo. El proceso padre
imprimirá el PID y el código de salida de cada proceso. El programa se ejecutará de la siguiente
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    
    char *comando[3];


    pid_t cpid;//pid proceso hijo
    
    int pipefd[2];//Creo los fileDescriptor de la tubería
    /*
                             ___________________
        fd[1] --> ESCRITURA |    TUBERÍA  --->  |  --> fd[0] LECTURA(hijo)
                             -------------------
    
    */
    

    if (argc != 5)
    {
        fprintf(stderr, "Invalid number of arguments.\nUsage: ./pipe <command1> <argument1> <command2> <argument2>\n");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1)//CREO TUBERIA 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();//Creo proceso hijo
   

     if (cpid == -1)//Control de errores
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    switch(cpid){
        case 0://Proceso hijo
        /*El dup 2 es para redirigir una salida donde:
        int dup2(int oldfd, int newfd);
                                | -> 0 STDIN
                                | -> 1 STDOUT
                                | -> 2 STDERR
        */
        dup2(pipefd[0],0);

        close(pipefd[0]);
        close(pipefd[1]);

        /*
        A execvp(),le debo pasar un array de char por *, a partir de el ejecutará
        todos los comandos que haya en el array, hasta que encuentre un NULL.
        Por eso creo char*comando[3];
        comando[0]= argumento 1
        comando[1]= argumento 2
        comando[2]= NULL que finaliza la ejecución de comandos

        execvp(comando[0],comando);
        */
       
        
       
       fprintf(stderr,"Ejecutando HIJO, pid padre PPID: %i\n",getppid());
       fprintf(stderr,"Ejecutando HIJO CPID: %i\n",getpid());

        comando[0]=argv[3];
        comando[1]=argv[4];
        comando[2]=NULL;

         fprintf(stderr,"Salida de: %s %s que es entrada de %s %s\n",argv[1],
         argv[2],argv[3],argv[4]);
        execvp(comando[0],comando);

        break;

        default://Proceso padre
        dup2(pipefd[1],1);

        close(pipefd[0]);
        close(pipefd[1]);

        fprintf(stderr,"Ejecutando PADRE PID: %i\n...\n..\n.\n",getpid());

        comando[0]=argv[1];
        comando[1]=argv[2];
        comando[2]=NULL;


        execvp(comando[0],comando);

        break;


    }
   
    return 0;
}
