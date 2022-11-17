CC = gcc
CCOPTS = -c -O2 -g -Wall -Wextra
LINKOPTS = -O2 -g -Wall -Wextra

TARGET = bank
SRCS = main.c logic.c utils.c test.c
OBJECTS = $(SRCS:.c=.o)

LIBS = -pthread

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LINKOPTS) $(LIBS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CCOPTS) -o $@ $<

clean:
	- $(RM) $(TARGET)
	- $(RM) $(TARGET).exe
	- $(RM) $(OBJECTS)
	- $(RM) *~
	- $(RM) *.h.gch
	- $(RM) *.d
	- $(RM) core.*
	- $(RM) account_db

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) -test all

withdrawalTest: $(TARGET)
	./$(TARGET) -test withdrawal

depositTest: $(TARGET)
	./$(TARGET) -test deposit

transferTest: $(TARGET)
	./$(TARGET) -test transfer

pretty: 
	indent *.c *.h -kr
