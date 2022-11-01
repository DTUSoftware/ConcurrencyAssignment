CC = gcc
CCOPTS = -c -g -Wall
LINKOPTS = -g

EXEC=bank
OBJECTS=main.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LINKOPTS) -o $@ $^

%.o:%.c
	$(CC) $(CCOPTS) -O2 $@ $^

clean:
	- $(RM) $(EXEC)
	- $(RM) $(EXEC).exe
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) core.*

run: $(EXEC)
	$(EXEC)

pretty: 
	indent *.c *.h -kr
