/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #3, Spring 2020	
 */
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include "../Communication.h"
#include "../StringArray.h"

void *ask(void *args)
{
	//thread asks query to server & then receives answer by a worker
}

int main(int argc, char const *argv[])
{
	char query_file[256]; //query file name
	int numThreads = -1;  //numthreads
	uint16_t sp = -1;	  //server port
	char sip[256];		  //serverIP

	for (int i = 0; i < argc; i++)
	{
		if (strcmp("-q", argv[i]) == 0)
		{
			strcpy(query_file, argv[i + 1]);
		}
		if (strcmp("-numThreads", argv[i]) == 0)
		{
			numThreads = atoi(argv[i + 1]);
		}
		if (strcmp("-sp", argv[i]) == 0)
		{
			sp = (uint16_t)atoi(argv[i + 1]);
		}
		if (strcmp("-sip", argv[i]) == 0)
		{
			strcpy(sip, argv[i + 1]);
		}
	}
	if ((sp < 0) || (numThreads < 0))
	{
		fprintf(stderr, "critical error: arguements\n");
		exit(EXIT_FAILURE);
	}

	//edw akouw ton server
	int accept_server_fd = Communication::create_connecting_socket(sip, sp);
	cout << "Accept server at port " << sp << " via FD: " << accept_server_fd << endl;

	Communication communicator(4096);

	//diavasma query file
	std::ifstream dataset(query_file);
	std::string line;
	int posa_commands = 0;
	StringArray entoles;
	while (std::getline(dataset, line))
	{
		entoles.insert(line);
		posa_commands++;
		char * lineCstr = new char[line.length() + 1];
		strcpy (lineCstr, line.c_str());
		communicator.send(lineCstr, accept_server_fd);
	}
	//arxika aplws stelnw 1-1 ston server
	//epeita numThreads

	return 0;
}
