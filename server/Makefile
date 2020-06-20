CC=g++ -std=c++11
CFLAGS=-c -pthread -Wpedantic -Ofast -Wall -ggdb3
SOURCES=server.cpp ../Communication.cpp PortCountries.cpp PCArray.cpp Queue.cpp ../StringArray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= whoServer
ARGS= -q 56321 -s 57642 -w 2 -b 4096
#-q queryportNumber -s statiscticsportnumber -w numthreads -b buffersize
VAL=valgrind --leak-check=full

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@  -lpthread 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

run:
	./$(EXECUTABLE) $(FILE) $(ARGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

val:
	$(VAL) ./$(EXECUTABLE) $(FILE) $(ARGS)

status:
	netstat -ant