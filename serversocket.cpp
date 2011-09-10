#include "network.hpp"
#include <iostream>

#define MYPORT 3500

void TTCPServerSocket::listen (Uint16 port) {
	IPaddress ip;

	/* ip.host = INADDR_ANY */
	if(SDLNet_ResolveHost(&ip,NULL,port) == -1) {
		std::cerr << "SDLNet_ResolveHost: "<< SDLNet_GetError() << std::endl;
		exit(1);
	}
	/*  poslouchame pro prichozi spojeni z port-u*/
	serversock=SDLNet_TCP_Open(&ip);
	if(!serversock) {
		std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		exit(2);
	} else listening = true;
}

/* zavre spojeni */
void TTCPServerSocket::close () {
	SDLNet_TCP_Close(serversock);
	listening = false;
}

/* posloucha na serversock pro prichozi spojeni, pokud se mu podari vytvorit spojeni,
 * zapise ho do clientsock uvnitr tridy klientskeho socketu*/
bool TTCPServerSocket::accept (TCPClientSocket client_socket) {
   TCPsocket newclient;
   newclient = SDLNet_TCP_Accept(serversock);
   if(!newclient) {
      std::cerr << "SDLNet_TCP_Accept: " << SDLNet_GetError() << std::endl;
   } else {
      client_socket->clientsock = newclient;
      client_socket->connected = true;
   }
   return ( newclient? true:false);
}

//TODO UDP
