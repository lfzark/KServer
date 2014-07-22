CC =		gcc
CFLAGS =	-O 
LDFLAGS =	-s $(SYSVLIBS)
CLEANC  = select 

all:		select

select:	select.o
	$(CC) select.o $(LDFLAGS) -o select -lpthread 

select.o:	select.c
	$(CC) $(CFLAGS) -c select.c -lpthread 

clean:
	rm -f  *.o  $(CLEANC)
