Ioanna Zapalidi
sdi1400044

3rd System Programming Assignment

Used: C++11 (Developed on Linux Ubuntu 16.04), used git for version control and valgrind for debugging purposes.

Contents:
- this readme
- previous assignment's files (bb, date, heap, ht, record, tree, date_format, quicksort, Pair, Triplette, PairArray, PidArray, TripleArray, StringArray .cpp & .h files, main_aggregator.cpp, main_worker.cpp/.h)
- client/ThreadArgsC.h is a class containing all the arguements to be passed to client threads (so we don't need global variables)
- server/ThreadArgs.h is a class containing all the arguements to be passed to threads (so we don't need global variables)
- server/Queue.h/.cpp: queue class and methods (circular buffer implementation)
- server/PortCountries.h/.cpp: a class that combines the IP, port number and an array of countries, so the server keeps track of where to send each country-based query
- server/PCArray.h/.cpp: array of PortCountries items.

Use:
- While in each project subfolder, run "$make" to create the .o files and the executable
- In each host, we execute "curl ifconfig.me" or "hostname -i" and the result is this host's IP address, which we pass as arguement properly in the Makefile (currently has my own PC's address for debug purposes)
- execute "$make run" to execute (to see the parameters values and alter them, see the Makefile file)
- execute "$make val" to execute using valgrind (requires it to be installed: sudo apt-get install valgrind)
- execute "$make clean" to clean the working folder. (leaves log files intact)
- execute "$make status" in server's folder to check status of sockets
- Execution order: server, master/workers, client.

Notes:
- Arbitrary selection of ports for my project are as follows: client to server connect on 56321 and workers to server on 58987
- The previously created "Connection" class implements the protocol here as well, expanding the previous version of it so it handles communication between sockets as well as through pipes.
- When the server receives a message beginning with 'W', it was sent by a worker, and when it begins with 'C', by a client thread.
- The workers initially send: one integer (size of circular buffer), one integer (how many workers are there), one string (contains info on each worker's IP, PORT and countries it has saved in its database). Afterwards, all workers send their summaries/statistics, which are written in the file "sum_file.txt" in order to reduce the clutter on the terminal.

Sources:
- class' notes
- My previous projects for this course.