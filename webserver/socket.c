int creer_serveur(int port){

	int socket_serveur ;

	socket_serveur = socket ( AF_INET , SOCK_STREAM , 0);

	if ( socket_serveur == -1)
	{
		/* traitement de l ’ erreur */
		perror ( "socket_serveur" );
		return -1;
	}
	/* Utilisation de la socket serveur */

	struct sockaddr_in saddr ;
	saddr . sin_family = AF_INET ; /* Socket ipv4 */
	saddr . sin_port = htons (port); /* Port d ’écoute */
	saddr . sin_addr . s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */

	if ( bind ( socket_serveur , ( struct sockaddr *)& saddr , sizeof ( saddr )) == -1)
	{
		/* traitement de l ’ erreur */
		perror ( "bind socker_serveur" );
		return -1;
	}

	return socket_serveur;
}

int listen(){

	if ( listen ( socket_serveur , 10) == -1)
		{
			/* traitement d ’ erreur */
			perror ( "listen socket_serveur" );
			return -1;
	}
}