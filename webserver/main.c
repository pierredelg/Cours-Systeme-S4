#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void traitement_signal(int sig){

    pid_t idFin;
    int statut;

    printf("Signal %d reçu\n",sig); //SIGCHLD = 17

    //On lance le waitpid afin de terminer les processus zombies fils 
    idFin = waitpid(-1, &statut, WNOHANG);

    //gestion erreur waitpid
    if (idFin == -1) {           
        perror("erreur du waitpid");
    }
    //On verifie le statut de fin du fils
    if (WIFEXITED(statut)){
        printf("Processus fils terminé\n");
    }
}

void initialiser_signaux(){

    /* On utilise la fonction signal() afin de modifier le comportement du programme lors de la réception d'un signal
       les parametres sont donc le signal de réception(SIGPIPE)
       le deuxieme parametre est le comportement à adopter(SIG_IGN) ici ignorer ce signal
    */
    if (signal(SIGPIPE,SIG_IGN) == SIG_ERR ){
        perror ("signal");
    }

    //on déclare une structure décrivant le comportement a avoir lors de la réception du signal SIGCHLD
    struct sigaction sa;
    
    //appel de la méthode traitement_signal() lors de la réception du signal SIGCHLD
    sa.sa_handler = &traitement_signal;

    //aucun signal a bloquer lors du traitement de ce signal (utilisation de la méthode sigemptyset() == NULL)
    sigemptyset(&sa.sa_mask);

    //On utilise SA_RESTART pour redémarrer automatiquement les fonctions interrompues
    sa.sa_flags = SA_RESTART ;

    /*On utilise sigaction qui d’attribuer un comportement spécifique à la réception d’un signal
    1er parametre :  SIGCHLD (le signal) 
    2eme parametre : sa qui est la structure definie avant et décrivant le comportement du programme
    */
    if(sigaction(SIGCHLD,&sa,NULL) == -1){

        perror("sigaction(SIGCHLD)");
    }
}

int main (){


    /* Arnold Robbins in the LJ of February ’95 , describing RCS */

    /* Main de départ

    if ( argc > 1 && strcmp ( argv [1] , "-advice" ) == 0) {
    printf ( " Don ’t Panic !\n " );
    return 42;
    }
    printf ( " Need an advice ?\n " );
    return 0;
    }
    
    */

    initialiser_signaux();

    char buffer[128] = "Salut tout la monde\n";
    int lecture,socket_client,pid,i;

    //On crée le socket serveur sur le port 8080
    //methode socket() + bind()
    int socket_serveur = creer_serveur(8080);

    //On met le serveur en écoute
    if(listen(socket_serveur,10) == -1){
        perror("listen");
        return -1;
    }

    while(1){

        //On accept un nouveau client et on garde le fd du client connecté
        socket_client = accept(socket_serveur , NULL, NULL);

        if (socket_client == -1){
            perror("accept");
            return -1;
        }

        //On crée un nouveau processus
        pid = fork();

        
        if (pid == 0){      /*Processus fils*/

        //on écrit sur le socket client
        write(socket_client, buffer, strlen(buffer));

            //On reinitialise le buffer
            memset(buffer,0,strlen(buffer));

            while(1){
                //On récupere ce que le client envoi 
                lecture = read(socket_client,buffer,128);

                if (lecture > 0){

                    //On affiche le résultat de la lecture
                    for(i = 0 ; i < lecture ; i++ ){
                        printf("%c",buffer[i]);
                    }

                    //On écrit dans le socket client afin de renvoyer le message lu par le serveur
                    write(socket_client, buffer, strlen(buffer));

                    //on réinitialise le buffer 
                    memset(buffer,0,strlen(buffer));
                }
                else{
                    printf("fin\n");
                    exit(0);
                    break;
                }
            }
        
        }else{      /*Processus père*/

            //On ferme le socket client
            close(socket_client);
        }
    } 
    //On ferme les sockets
    printf("Fermeture de la socket serveur\n");
    close(socket_serveur);

/*  
    -> utiliser la commande 'nc localhost 8080' pour tester et ouvrir un client
    
    ->la commande 'ps -u nomUtilisateur' permet de voir la liste des processus
*/
}
