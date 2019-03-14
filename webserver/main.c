#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


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

int main(){

    initialiser_signaux();

    char bufferFirstLine[128] = "Bienvenue sur le serveur Pawnee\n";
    char bufferContenu[128] = {" "};
    int socket_client = 0;
    int pid = 0;
    char *nomServeur ="<Pawnee>";
    FILE *fdClient = NULL;
    char chemin[128] = {" "} ;
    int indiceBuffer = 0;
    int indiceChemin = 0;
    FILE* fdFichierTrouve = NULL;
    

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

            if(fdClient == NULL){
                perror("impossible d'ouvrir le socket");
                return -1;
            }

           
            while(1){

                //On récupere la premiere ligne envoyée par le client
                fgets(bufferFirstLine, 128, fdClient);

                //On lit la ligne tant que le contenu est different de "\r\n"
                while(strcmp(bufferContenu,"\r\n") != 0){

                    fgets(bufferContenu, 128, fdClient);
                    if(strcmp(bufferContenu," \r\n") == 0){
                        break;
                    }
                }
             
                indiceBuffer = 4;   //Ici l'indice commence à 4 pour récuperer le chemin aprés le GET
                indiceChemin = 0;

                //On copie le chemin de la requete GET tant que l'on ne rencontre pas d'espace
                while(bufferFirstLine[indiceBuffer] != 32){
                    chemin[indiceChemin] = bufferFirstLine[indiceBuffer];
                    indiceBuffer++;
                    indiceChemin++;
                }
                
                //On essaye d'ouvrir le fichier du chemin
                //Si le fopen retourne NULL c'est que le fichier n'existe pas
                if(fopen(chemin,"r") == NULL){
                    printf("Pas de fichier : %s\n",chemin);
                }
                
                //On crée une requete GET valide avec le chemin récuperé
                char testMethode[128] = "GET ";
                strcat(testMethode,chemin);
                strcat(testMethode," HTTP/1.1\r\n");

                //On compare la premiere ligne et on verifie que la requete recue est valide 
                if(strcmp(bufferFirstLine,testMethode) == 0){
                    //si le fichier existe ou si on demande simplement la racine on envoie une réponse 200
                    if(fdFichierTrouve != NULL || strcmp(bufferFirstLine,"GET / HTTP/1.1\r\n") == 0){
                        fprintf(fdClient,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 23\r\n\r\n%s Tout est OK!\r\n",nomServeur);
                    }
                    //sinon On envoie une réponse 404
                    else{
                        fprintf(fdClient,"HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 24\r\n\r\n%s 404 Not Found\r\n",nomServeur);
                    }
                
                }else{
                    //On envoie un message d'erreur si la requete ne correspond pas à une requete GET valide
                    fprintf(fdClient,"HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: 26\r\n\r\n%s 400 Bad Request\r\n",nomServeur);
                    
                }

                 //On reinitialise le buffer
                memset(bufferFirstLine,0,strlen(bufferFirstLine));
                memset(bufferContenu,0,strlen(bufferContenu));

                //On vérifie que le descripteur ne soit pas NULL avant de les fermer
                if(fdFichierTrouve != NULL){
                    fclose(fdFichierTrouve);   
                }
                if(fdClient != NULL){
                    fclose(fdClient);
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