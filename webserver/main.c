# include <stdio.h>
# include <string.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>

int main ()
{


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


/*int fd;
char buffer[128];
ssize_t ret;
*/

  int socket_serveur = creer_serveur(8080);
  printf("%d",socket_serveur);


  listen(socket_serveur,10); 


  
  
  
  int socket_client ;
  socket_client = accept(socket_serveur , NULL, NULL);
  if (socket_client == -1)
    {
      perror ( " accept " );
      /* traitement d ’ erreur */
    }

  printf("accept");
}
