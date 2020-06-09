CC=g++ -std=c++11
CFLAGS=-c -Wpedantic -Ofast -Wall -ggdb3
SOURCES=main_aggregator.cpp main_worker.cpp date.cpp record.cpp ht.cpp aht.cpp bb.cpp tree.cpp heap.cpp Communication.cpp Pair.cpp PairArray.cpp PidArray.cpp quicksort.cpp StringArray.cpp Triplette.cpp TripleArray.cpp date_format.cpp Summ.cpp SummArray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=master
ARGS= -w 3 -b 4096 -s 46.246.193.8 -p 57642 -i ../../input_dir_large
#-w -> number of workers, -b -> buffersize for pipes, -s -> server IP  -i -> input directory, -p -> serverPort
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

test:
	./$(EXECUTABLE) $(FILE) -w 1 -b 4096 -s 46.246.193.8 -p 57642 -i ../../input_dir_tiny