CC=g++ -std=c++11
CFLAGS=-c -Wpedantic -Ofast -Wall -ggdb3 
SOURCES=client.cpp ../Communication.cpp ../StringArray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= whoClient
ARGS= -q ../client.txt -numThreads 10 -sp 56321 -sip 127.0.0.1 #test with big client file
ARGST= -q ../clientTiny.txt -numThreads 1 -sp 56321 -sip 127.0.0.1 #test with small client file
#-q query file -numthreads -sp ServerPort -sip ServerIP
VAL=valgrind --leak-check=full

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lpthread

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run:
	./$(EXECUTABLE) $(FILE) $(ARGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

val:
	$(VAL) ./$(EXECUTABLE) $(FILE) $(ARGS)

valt:
	$(VAL) ./$(EXECUTABLE) $(FILE) $(ARGST)

test:
	./$(EXECUTABLE) $(FILE) $(ARGST)