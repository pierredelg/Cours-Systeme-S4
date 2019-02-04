# include <stdio.h>
# include <string.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

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


  //On crée le socket serveur sur le port 8080
  //methode socket() + bind()
  int socket_client ;
  char buffer[128] = "Salut tout la monde";
  int i;
  int socket_serveur = creer_serveur(8080);

  printf("%d",socket_serveur);

  //On ecoute met le serveur en écoute
  if(listen(socket_serveur,10) == -1){
    perror("listen");
    return -1;
  }
  //On récupere le fd client 
 
  socket_client = accept(socket_serveur , NULL, NULL);
  
  if (socket_client == -1){
      perror("accept");
      return -1;
  }
  //on écrit sur le socket client
  write(socket_client, buffer, 128);

  //On récupere ce que le client envoi 
  read(socket_client,buffer,128);

  //On affiche le résultat de la lecture
  for(i = 0 ; i < 128 ; i++ ){
      printf("%s",&buffer[i]);
  }
  
  //On ferme les sockets
  printf("Fermeture de la socket serveur\n");
  close(socket_serveur);
  printf("Fermeture de la socket client\n");
  close(socket_client);

  /* utiliser la commande nc localhost 8080 pour tester*/
}
