
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>



int creer_serveur(int port){

  int socket_serveur ;

  //On crée le socket serveur 
  //AF_INET (ipv4)
  //SOCK_STREAM (TCP)
  socket_serveur = socket (AF_INET,SOCK_STREAM, 0);

  if ( socket_serveur == -1)
    {
      /* traitement de l ’ erreur */
      perror ( "socket_serveur" );
      return -1;
    }
  /* Utilisation de la socket serveur */

  struct sockaddr_in saddr;

  //On redéfini les éléments de la structure sockaddr_in
  saddr.sin_family = AF_INET; /* Socket ipv4 */
  saddr.sin_port = htons (port);/* Port d ’écoute */
  saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
  int optval = 1;
  
  //On modifie l'option du socket serveur pour autoriser une connexion au serveur sans timeout
  //SOL_SOCKET (option niveau socket)
  //SO_REUSEADDR(option permettant de refaire un appel à bind())
  //optval ( int qui doit donner un nombre différent de 0)
  
  if ( setsockopt (socket_serveur,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int)) == -1){
    perror ("Can not set SO_REUSEADDR option");
  }
  
  /*
    bind() permet d'attacher une socket avec une adresse et un port 
    une socket ne peut etre attachée qu'avec une seule adresse et un port
  */
  if ( bind(socket_serveur , (struct sockaddr*)&saddr , sizeof(saddr)) == -1){
      /* traitement de l ’ erreur */
      perror ( "bind socker_serveur" );
      return -1;
  }
	
  return socket_serveur;
}

