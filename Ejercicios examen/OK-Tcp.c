/*
Ejercicio 2 (1 punto).Escribe un programa servidor TCP que escuche en una dirección (IPv4 o IPv6
en cualquier formato) y puerto dados como argumentos. El servidor devolverá lo que el cliente le
envíe. En cada conexión, el servidor mostrará la dirección y el puerto del cliente.

-COPIO LO QUE HAY EN GETADDRINFO:
    Cambio solo argumentos.
    Añado el listen(sfd,5);
    Lo que pone como memorizar es lo que habría que añadir
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

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
        fprintf(stderr, "ERROR:Usa el formato %s <dir. IPv4|IPv6> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;              /* For wildcard IP address */
    hints.ai_protocol = 0;           /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     0);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; 

        close(sfd);
    }

    freeaddrinfo(result); 

    if (rp == NULL)
    { 
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }



     //---------------------------------------------------MEMORIZAR
    listen(sfd, 5);
     //---------------------------------------------------MEMORIZAR



    while (1)
    {
        char host[NI_MAXHOST], service[NI_MAXSERV];
        peer_addr_len = sizeof(peer_addr);
        

         //---------------------------------------------------MEMORIZAR
        int clisd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_len);
         //---------------------------------------------------MEMORIZAR

        s = getnameinfo((struct sockaddr *)&peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);


       //---------------------------------------------------MEMORIZAR
        printf("Conexión desde %s:%s\n", host, service);
        while ( nread = recv(clisd, buf, 80, 0))
        { // Comprobar mensaje!
        buf[nread] ='\0';
        printf("\tMensaje: %s\n", buf);

            send(clisd, buf, nread, 0);
        }
        close(clisd);
         //---------------------------------------------------MEMORIZAR
    }


}
