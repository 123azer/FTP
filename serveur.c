#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>  
#include <sys/types.h> 
#include <netinet/in.h>


/*
 * code serveur.
 */
main(argc, argv)
int argc;
char **argv;
{
    unsigned short port;       /* port de serveur  */
    char buf[300];             /* donnee reçu et envoyer par le serveur */
    struct sockaddr_in client; /* address de client          */
    struct sockaddr_in server; /* address de serveur      */
    int s;                     /* socket pour accepter la connexion    */
    int ns;                    /* socket connecté au client          */
    int namelen;               /* longeur de nom client    */
    FILE* fichier=NULL;    
    /*
     * test des arguments.
     */

    if (argc != 2)
    {
        printf("les arguments sont incorrect\n");
        exit(1);
    }

    /*
     * le premier argument est le numero de port.
     */
    port = (unsigned short) atoi(argv[1]);

    /*
     * creation de la socket.
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket no cree\n");
        exit(2);
    }

    /*
     * mettre les information de serveur dans server structure.
     */
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("probleme de bind\n");
        exit(3);
    }

    
    if (listen(s, 1) != 0)
    {
        printf("probleme de listen\n");
        exit(4);
    }
    printf("serveur a demarre...\n");
    /*
     * Acceptation de la connexion.
     */
    namelen = sizeof(client);
    if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
    {
        printf("probleme de accept\n");
        exit(5);
    }
    printf("le client est connecte :)\n"); 
    /*
     * recevoire le message de client.
     */
    while(1)
    {
         if (recv(ns, buf, sizeof(buf), 0) == -1)// le serveur recoi le nom de fichier ou la valeur 0 pour terminer la connexion
         {
              printf("probleme de reception\n");
              exit(6);
         }
         if(buf[0]=='0'){
              close(ns);
              close(s);    // terminer la connexion
              exit(0);
         }
         fichier=fopen(buf,"r");  // on ouvre le fichier avec le nom envoyer par le client
         if(fichier==NULL)// si le fichier n'existe pas
         {
             
             buf[0]=2;// buf[0]#1 donc on va pas commencer l'envoi le serveur va reboocler et attendre un nom du fichier valide
             send(ns, buf, sizeof(buf), 0);
         } 
         else
         { 
             buf[0]='1';// donc ce cas le client doit etre pret pour recevoire les donnees du fichier
             send(ns, buf, sizeof(buf), 0);
             while(!feof(fichier))
             {  
                    fgets(buf,300,fichier); // on va envoyer ligne par ligne
                    send(ns, buf, sizeof(buf),0);
                    if(!feof(fichier)) buf[0]='1';// on va indiquer au client que le fichier arrive a son fin
             }
             fclose(fichier);
         }
    }
 }
