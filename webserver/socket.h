# ifndef __SOCKET_H__
# define __SOCKET_H__
/* * Crée une socket serveur qui écoute sur toute les interfaces IPv4
de la machine sur le port passé en paramètre . La socket retournée
doit pouvoir ê tre utilis é e directement par un appel à accept.
La fonction retourne -1 en cas d ’erreur ou le descripteur de la
socket créée . */
int creer_serveur ( int port );

enum http_method {
	HTTP_GET ,
	HTTP_UNSUPPORTED ,
};
typedef struct
{
	enum http_method method ;
	int major_version ;
	int minor_version ;
	char target [ 1024 ];
}http_request;

# endif

