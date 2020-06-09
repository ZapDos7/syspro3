CC=g++ -std=c++11
CFLAGS=-c -Wpedantic -Ofast -Wall -ggdb3 -lpthread
SOURCES=client.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= whoClient
ARGS= -q client.txt -10 -sp A -sip 46.246.193.8
#-q query file -numthreads -sp ServerPort -sip ServerIP
VAL=valgrind --leak-check=full

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run:
	./$(EXECUTABLE) $(FILE) $(ARGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

val:
	$(VAL) ./$(EXECUTABLE) $(FILE) $(ARGS)