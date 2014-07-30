BINDIR =	/usr/local/sbin
CC =		gcc
CFLAGS =	-O 
LDFLAGS =	-s $(SYSVLIBS)

all:		kserver

kserver:	kserver.o
	$(CC) kserver.o $(LDFLAGS) -o kserver -lpthread 

kserver.o:	kserver.c
	$(CC) $(CFLAGS) -c kserver.c -lpthread 
 
install:	all
	rm -f $(BINDIR)/kserver
	cp kserver $(BINDIR)/kserver

clean:
	rm -f kserver *.o core core.* *.core
