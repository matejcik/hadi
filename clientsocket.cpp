#include "network.hpp"
#include <iostream>

TTCPClientSocket::TTCPClientSocket () {
   connected = false;
}

/* otevre spojeni s protected TCPsocket clientsock */
void TTCPClientSocket::connect (const char* host, Uint16 port) {
	IPaddress ip;

	/* ziskame IP adresu... */
	if(SDLNet_ResolveHost(&ip,host,port) == -1) {
		std::cerr << "SDLNet_ResolveHost: "<< SDLNet_GetError() << std::endl;
		exit(1);
	}
	/* ...skrz kterou se spojime s hostem pres socket "client"  */
	clientsock=SDLNet_TCP_Open(&ip);
	if(!clientsock) {
		std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
		exit(2);
	} else connected = true;
}

/* zavre spojeni */
void TTCPClientSocket::close () {
	SDLNet_TCP_Close(clientsock);
	connected = false;
}

/* posle data skrz clientsock
 * -vraci 0 pri chybe, jinak velikost poslanych dat
 */
int TTCPClientSocket::send (TData data) {
	Uint32 poslano,delka;

	//zjisti velikost posilanych dat
	delka = sizeof(data);

	// posle data
	poslano = SDLNet_TCP_Send(clientsock, &data, delka);
	if(poslano < delka) {
		if(SDLNet_GetError() && strlen(SDLNet_GetError()) )
         std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
		return(0);
	}
	return(poslano);
}

/* prijme data	z clientsock
 * vraci prijata data, v pripade chyby uvnitr zmeni typ na ERROR
 */

TData TTCPClientSocket::recv () {
   Uint32 prijato, delka;
   // je sizeof(TData) == sizeof(data)???
   delka = sizeof(TData);
   TData data;

   //prijme data ze socketu
   prijato = SDLNet_TCP_Recv(clientsock,&data, delka);
   if (prijato < delka )  {
      std::cerr << "SDLNet_TCP_Recv: " << SDLNet_GetError() << std::endl;
      data.typ = ERROR;
   }

   return (data);
}

//TODO UDP