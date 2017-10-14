CC=g++
SOCKDIR = ./socketCode
SERVERSRC=p1.cc utility.cc manipulate.cc $(SOCKDIR)/Verify333.c $(SOCKDIR)/CSE333Socket.cc $(SOCKDIR)/ClientSocket.cc $(SOCKDIR)/ServerSocket.cc
SERVERHEADER=module.h utility.h gamedef.h gamestate.h board.h manipulate.h $(SOCKDIR)/CSE333.h $(SOCKDIR)/CSE333Socket.h $(SOCKDIR)/ClientSocket.h $(SOCKDIR)/ServerSocket.h
CLIENTSRC=p2.cc utility.cc manipulate.cc $(SOCKDIR)/ClientSocket.cc $(SOCKDIR)/CSE333Socket.cc
CLIENTHEADER=module.h utility.h gamedef.h gamestate.h board.h manipulate.h $(SOCKDIR)/ClientSocket.h $(SOCKDIR)/CSE333Socket.h

LIBS=./array2d/libhw2.a
CFLAGS = -Wall -std=c++11
IFLAGS =  -I./array2d -I./jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L./array2d -L./jansson/lib -larray2d -l:libjansson.a -lpthread
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`
all: server client
server: $(SERVERSRC) $(SERVERHEADER) $(LIBS)
	$(CC) $(CFLAGS) -O0  -Wno-write-strings $(IFLAGS) $(GTKCFLAGS) $(SERVERSRC) $(GTKLDFLAGS) $(LDFLAGS) -o p1
client: $(CLIENTSRC) $(CLIENTHEADER) $(LIBS)
	$(CC) $(CFLAGS) -O0  -Wno-write-strings $(IFLAGS) $(GTKCFLAGS) $(CLIENTSRC) $(GTKLDFLAGS) $(LDFLAGS) -o p2
clean:
	rm -f *.o *~ p1 p2 makegame temp
