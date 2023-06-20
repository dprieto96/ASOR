/*
Ejercicio 3 (1,5 puntos). Escribe un programa que lea simultáneamente de dos tuberías con nombre
(tuberia1 y tuberia2). El programa mostrará el nombre de la tubería desde la que se leyó y los
datos leídos. El programa detectará cuándo se cierra el extremo de escritura de una tubería (read()
devuelve 0) para cerrarla y volver a abrirla.

Voy a añadir una pequeña modificación y es que si recive:
    -> Recive "t": Devuelve la hora en HH:MM:SS
    -> Recive "q": Cierra el servidor
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <time.h>s

#define BUF_SIZE 500

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;             /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        close(sfd);
    }

    freeaddrinfo(result); /* No longer needed */

    if (rp == NULL)
    { /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    /* Read datagrams and echo them back to sender */

    for (;;)
    {
        peer_addr_len = sizeof(peer_addr);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
                         (struct sockaddr *)&peer_addr, &peer_addr_len);
        if (nread == -1)
            continue; /* Ignore failed request */

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *)&peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);

        buf[nread] = '\0';

        

        if (strncmp(buf, "t", strlen(buf)-1)==0)
        { // Devuelve la hora

            time_t hora;
            hora = time(NULL);
            struct tm *horaActual = localtime(&hora);

            char chora[500];
            strftime(chora, sizeof(chora), "%H:%M:%S\n", horaActual);
           // printf(chora);

            stpcpy(buf, chora);
            buf[strlen(buf)-1]="\0";

           
            sendto(sfd, buf, strlen(buf)-1, 0,
                       (struct sockaddr *)&peer_addr,
                       peer_addr_len) ;

        }

        else if (strncmp(buf, "q", strlen(buf)-1)==0)
        {
           
            stpcpy(buf, "El servidor ha cerrado, largate\n");
            sendto(sfd, buf, strlen(buf)-1, 0,
                       (struct sockaddr *)&peer_addr,
                       peer_addr_len);
            close(sfd);
        }

        else
        {

            if (s == 0)
                printf("Received %zd bytes from %s:%s\n",
                       nread, host, service);
            else
                fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

                sendto(sfd, buf, strlen(buf)-1, 0,
                       (struct sockaddr *)&peer_addr,
                       peer_addr_len);
            
        }

        
    }

    return 0;
}
