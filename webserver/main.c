# include <stdio.h>
# include <string.h>
int main ( int argc , char ** argv )
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


int fd;
char buffer[128];
ssize_t ret;


creer_serveur(8080);

fd = open(,O_RONLY);

if(fd == -1 ){
	fprintf(stderr,"Erreur d'ouverture");
}