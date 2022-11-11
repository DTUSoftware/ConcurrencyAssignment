CC = gcc
CCOPTS = -c -O2 -g -Wall
LINKOPTS = -O2 -g

TARGET = bank
SRCS = main.c test.c
OBJECTS = $(SRCS:.c=.o)

LIBS = -pthread

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LINKOPTS) $(LIBS) -o $@ $^

%.o:%.c
	$(CC) $(CCOPTS) -o $@ $^

clean:
	- $(RM) $(EXEC)
	- $(RM) $(EXEC).exe
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) *.h.gch
	- $(RM) core.*

run: $(EXEC)
	$(EXEC)

pretty: 
	indent *.c *.h -kr
