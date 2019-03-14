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

    pid_t pidFin;
    int statut;

    printf("Signal %d reçu\n",sig); //SIGCHLD = 17

    //On lance le waitpid afin de terminer les processus zombies fils

    pidFin = waitpid(-1, &statut, WNOHANG);

    //gestion erreur waitpid
    if (pidFin == -1) {           
        perror("erreur du waitpid");
    }
    //On verifie le statut de fin du fils
    if (WIFEXITED(statut)){
        printf("Processus fils n°%d terminé\n",pidFin);
    }
}

void initialiser_signaux(){

  
  if (signal(SIGPIPE,SIG_IGN) == SIG_ERR ){
    perror ("signal");
  }

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

    initialiser_signaux();

    char bufferFirstLine[128] = "Bienvenue sur le serveur Pawnee\n";
    char bufferContenu[128];
    int socket_client;
    int pid;
    FILE *fdClient;
    char * nomServeur = "<Pawnee>";
    char cheminFichier[1024];

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
        
            //On crée le file descriptor client sur le socket en lecture et en écriture ("w+")
            fdClient = fdopen(socket_client, "w+");
        
            while(1){

                //On récupere la premiere ligne envoyée par le client
                fgets(bufferFirstLine, 128, fdClient);

                //On lit la ligne tant que le contenu est different de "\r\n"
                while(strcmp(bufferContenu,"\r\n") != 0){

                    fgets(bufferContenu, 128, fdClient);
            
                }
                    cheminFichier = 
                    //On compare la premiere ligne et on verifie que "GET / HTTP/1.1\r\n"    
                    if(strcmp(bufferFirstLine,"GET / HTTP/1.1\r\n") == 0){


                        fprintf(fdClient,"%s\nHTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 14\r\n\r\nTout est OK!\r\n",nomServeur);

                       
                    }else{
                        //On envoie un message d'erreur si la requete ne correspond pas au "GET / HTTP/1.1\r\n"
                        fprintf(fdClient,"%s\nHTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 17\r\n\r\n400 Bad request\r\n",nomServeur);
                        
                    }

                     //On reinitialise le buffer
                    memset(bufferFirstLine,0,strlen(bufferFirstLine));
                    memset(bufferContenu,0,strlen(bufferContenu));

                    fclose(fdClient);
                             
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
