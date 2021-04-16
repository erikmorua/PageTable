# Nathan Azoulay, cssc3302 Red ID: 821871239
# Erik Morua, cssc3333, Red ID: 816670615
# makefile

CC =gcc 
CCFLAGS=-c -Wall


all: pagetable 

pagetable: ptable.o ptableHelper.o byu_tracereader.o
	$(CC) ptable.o ptableHelper.o byu_tracereader.o  $(LFLAGS) -o pagetable -lm

ptable.o: ptable.c 
	$(CC) $(CCFLAGS) ptable.c ptableHelper.h

ptableHelper.o: ptableHelper.c 
	$(CC) $(CCFLAGS) ptableHelper.c ptableHelper.h

byu_tracereader.o: byu_tracereader.c 
	$(CC) $(CCFLAGS) byu_tracereader.c 

clean:
	$(RM) *.o pagetable

