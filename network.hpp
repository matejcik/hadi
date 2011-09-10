#ifndef NETWORK__HPP
#define NETWORK__HPP

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <had.hpp>
#include "ovladace/o_networked.hpp"

/* typ je ERROR v pripade nekorektnosti dat */
typedef enum { TIMING, POHYB_HADA, CLIENT_QUIT, SERVER_KILL, NEW_CLIENT, SEED, START, ERROR } Typ;

struct TTimingData {
	Typ typ; // vzdycky musi byt prvni... aby ta polozka typ v TData byla totez
	Uint8 count;
};

struct TPohybData {
	Typ typ;
	Uint8 id;
	Smer smer;
};

struct TClientData {
	Typ typ;
	Uint8 id;
};

struct THandshakeData {
	Typ typ;
	Uint8 id;
	unsigned long seed;
   Uint32 ping;	//ms
};

struct TStartData {
   Typ typ;
   Uint32 zaciname; //za kolik se zacne = 2*max_ping
};

typedef union {
	Typ typ;
	TTimingData timing;
	TPohybData pohyb;
	TClientData client;
	THandshakeData handshake;
	TStartData  start;
} TData;

class TClientSocket {
	public:
		virtual void connect (const char* host, Uint16 port) = 0;
		virtual void close () = 0;
		virtual int send (TData) = 0;
		virtual TData recv () = 0;
};
typedef TClientSocket* ClientSocket;

class TServerSocket {
	public:
		virtual void listen () = 0;
		//virtual bool accept (ClientSocket) = 0;
		virtual void close () = 0;
};
typedef TServerSocket* ServerSocket;

class TTCPClientSocket : public TClientSocket {
	public:
		TTCPClientSocket ();	//konstruktor jen nastavi connected na false
		virtual void connect (const char* host, Uint16 port);
		virtual void close ();
		virtual int send (TData);
		virtual TData recv ();

		friend class TTCPServerSocket;
	protected:
		TCPsocket clientsock;
		bool connected;
};
typedef TTCPClientSocket* TCPClientSocket;

class TTCPServerSocket : public TServerSocket {
	public:
	   /* konstruktor urci, na kterem portu budeme poslouchat ??*/
		TTCPServerSocket ();

		virtual void listen ();
		virtual bool accept (TCPClientSocket);
		virtual void close ();
	protected:
		TCPsocket serversock;
		bool listening;   //je to potreba vedet?
		Uint16 port_;
};

//TODO: UDP Socket

class Network { // tady nebudou potreba pointery, protoze Network bude jedna
   public:
      virtual void init() = 0;
      virtual void quit() = 0;
      virtual void tick() = 0;
		
		//posle to vsem vc. nebo ne TODO
		virtual void send (Uint8 id, Smer s) = 0;
	protected:
		Hadovnik hadovnik;
};

// TODO ClienhtNetwork!!!
template <class Sock> /* casem mozna TSocket bude templatovana na server/klient */
class ClientNetwork : public Network {
public:
		virtual void init (); // bude mit klient vlastni?
		virtual void quit ();
		virtual void tick ();
		virtual void send (Uint8 id, Smer s);
		virtual void connect (const char* servername);
		// tady mozna pak zamenit connect za init, at je to v jednom.
		//...ale jen mozna ;e)
	protected:
		Sock socket;
		virtual void handshake ();
};

//
template <class ServSock, class CliSock>
class ServerNetwork : public Network {
	public:
		virtual void init (); // serve () necht je v initu
		virtual void quit ();
		virtual void tick ();
		virtual void send (Uint8 id, Smer s);
		virtual void start ();
	protected:
		Uint32 max_ping;

		ServSock server; // tohle bude ServerSocket
		struct Klient {
			int id;
			NetworkedOvl ovladac;
			Uint32 ping;
			CliSock* socket; //...zatimco tohle budou sockety obyc.
		};
		typedef std::vector<Klient> Klienti;
		Klienti klienti;
		
    bool synchronizace(Klient);
    void serve();
    bool handshake(Klient);
	 bool started;
    bool accept ();
};

extern ServerNetwork<TTCPServerSocket,TTCPClientSocket> network;

#endif
