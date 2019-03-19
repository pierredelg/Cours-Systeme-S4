#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "socket.h"
#include "http_parse.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define in_range(a,b,c) ((a) < (b) ? 0 : ((a) > (c) ? 0 : 1))
#define BUFFER_SIZE 2048


void traitement_signal(){

    pid_t pidFin;

    while(pidFin != -1){

        pidFin = waitpid(-1, NULL, WNOHANG);    

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

char * fgets_or_exit ( char * buffer , int size , FILE * fdClient ){

    if(fgets(buffer, size, fdClient) == NULL){
        perror("Erreur de lecture sur le client");
        exit(1);
    }
    return buffer;
}

void send_status ( FILE * client , int code , const char * reason_phrase ){

    if(fprintf(client,"HTTP/1.1 %d %s\r\n",code,reason_phrase) < 0){
        perror("Erreur envoi status");
    }
}

void send_response ( FILE * client , int code , const char * reason_phrase ,const char * message_body ){

    send_status(client,code,reason_phrase);

    if(fprintf(client,"%s",message_body) < 0){
        perror("Erreur envoi réponse");
    }

    if(fflush(client) == EOF){
        perror("Erreur flush fdclient");
    }
}

int parse_http_request(const char *request_line , http_request *request)
{

    if (strncmp(request_line, "GET ", 4) != 0)
    {
        request->method = HTTP_UNSUPPORTED;
        return 0;
    }
    request->method = HTTP_GET;
    /* Find the target start */
    const char *target = strchr(request_line, ' ');
    if (target == NULL){
        return 0;
    }
    target++;
      /* Find target end and copy target to request */
    char *target_end = strchr(target, ' ');
    if (target_end == NULL){
        return 0;
    }
    int size = min(target_end - target, MAX_TARGET_SIZE);
    strncpy(request->target, target, size);
      /* If target is more than size, \0 is not add to dst, so... */
    request->target[size] = '\0';

      /* Now http version (only support HTTP/M.m version format) */
      /* Quote from RFC:
         Additionally, version numbers have been restricted to
         single digits, due to the fact that implementations are known to
         handle multi-digit version numbers incorrectly.
      */
    char *version = target_end + 1;
    if (strncmp(version, "HTTP/", 5) != 0){
        return 0;
    }
    if (!in_range(version[5], '0', '9')){ // major
        return 0;
    }
    if (version[6] != '.'){ // mandatory dot
        return 0;
    }
    if (!in_range(version[7], '0', '9')){ // minor
        return 0;
    }
    request->http_major = version[5] - '0';
    request->http_minor = version[7] - '0';
    return 1;
}

void skip_headers(FILE * client){

    char buffer[BUFFER_SIZE];

    //On continue à lire sur le client tant qu'on arrive pas à la derniere ligne
    while(strcmp(buffer,"\r\n") != 0){

        fgets_or_exit (buffer,BUFFER_SIZE, client);
    }

}

char * rewrite_target ( char * target ){
    char * delimiteur = "?";
    char * resultat = strtok(target,delimiteur);
    if(strcmp(resultat,"/") == 0){
        return "index.html";
    }
    return resultat;
}

FILE * check_and_open ( const char * target , const char * document_root ){


    char chemin[BUFFER_SIZE];
    strcpy(chemin,document_root);
    strcat(chemin,target);
    return fopen(chemin,"r");
}

int get_file_size(int fd){

    struct stat buffer;
    
    int resultat = fstat(fd,&buffer);

    if (resultat == -1) {
        perror("stat");
        exit(1);
    }
    return buffer.st_size;

}

int copy(FILE *in, FILE *out){

    char texte[BUFFER_SIZE];
   return fprintf(out,"%s\n",fgets_or_exit(texte,BUFFER_SIZE,in));
}
int main(int argc, char** argv){

   
    /*Déclaration des variables*/
    char bufferFirstLine[BUFFER_SIZE] = "";
    char bufferContenu[BUFFER_SIZE] = {" "};
    int socket_client = 0;
    int pid = 0;
    FILE *fdClient = NULL;
    FILE *fdFichierTrouve = NULL;
    http_request request;
    char *cheminServeur = NULL;
    FILE *fichier = NULL;
    int tailleFichier;

    //On vérifie s'il y a un argument
    if(argc != 1){
        cheminServeur = argv[1];
    }

    initialiser_signaux();

    /*On vérifie que le chemin de racine 
        du serveur donné en parametre est correct*/
    if(argc != 1 && fopen(cheminServeur,"r") == NULL){
        perror("Erreur sur le chemin de la racine du serveur");
        exit(1);
    }

    /*On crée le socket serveur sur le port 8080
        methode socket() + bind()*/
    int socket_serveur = creer_serveur(8080);

    if(socket_serveur == -1){
        perror("Erreur d'ouverture du serveur");
        exit(1);
    }

    //On met le serveur en écoute
    if(listen(socket_serveur,10) == -1){
        perror("Erreur sur le listen du serveur");
        exit(1);
    }

    while(1){

        //On accept un nouveau client et on garde le fd du client connecté
        socket_client = accept(socket_serveur , NULL, NULL);

        if (socket_client == -1){
          perror("Erreur dans la connexion client");
          exit(1);
        }

        //On crée un nouveau processus
        pid = fork();

        if (pid == 0){      /*Processus fils*/

        //On crée le file descriptor client sur le socket en lecture et en écriture ("w+")
        fdClient = fdopen(socket_client, "w+");

        if(fdClient == NULL){
            perror("Impossible d'ouvrir le socket client");
            exit(1);
        }

        while(1){

            //On receptionne la premiere ligne  
            fgets_or_exit(bufferFirstLine,BUFFER_SIZE,fdClient);

            //On passe toutes les lignes d'entetes
            skip_headers(fdClient);

            //On compare la premiere ligne et on verifie que la requete recue est valide 
            if(parse_http_request(bufferFirstLine,&request) == 1){

                if(cheminServeur != NULL){
                    //on essaye d'ouvrir la target contenue dans la requete
                    fichier = check_and_open(request.target,cheminServeur);
                    
                    if (fichier == NULL){
                        perror("Erreur d'ouverture du fichier");
                        send_response(fdClient , 404 , "Not Found", "Connection: close\r\nContent-Length: 13\r\n\r\n404 Not Found");
                        exit(1);
                    }
                    else{

                        fprintf(fdClient,"HTTP/1.1 200 OK\r\n");

                        tailleFichier = get_file_size(fileno(fichier));
                        tailleFichier +=8;
                        printf("taille = %d\n", tailleFichier);

                        if(fprintf(fdClient,"Content-Length: %d\r\n",tailleFichier) < 0){
                             perror("Erreur envoi réponse Content-Length");
                        }
                        if(fprintf(fdClient,"Content-type: text/html; charset=utf-8\r\n\r\n") < 0){
                             perror("Erreur envoi réponse Content-type");
                        }
                        while(copy(fichier,fdClient) > 0);

                        fprintf(fdClient,"\r\n");

                    }
                }
                
                send_response(fdClient , 404 , "Not Found", "Connection: close\r\nContent-Length: 13\r\n\r\n404 Not Found");    
            }
            else{
            	//On envoie un message d'erreur si la requete ne correspond pas à une requete GET valide
                send_response(fdClient , 400 , "Bad Request", "Connection: close\r\nContent-Length: 15\r\n\r\n400 Bad Request");                  
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

            //On tue le processus fils
            exit(0);          

        }
        }else{      /*Processus père*/

            //On ferme le socket client
            close(socket_client);

        }
    } 
      //On ferme les sockets
    printf("Fermeture de la socket serveur\n");
    close(socket_serveur);
    exit(0);
}

