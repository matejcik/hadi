CXXFLAGS = -O0 -g
#SDLNET_LIBS = -lSDL_net
SDLNET_LIBS =
SDLNET_INCLUDES = -I/usr/include/SDL -I.
LIBFLAGS = `sdl-config --libs` -L/usr/lib -lGL -lGLU $(SDLNET_LIBS)
INCFLAGS = `sdl-config --cflags` $(SDLNET_INCLUDES)

BASE = had.o hadovnik.o vyhodnocovac.o mapa.o bonusy.o inventar.o random.o bonusovnik.o main.o interface.o
HADI = hadi/h_stdout.o
OVLADACE = ovladace/o_random.o ovladace/o_brandom.o ovladace/o_interface.o
NETWORK = clientsocket.o serversocket.o network.o ovladace/o_networked.o

OBJS = $(BASE) $(HADI) $(OVLADACE)

all : had

#had : $(OBJS) $(NETWORK)
had : $(OBJS)
	g++ $(LIBFLAGS) -o $@ $(OBJS)

%.o : %.cpp
	g++ $(INCFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OBJS) : had.hpp

$(NETWORK) main.o : network.hpp

main.o interface.o hadi/h_gl.o : interface.hpp

.PHONY : clean

clean :
	rm -f had *.o hadi/*.o ovladace/*.o
