#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>  
#include <sys/types.h> 
#include <netinet/in.h>

/*
 * Code Client
 */
main(argc, argv)
int argc;
char **argv;
{
    unsigned short port;       /* port dans lequel le client doit se connecter*/
    char buf[300];             /* donnee a envoyé et recevé */
    struct hostent *hostnm;    /* address ip de serveur */
    struct sockaddr_in server; /* server address (port,add_ip,AF_INET) */
    int s;                     /* client socket */
    char Nfile[20];            // le nom de fichier dans le serveur
    FILE* fichier;     

    /*
     * test des arguments.
     */
    if (argc != 3)
    {
        printf("les arguments sont incorrect\n");
        exit(1);
    }

    /*
     * le premier argument est l'address ip de serveur.
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
       printf("adress incorrect\n");
        exit(2);
    }

    /*
     * le second argument est le numero de port.
     */
    port = (unsigned short) atoi(argv[2]);

    /*
     * mettre les information de serveur dans server structure.
     */
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /*
     * creation de la socket.
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket no cree\n");
        exit(3);
    }

    /*
     * connexion au serveur.
     */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("probleme de connexion\n");
        exit(4);
    }
    while(1)
    {
        printf("entrer le nom du fichier | 0 pour terminer la connexion\n");
        scanf("%s",Nfile);
        if (send(s, Nfile, sizeof(buf), 0) < 0)//envoi de nom de fichier au serveur
        {
            printf("probleme d'envoi\n");
            exit(5);
        }
        
       if(Nfile[0]=='0')// ici le client termine la connexion 
       {
           close(s);
           exit(0);
       }
       recv(s, buf, sizeof(buf), 0); // reponse de serveur,le fichier existe ou pas dans le serveur   
       if(buf[0]=='1')
       /* si le fichier est trouve le serveur envoi la valeur 1 pour commencer l'envoi de fichier sinon on recommence jusqu'a entrer un nom de
       fichier valide*/
       {
           fichier=fopen(Nfile,"w+");   // on va creer un fichier en mode ecriture
           recv(s, buf, sizeof(buf), 0); // on commence a recevoire le premier mot de fichier
       while(buf[0]!='1') // si le fichier arrive a son fin le serveur envoi la valeur 1 dans le buf pour indiquer au client que c'est le EOF
       {
           
           fprintf(fichier,"%s",buf);
           recv(s, buf, sizeof(buf), 0);
       }
       fclose(fichier);
       }
       else
       {
           printf("le fichier n'existe pas\n");
       }

    }
}
