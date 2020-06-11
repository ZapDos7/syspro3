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

#define PORT 56321

int main(int argc, char const *argv[])
{
	char query_file[256]; //query file name
	int numThreads = -1;  //numthreads
	int sp = -1;		  //server port
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
			sp = atoi(argv[i + 1]);
		}
		if (strcmp("-sip", argv[i]) == 0)
		{
			strcpy(sip, argv[i + 1]);
		}
	}
	if ((sp < 0) || (numThreads < 0))
	{
		perror("critical error: arguements");
		exit(-1);
	}

	//Communication communicator;

	//arxiko setarisma socket
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	//diavasma query file
	std::ifstream dataset(query_file);
	std::string line;
	int posa_commands = 0;
	while (std::getline(dataset, line))
	{
		posa_commands++;
	}
	std::string commsStr = to_string(posa_commands);
	//fprintf(stderr, "posa commands exw: %s\n\n", commsStr.c_str());
	send(sock, commsStr.c_str(), commsStr.length(), 0);
	//ksana, stelnw poses entoles kai ti paizei
	while (std::getline(dataset, line))
	{
		send(sock, line.c_str(), line.length(), 0);
	}
	send(sock, "BYE", strlen("BYE"), 0);
	//gia kathe entoli create thread
	//when all read, send all via threads SIMULTANEOUSLY
	//otan i entoli apostalei, kathe thread --> print reply apo server in stdout, thread.telos
	//when all threads done, client done

	//!!!!!!! mutexes gia prostasia shared variables between threads!!
	//oxi busy waiting

	valread = read(sock, buffer, 1024);
	fprintf(stderr, "CLIENT//poso: %d\t ti: %s\n", valread, buffer);
	return 0;
}
