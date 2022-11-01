CC = gcc
CCOPTS = -c -g -Wall
LINKOPTS = -g -lrt 

EXEC=bank
OBJECTS=main.c main.h

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LINKOPTS) -c -O2 $@ $^

%.o:%.c
	$(CC) $(CCOPTS) -c -O2 $@ $^

clean:
	- $(RM) $(EXEC)
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) core.*

run: $(EXEC)
	$(EXEC)

pretty: 
	indent *.c *.h -kr
