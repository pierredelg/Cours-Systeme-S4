# include <stdio.h>
# include <string.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>


void initialiser_signaux(){
  
  if (signal(SIGPIPE,SIG_IGN) == SIG_ERR ){
    perror ("signal");
  }

}

int main (){


  /* Arnold Robbins in the LJ of February ’95 , describing RCS */

  /*
  if ( argc > 1 && strcmp ( argv [1] , "-advice" ) == 0) {
  printf ( " Don ’t Panic !\n " );
  return 42;
  }
  printf ( " Need an advice ?\n " );
  return 0;
  }
  */

                    /*coté serveur*/

  initialiser_signaux();
  
  //On crée le socket serveur sur le port 8080
  //methode socket() + bind()
  int socket_client;
  
  char buffer[128] = "Salut tout la monde\n";

  int i;
  int socket_serveur = creer_serveur(8080);
  int lecture;

  //On ecoute met le serveur en écoute
  if(listen(socket_serveur,10) == -1){
    perror("listen");
    return -1;
  }
  //On récupere le fd client 
 
  //while(1){

    //On accept un nouveau client et on garde le fd du client connecté
    socket_client = accept(socket_serveur , NULL, NULL);
    
    if (socket_client == -1){
        perror("accept");
        return -1;
    }
    //on écrit sur le socket client
    write(socket_client, buffer, strlen(buffer));
    
    //On reinitialise le buffer
    memset(buffer,0,strlen(buffer));

    while(1){
    //On récupere ce que le client envoi 
      lecture = read(socket_client,buffer,128);

      if ((lecture-1) != 0){

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
        break;
      }
    }
  //} 
  //On ferme les sockets
  printf("Fermeture de la socket serveur\n");
  close(socket_serveur);
  printf("Fermeture de la socket client\n");
  close(socket_client);

  /* utiliser la commande nc localhost 8080 pour tester*/
}
