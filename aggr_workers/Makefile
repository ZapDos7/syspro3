CC=g++ -std=c++11
CFLAGS=-c -Wpedantic -Ofast -Wall -ggdb3
SCRIPT = create_infiles.sh
SOURCES=main_aggregator.cpp main_worker.cpp date.cpp record.cpp ht.cpp aht.cpp bb.cpp tree.cpp heap.cpp Communication.cpp Pair.cpp PairArray.cpp PidArray.cpp quicksort.cpp StringArray.cpp Triplette.cpp TripleArray.cpp date_format.cpp Summ.cpp SummArray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=diseaseAggregator
ARGS= -w 3 -b 4096 -i ../input_dir_large
#-w -> number of workers, -b -> buffersize for pipes, -i -> input directory
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

#test:
#$(CC) -c test.cpp date.cpp record.cpp ht.cpp aht.cpp bb.cpp tree.cpp heap.cpp Communication.cpp Pair.cpp PairArray.cpp PidArray.cpp quicksort.cpp StringArray.cpp Triplette.cpp TripleArray.cpp date_format.cpp 
#g++ -o test test.o  date.o record.o ht.o aht.o bb.o tree.o heap.o Communication.o Pair.o PairArray.o PidArray.o quicksort.o StringArray.o Triplette.o TripleArray.o date_format.o 

#runtest:
#./test -w 3 -b 4096 -i ../input_dir_tiny

test1:
	./$(EXECUTABLE) -w 1 -b 4096 -i ../input_dir_tiny

test3:
	./$(EXECUTABLE) -w 3 -b 4096 -i ../input_dir_tiny

cleanlog:
	rm log_file.*

val:
	$(VAL) ./$(EXECUTABLE) $(FILE) $(ARGS)

script:
	./$(SCRIPT) diseasesFile.txt countriesFileSmall.txt ../dir 5 10