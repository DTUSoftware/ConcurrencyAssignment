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
	- $(RM) $(TARGET)
	- $(RM) $(TARGET).exe
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) *.h.gch
	- $(RM) core.*

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) -test all

pretty: 
	indent *.c *.h -kr
