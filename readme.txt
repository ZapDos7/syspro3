Ioanna Zapalidi
sdi1400044

3rd System Programming Assignment

Used: C++11 (Developed on Linux Ubuntu 16.04), used git for version control and valgrind for debugging purposes.

Contents:
- this readme
- previous assignment's files (bb, date, heap, ht, record, tree, date_format, quicksort, Pair, Triplette, PairArray, PidArray, TripleArray, StringArray .cpp & .h files, main_aggregator.cpp, main_worker.cpp/.h)

Use:
- While in each project subfolder, run "$make" to create the .o files and the executable
- In each host, we execute "curl ifconfig.me" and the result is this host's IP address, which we pass as arguement properly in the Makefile (currently has my own PC's address for debug purposes)
- execute "$make run" to execute (to see the parameters values and alter them, see the Makefile file)
- execute "$make val" to execute using valgrind (requires it to be installed: sudo apt-get install valgrind)
- execute "$make clean" to clean the working folder. (leaves log files intact)
- execute "$make status" in server's folder to check status of sockets

Notes:
- Arbitrary selection of ports for my project are as follows: serverPort = 57642

Sources:
- class' notes
- My previous projects for this course.