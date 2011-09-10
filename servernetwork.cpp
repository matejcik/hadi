#include <had.hpp>
#include <network.hpp>
#include <ctime>
#include <iostream>

#define POCETSYNCH 5 //pocet synch paketu, ze kterych se spocita lag

bool ServerNetwork<TTCPServerSocket,TTCPClientSocket>::synchronizace(Klient client) {
   Uint32 base, lag = 0, lagy[POCETSYNCH];
   bool podarilo = false, prijato;

   for(Uint8 i = 0; i < POCETSYNCH; i++) {
      prijato = false;
      //poznamenam si cas
      base=SDL_GetTicks();

      //poslu paket
      TTimingData timingdata; timingdata.typ=TIMING; timingdata.count=i;
      TData data; data.timing = timingdata;
      client.socket->send (data);

      //prijmu stejny paket od klienta
      TData recdata = client.socket->recv ();
      if ( (recdata.timing.typ == TIMING) && (recdata.timing.count == i) ) prijato = true;
	}

   //spoctu lag a zaradim do pole
   lagy[i]=SDL_GetTicks()-base;

	//spocitam prumerny lag a zapisu klientovi do polozky ping
   for(Uint8 j = 0; j < POCETSYNCH; j++) lag += lagy[j];
   client.ping = lag / POCETSYNCH;
   podarilo= prijato;
   return (podarilo);
}

void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::serve() {
   server.listen();
   Random::seed = time(NULL);
}

bool ServerNetwork<TTCPServerSocket,TTCPClientSocket>::handshake(Klient client) {
   // zjisti ping klienta
   bool podarilo = synchronizace(client);

   //posle klientovi jeho THandshakeData = id, seed a ping
   THandshakeData shakedata; shakedata.typ=SEED; shakedata.id=client.id; 
		shakedata.seed=Random::seed; shakedata.ping=client.ping;
   TData data; data.handshake = shakedata;
   client.socket->send (data);

   return(podarilo);
}



void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::init () {
   /* initializovana SDL
	if(SDL_Init(0)==-1) throw(SDLNet_GetError());
	*/

	// initializovana SDL_net
	if(SDLNet_Init()==-1) throw(SDLNet_GetError());
		
   //pripravime si socket na prijem a inicializujeme seed
	serve();
}

void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::quit () {
   //vyprazdni vector klientu a postupne je pozavirej
   //je to vubec potreba?
   for (Klienti::reverse_iterator i = klienti.rbegin(); i != klienti.rend(); i++) {
      (*i).socket->close();
      klienti.pop_back();
   }

   // shutdown SDL_net
	SDLNet_Quit();

	/* shutdown SDL
	SDL_Quit();
	*/
}

void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::start () {
   started = true;
   max_ping = 0;

   //zjistime maximalni ping
   for (Klienti::iterator i = klienti.begin(); i != klienti.end(); i++)
      if (max_ping < (*i).ping) max_ping = (*i).ping;


   //posleme vsem klientum, za jak dlouho zaciname
   for (Klienti::iterator j = klienti.begin(); j != klienti.end(); j++) {
      TStartData startdata; startdata.typ=START; startdata.zaciname=2*max_ping;
      TData data; data.start = startdata;
      (*j).socket->send (data);
   }

   //pockame stanovenou dobu
   SDL_Delay(2*max_ping);
}

void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::tick () {
   //prijimam nove klienty, dokud nezacneme
   while (!started && accept()) {
      if (klienti.size() == 2) start();
   }

   //hra zacala
   if (started) { 
		//pruchod 
			//od klienta ctu a vytipnu smer a poslu ovl fci command

      //prvni pruchod projde klienty a zjisti, zda neco poslali
      //pokud od nich neco prislo, posle to jejich ovladacum
      //kazdopadne posle v druhem pruchodu vsem pohyby vsech ostatnich
         //nebo jen pohyby ostatnich?
   }
}

template <>
bool ServerNetwork<TTCPServerSocket,TTCPClientSocket>::accept () {
   while (true) {
      TCPClientSocket newsock = new TTCPClientSocket;
      if (! server.accept(newsock)) return (false); //uz nejsou zadni dalsi cekajici klienti

      //mame noveho klienta
      Klient newclient;

      //zavedeme mu ovladac a ostatni atributy
		NetworkedOvl newovl = new TNetworkedOvl(hadovnik,newclient.id); //kdyz je Hadovnik glob.prom.
		newclient.id = (int)klienti.size();
		newclient.ovladac = newovl;
		newclient.socket = newsock;

      //handshake
      if (!handshake (newclient)) return (false); // neco se nepovedlo

      // projdeme klienty a navzajem je seznamime
      for (Klienti::iterator i = klienti.begin(); i != klienti.end(); i++) {
         // kazdeho novemu klientovi
         TClientData clientdata; clientdata.typ=NEW_CLIENT; clientdata.id=(*i).id;
         TData data; data.client = clientdata;
         newclient.socket->send (data);

         //kazdemu noveho klienta
         TClientData newclientdata; newclientdata.typ=NEW_CLIENT; newclientdata.id=newclient.id ;
         TData newdata; newdata.client=newclientdata;
         (*i).socket->send (data);
      }

      //pridame k ostatnim do vektoru
      klienti.push_back(newclient);
		return (true);
	}
}

void ServerNetwork<TTCPServerSocket,TTCPClientSocket>::send (Uint8 id, Smer s) {}
