CC=g++ -std=c++11
CFLAGS=-c -Wpedantic -Ofast -Wall -ggdb3 -lpthread
SOURCES=server.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE= whoServer
ARGS= -q 55000 -10 -s 56000 -w 2 -b 4096
#-q queryportNumber -s statiscticsportnumber -w numthreads -b buffersize
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

status:
	netstat -ant