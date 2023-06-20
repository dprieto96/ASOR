/*
Ejercicio 2 (1 punto).Escribe un programa servidor UDP que escuche en una dirección (IPv4 o IPv6
en cualquier formato) y puerto dados como argumentos. Recibirá del cliente una ruta de fichero y le
devolverá una cadena con el tipo del fichero (regular, directorio, enlace, fifo u otro). Además, al
recibir cada mensaje imprimirá en el terminal la dirección y el puerto del cliente.
*/

/*
COPIO LO QUE HAY EN $man getaddrinfo SERVIDOR que es lo que me piden, solo toco argumentos
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <sys/sysmacros.h>

#include <string.h>



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
        fprintf(stderr, "EROR:La IP es %s y el puerto %s\n", argv[1], argv[2]);
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
        //exit(EXIT_FAILURE);
    }
    /* Read datagrams and echo them back to sender */

    for (;;)
    {
        peer_addr_len = sizeof(peer_addr);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0,
                         (struct sockaddr *)&peer_addr, &peer_addr_len);
        

        //printf("BUFF:%s y longitud %li\n",buf,strlen(buf));
        //printf("NREAD:%i \npeer_addr:%i\naddrlen:%i\n\n\n",nread,peer_addr,peer_addr_len);
        buf[nread] = '\0';
        struct stat sb;
        char puntero[BUF_SIZE];
       // int longitud=0;
        strncpy(puntero,buf,strlen(buf)-1);
        //puntero[nread]='\0';
        

        printf("El puntero es: %s y su LONGITUDIONALIDAD: %li\n",puntero,strlen(puntero));

           if (lstat(puntero, &sb) == -1) {
               perror("lstat");
              // exit(EXIT_FAILURE);
           }

            else{
           printf("Tipo de fichero:");

           switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  printf(" block device\n");            break;
           case S_IFCHR:  printf(" character device\n");        break;
           case S_IFDIR:  printf(" directory\n");               break;
           case S_IFIFO:  printf(" FIFO/pipe\n");               break;
           case S_IFLNK:  printf(" symlink\n");                 break;
           case S_IFREG:  printf(" regular file\n");            break;
           case S_IFSOCK: printf(" socket\n");                  break;
           default:       printf(" unknown?\n");                break;
           }

          // exit(EXIT_SUCCESS);
            }


        if (nread == -1)
            continue; /* Ignore failed request */

        char host[NI_MAXHOST], service[NI_MAXSERV];

        s = getnameinfo((struct sockaddr *)&peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0)
            printf("Received %zd bytes from %s:%s\n",
                   nread, host, service);
        else{
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));}



            





        if (sendto(sfd, buf, nread, 0,
                           (struct sockaddr *) &peer_addr,
                           peer_addr_len) != nread)
                   fprintf(stderr, "Error sending response\n");

        }
    return 0;
}
