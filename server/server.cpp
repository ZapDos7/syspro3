/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #3, Spring 2020	
 */
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h> /* sockets */
#include <netdb.h>	  /* gethostbyaddr */
#include <stdlib.h>	  /* exit */
#include <ctype.h>	  /* toupper */
#include <signal.h>	  /* signal */
#include <pthread.h>  /* threads */
#include "../Communication.h"

//#define PORT 56321 //query port
#define PORT 57642 //worker port

void sigchld_handler(int sig)
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

int main(int argc, char const *argv[])
{

	int q = -1; //query port num
	int s = -1; //workers port num
	int w = -1; //num of threads
	int b = -1; //bufferSize

	for (int i = 0; i < argc; i++)
	{
		if (strcmp("-q", argv[i]) == 0)
		{
			q = atoi(argv[i + 1]);
		}
		if (strcmp("-w", argv[i]) == 0)
		{
			w = atoi(argv[i + 1]);
		}
		if (strcmp("-b", argv[i]) == 0)
		{
			b = atoi(argv[i + 1]);
		}
		if (strcmp("-s", argv[i]) == 0)
		{
			s = atoi(argv[i + 1]);
		}
	}
	if ((w < 0) || (b < 0) || (s < 0) || (q < 0))
	{
		perror("critical error: arguements");
		exit(EXIT_FAILURE);
	}

	//communication
	Communication communicator(b);

	//socket time
	int server_fd, new_socket, valread; //1o: FD tou master socket, 2o: FD tou communication socket, 3o: return val tis read
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	// creating socket fd
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// attaching socket to the port
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) //fd, posa connection reqs 8a ginoun mexri na pw oti ok paei terma
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	
	while (true)
	{
		valread = read(new_socket, buffer, 1024);
		fprintf(stderr, "SERVER//poso: %d\t ti: %s\n",valread, buffer);
		if (string(buffer)=="BYE")
		{
			break;
		}
	}

	char *ack = "Eimai o server kai sou lew, client, oti elava ta mnmt sou!";
	send(new_socket, ack, strlen(ack), 0);
	//printf("Hello message sent\n");
	return 0;
}
//summaries
// ofstream summ_file;
// std::string onomaarxeiousum = "sum_file.txt";
// summ_file.open(onomaarxeiousum);

// //lipsi summaries
// for (int i = 0; i < w; i++)
// {
//     while (true)
//     {
//         char *buf = communicator.createBuffer();
//         communicator.recv(buf, pid_in_out.items[i].in);
//         //fprintf(stderr, "Elava apo to worker %d to minima: '%ld' \n", i, sizeof(buf));
//         if (string(buf) == "BYE")
//         { //elava to eidiko mhnuma oti that's a nono
//             break;
//         }
//         //and en einai BYE valto mesa
//         summ_file << buf << "\n";
//         communicator.destroyBuffer(buf);
//     }
// }
// summ_file.close();
//std::string com; //command
/*while (printf("?") && std::getline(std::cin, com))
    { //to "?" einai prompt gia ton user

        if (child_died == true)
        {
            fprintf(stderr, "kapoio paidi m kati epathe\n");
            //paw na brw poio pethane na to kanw respawn
            // for (int i = 0; i < w; i++)
            // {
            //     int killres = kill(pid_in_out.items[i].pid, 0);
            //     if (killres != 0) //brika to ptwma
            //     {
            //         int out_fd = open(names_out.items[i].c_str(), O_WRONLY);
            //         pid_in_out.items[i].out = out_fd;
            //         int in_fd = open(names_in.items[i].c_str(), O_RDONLY);
            //         pid_in_out.items[i].in = in_fd;

            //         pid_t child_pid = fork();
            //         if (child_pid == -1)
            //         {
            //             perror(" Failed to fork");
            //             exit(1);
            //         }
            //         if (child_pid == 0) //paidi
            //         {
            //             // call worker main agian
            //             child_pid = getpid();
            //             //cout << "child rebirthed with PID " << child_pid << endl;
            //             //kalw ti main tou paidiou
            //             return main_worker(in_dir, b, names_out.items[i].c_str(), names_in.items[i].c_str());
            //         }
            //         else //gonios
            //         {
            //             processIds.update_pid(pid_in_out.items[i].pid, child_pid);
            //             Triplette t(child_pid);
            //             pid_in_out.swap(pid_in_out.items[i].pid, t);
            //             pid_in_out.items[i].out = out_fd;
            //             pid_in_out.items[i].in = in_fd;

            //             //stelnw ksana tis xwres ston neo m gio
            //             // ta onoma twn pipes einai idia, oi FD's enimerothikan parapanw
            //             //cout << "aggregator opened pipes for worker: " << processIds.items[i] << endl;
            //             for (int j = 0; j < countries.size; j++) //tou ksanastelnw tis xwres tou
            //             {
            //                 if (j % w == i)
            //                 { //round robin diaxwrismos twn xwrwn ana worker
            //                     countries.items[j].pid = child_pid;
            //                     countries.items[j].out = out_fd;
            //                     countries.items[j].in = in_fd;

            //                     char *buf = communicator.createBuffer();
            //                     communicator.put(buf, countries.items[j].country);
            //                     communicator.send(buf, countries.items[j].out);
            //                     communicator.destroyBuffer(buf);
            //                 }
            //             }
            //             //aploikos elegxos gia liksi apostolis xwrwn se worker - epeidi den kseroume posa tha parei o ekastote worker
            //             char *buf = communicator.createBuffer();
            //             communicator.put(buf, "BYE");
            //             communicator.send(buf, out_fd);
            //             communicator.destroyBuffer(buf);
            //         }
            //     }
            // }
            child_died = false;
        }

        if (com.length() == 0)
        {
            continue; //ama m dwseis enter, sunexizw na zhtaw entoles
        }
        if (com[0] != '/')
        {
            com = "/" + com; //trexei kai me kai xwris / stis entoles
        }
        char *cstr = new char[com.length() + 1]; //auto 8a kanw tokenize
        strcpy(cstr, com.c_str());               //copy as string to line sto cstr
        char *pch;
        const char delim[2] = " ";
        pch = strtok(cstr, delim);
        std::string comms[8]; //i entoli me ta perissotera orismata einai h insertPatientRecord me d2
        int counter = 0;
        comms[0] = pch;
        //check first word to match with command, check entire command if correct

        if (comms[0] == "/listCountries") // /listCountries --> for each country print PID of corresponding worker
        {
            success++;
            total++;
            countries.print_lc(); //den xreiazetai epikoinwnia me workers giati exw "skonaki" ti domi countries
        }
        else if (comms[0] == "/exit")
        {
            for (int i = 0; i < w; i++)
            {
                char *buf = communicator.createBuffer();
                communicator.put(buf, comms[0]);
                communicator.send(buf, pid_in_out.items[i].out);
                communicator.destroyBuffer(buf);
            }
            std::string *results = new std::string[w];
            for (int i = 0; i < w; i++)
            {
                char *buf = communicator.createBuffer();
                communicator.recv(buf, pid_in_out.items[i].in);
                std::string tmp(buf);
                results[i] = tmp;
                communicator.destroyBuffer(buf);
            }
            int metritis = 0;
            int *noumera = new int(w);
            noumera[0] = total;
            noumera[1] = success;
            noumera[2] = failed;
            for (int i = 0; i < w; i++)
            {
                char *cstr2 = new char[results[i].length() + 1];
                strcpy(cstr2, com.c_str());
                char *pch2;
                const char delim2[2] = ","; // \0
                pch2 = strtok(cstr2, delim2);
                while (pch2 != NULL)
                {
                    noumera[metritis] += atoi(pch2);
                    metritis++;
                    pch2 = strtok(NULL, delim2);
                }
            }

            //free memory again as needed
            delete[] cstr;
            ofstream logfile;
            std::string onomaarxeiou = "log_file.";
            onomaarxeiou += to_string(getpid());
            logfile.open(onomaarxeiou);
            for (int i = 0; i < countries.size; i++) //grafw poies einai oi xwres m
            {
                logfile << countries.items[i].country << "\n";
            }
            logfile << "TOTAL: " << total << "\n";     //posa erwthmata mou irthan
            logfile << "SUCCESS: " << success << "\n"; //posa success
            logfile << "FAIL: " << failed << "\n";     //posa fail
            logfile.close();
            std::cout << "parent exiting\n";
            break;
        }
        else if (comms[0] == "/diseaseFrequency") //8. /diseaseFrequency virusName date1 date2 [country]
        {
            while (pch != NULL) //kovw tin entoli sta parts tis
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if ((counter > 5) || (counter < 4))
            {
                std::cerr << "error\n";
                failed++;
            }
            else if (counter == 5) //exw xwra ara paw se auti ti xwra --> se auto to process na kanw tin entoli
            {
                //find se poio worker einai auti i xwra
                //uparxei genika auti i xwra?
                bool uparxei = false;
                for (int i = 0; i < countries.size; i++)
                {
                    if (countries.items[i].country == comms[4])
                    {
                        uparxei = true;
                    }
                }
                if (uparxei == false)
                {
                    failed++;
                    fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                }
                int poios = 0;
                for (int i = 0; i < countries.size; i++) //gia kathe xwra
                {
                    if (countries.items[i].country == comms[4]) //an eisai auti pou psaxnw
                    {
                        //send to worker
                        std::cerr << comms[4] << "\n";
                        char *minima = new char[com.length() + 1];
                        strcpy(minima, com.c_str());                    //eidallws nmzei oti to com.c_str() einai const char *
                        char *buf = communicator.createBuffer();        //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, minima);                  //vazw to minima sto buf
                        communicator.send(buf, countries.items[i].out); //to kanw send ara write
                        communicator.destroyBuffer(buf);                //yeet
                        //o,ti lavw to tupwnw
                        poios = i;
                        break;
                    }
                }
                bool found = false;
                char *buf = communicator.createBuffer();
                communicator.recv(buf, countries.items[poios].in);
                //                communicator.recv(buf, pid_in_out.items[poios].in);
                if (string(buf) == "ERR") //an lathos command, fail++;
                {
                    failed++;
                    fprintf(stderr, "Lathos command wre \n");
                    exit(1);
                }
                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                {
                    //fprintf(stderr, "worker %d has not this record\n", pid_in_out.items[i].pid);
                }
                else //mou dwses apantisi ok
                {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                    found = true;
                    success++;
                    //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                    fprintf(stdout, "%s\n", buf);
                }

                communicator.destroyBuffer(buf);
                //metrites
                if (found == false) //teleiwsame kai kanenas den ton brike
                {
                    fprintf(stdout, "This disease doesn't exist in the database.\n");
                    failed++;
                }
            }
            else //if (counter==4) //ara den exw xwra ara ti zitaw apo olous
            {
                for (int i = 0; i < pid_in_out.size; i++) //gia kathe worker
                {
                    char *minima = new char[com.length() + 1];
                    strcpy(minima, com.c_str());                     //eidallws nmzei oti to com.c_str() einai const char *
                    char *buf = communicator.createBuffer();         //ftiaxnw ton buffer gia na steilw to mnm
                    communicator.put(buf, minima);                   //vazw to minima sto buf
                    communicator.send(buf, pid_in_out.items[i].out); //to kanw send ara write
                    communicator.destroyBuffer(buf);                 //yeet
                }

                bool found = false;
                int sum = 0;

                for (int i = 0; i < w; i++) //send se olous
                {
                    char *buf = communicator.createBuffer();
                    communicator.recv(buf, pid_in_out.items[i].in);
                    if (string(buf) == "ERR") //an lathos command, fail++;
                    {
                        failed++;
                        fprintf(stderr, "Lathos command wre \n");
                        exit(1);
                    }
                    else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                    {
                        //fprintf(stderr, "worker %d has not this record\n", pid_in_out.items[i].pid);
                    }
                    else //mou dwses apantisi ok
                    {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                        found = true;
                        success++;
                        //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);

                        sum += atoi(buf);
                    }

                    communicator.destroyBuffer(buf);
                    //metrites
                    if (found == false) //teleiwsame kai kanenas den ton brike
                    {
                        fprintf(stdout, "This disease doesn't exist in the database.\n");
                        failed++;
                    }
                    else
                    {
                        cout << sum << endl;
                    }
                }
            }
        }
        else if (comms[0] == "/topk-AgeRanges") // /topk-AgeRanges k country disease d1 d2
        {
            while (pch != NULL)
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if (counter != 6)
            {
                std::cerr << "Wrong amount of agruements\n";
                failed++;
                break;
            }
            //is k anamesa se 1 kai 4?
            int k = atoi(comms[1].c_str());
            if ((k < 1) || (k > 4))
            {
                fprintf(stderr, "Wrong value for k.\n");
                failed++;
                break;
            }
            //find se poio worker einai auti i xwra
            //uparxei genika auti i xwra?
            bool uparxei = false;
            for (int i = 0; i < countries.size; i++)
            {
                if (countries.items[i].country == comms[2])
                {
                    uparxei = true;
                }
            }
            if (uparxei == false)
            {
                failed++;
                fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                break;
            }
            int poios = -1;
            for (int i = 0; i < countries.size; i++) //gia kathe xwra
            {
                if (countries.items[i].country == comms[2]) //an eisai auti pou psaxnw
                {
                    //send to worker
                    //std::cerr << comms[4] << "\n";
                    char *minima = new char[com.length() + 1];
                    strcpy(minima, com.c_str());                    //eidallws nmzei oti to com.c_str() einai const char *
                    char *buf = communicator.createBuffer();        //ftiaxnw ton buffer gia na steilw to mnm
                    communicator.put(buf, minima);                  //vazw to minima sto buf
                    communicator.send(buf, countries.items[i].out); //to kanw send ara write
                    communicator.destroyBuffer(buf);                //yeet
                    poios = i;
                    break;
                }
            }
            //paw na dw tin apantisi
            char *buf = communicator.createBuffer();
            communicator.recv(buf, countries.items[poios].in);
            if (string(buf) == "ERR") //an lathos command, fail++;
            {
                fprintf(stderr, "Lathos command wre \n");
            }
            else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
            {
                fprintf(stderr, "No reply yielded\n");
            }
            else //mou dwses apantisi ok
            {
                fprintf(stdout, "%s\n", buf);
            }
            communicator.destroyBuffer(buf);
        }
        else if (comms[0] == "/searchPatientRecord") // /searchPatientRecord id
        {
            for (int i = 0; i < w; i++) //send se olous
            {
                char *buf = communicator.createBuffer();         //ftiaxnw ton buffer gia na steilw to mnm
                communicator.put(buf, com.c_str());              //vazw oli tin entoli sto buf
                communicator.send(buf, pid_in_out.items[i].out); //to kanw send ara write
                communicator.destroyBuffer(buf);                 //yeet
            }
            bool found = false;
            for (int i = 0; i < w; i++) //send se olous
            {
                char *buf = communicator.createBuffer();
                communicator.recv(buf, pid_in_out.items[i].in);
                if (string(buf) == "ERR") //an lathos command, fail++;
                {
                    failed++;
                    fprintf(stderr, "Lathos command wre \n");
                    exit(1);
                }
                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                {
                    //fprintf(stderr, "worker %d has not this record\n", pid_in_out.items[i].pid);
                }
                else //mou dwses apantisi ok
                {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                    found = true;
                    success++;
                    //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                    fprintf(stdout, "%s\n", buf);
                }

                communicator.destroyBuffer(buf);
                //metrites
                if (found == false) //teleiwsame kai kanenas den ton brike
                {
                    fprintf(stdout, "This record doesn't exist in the database.\n");
                    failed++;
                }
            }
        }
        else if (comms[0] == "/numPatientAdmissions") //  /numPatientAdmissions disease d1 d2 [country]
        {
            while (pch != NULL) //kovw tin entoli sta parts tis
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if ((counter != 5) && (counter != 4))
            {
                std::cerr << "error edw edw edw\n";
                failed++;
            }
            //uparxei genika auti i xwra?
            if (counter == 5) //exw country
            {
                //uparxei?
                bool uparxei = false;
                for (int i = 0; i < countries.size; i++)
                {
                    if (countries.items[i].country == comms[4])
                    {
                        uparxei = true;
                    }
                }
                if (uparxei == false)
                {
                    failed++;
                    fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                    break;
                }
                //find se poio worker einai auti i xwra
                int poios = -1;
                for (int i = 0; i < countries.size; i++) //gia kathe xwra
                {
                    if (countries.items[i].country == comms[4]) //an eisai auti pou psaxnw
                    {
                        //send to worker
                        std::cerr << comms[4] << "\n";
                        char *minima = new char[com.size() + 1];
                        strcpy(minima, com.c_str());                    //eidallws nmzei oti to com.c_str() einai const char *
                        char *buf = communicator.createBuffer();        //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, minima);                  //vazw to minima sto buf
                        communicator.send(buf, countries.items[i].out); //to kanw send ara write
                        communicator.destroyBuffer(buf);                //yeet
                        poios = i;
                        break;
                    }
                }
                char *buf = communicator.createBuffer();
                communicator.recv(buf, countries.items[poios].in);
                //communicator.recv(buf, pid_in_out.items[poios].in);
                if (string(buf) == "ERR") //an lathos command, fail++;
                {
                    fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                    break;
                }
                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                {
                    fprintf(stdout, "No relevant information exists in database.\n");
                }
                else //mou dwses apantisi ok
                {
                    //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                    fprintf(stdout, "%s %s\n", countries.items[poios].country.c_str(), buf);
                }
                communicator.destroyBuffer(buf);
            }
            else //den exw country ara send to all
            {
                for (int i = 0; i < pid_in_out.size; i++) //gia kathe worker
                {
                    char *minima = new char[com.length() + 1];
                    strcpy(minima, com.c_str());                     //eidallws nmzei oti to com.c_str() einai const char *
                    char *buf = communicator.createBuffer();         //ftiaxnw ton buffer gia na steilw to mnm
                    communicator.put(buf, minima);                   //vazw to minima sto buf
                    communicator.send(buf, pid_in_out.items[i].out); //to kanw send ara write
                    communicator.destroyBuffer(buf);                 //yeet
                }
                //replies time
                //std::string replies[w];
                StringArray replies(w);
                for (int i = 0; i < w; i++) //receive apo olous
                {
                    char *buf = communicator.createBuffer();
                    communicator.recv(buf, pid_in_out.items[i].in);
                    if (string(buf) == "ERR") //an lathos command, fail++;
                    {
                        fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                        //break;
                    }
                    else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                    {
                        //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                        fprintf(stderr, "Worder %d doesn't have disease %s\n", countries.items[i].pid, comms[1].c_str());
                        //break;
                    }
                    else //mou dwses apantisi ok
                    {
                        //buf = "Xwra Num\nXwra Num\n etc"
                        std::string temp_reply(buf);
                        replies.insert(temp_reply);
                    }
                    communicator.destroyBuffer(buf);
                }
                //tupwnw
                for (int i = 0; i < replies.size; i++)
                {
                    fprintf(stdout, "%s", replies.items[i].c_str());
                }
                fprintf(stdout, "\n");
            }
        }
        else if (comms[0] == "/numPatientDischarges") //  /numPatientDischarges disease d1 d2 [country]
        {
            while (pch != NULL) //kovw tin entoli sta parts tis
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if ((counter != 5) && (counter != 4))
            {
                std::cerr << "error edw edw edw\n";
                failed++;
            }
            //uparxei genika auti i xwra?
            if (counter == 5) //exw country
            {
                //uparxei?
                bool uparxei = false;
                for (int i = 0; i < countries.size; i++)
                {
                    if (countries.items[i].country == comms[4])
                    {
                        uparxei = true;
                    }
                }
                if (uparxei == false)
                {
                    failed++;
                    fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                    break;
                }
                //find se poio worker einai auti i xwra
                int poios = -1;
                for (int i = 0; i < countries.size; i++) //gia kathe xwra
                {
                    if (countries.items[i].country == comms[4]) //an eisai auti pou psaxnw
                    {
                        //send to worker
                        std::cerr << comms[4] << "\n";
                        char *minima = new char[com.size() + 1];
                        strcpy(minima, com.c_str());                    //eidallws nmzei oti to com.c_str() einai const char *
                        char *buf = communicator.createBuffer();        //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, minima);                  //vazw to minima sto buf
                        communicator.send(buf, countries.items[i].out); //to kanw send ara write
                        communicator.destroyBuffer(buf);                //yeet
                        poios = i;
                        break;
                    }
                }
                char *buf = communicator.createBuffer();
                communicator.recv(buf, countries.items[poios].in);
                //communicator.recv(buf, pid_in_out.items[poios].in);
                if (string(buf) == "ERR") //an lathos command, fail++;
                {
                    fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                    break;
                }
                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                {
                    fprintf(stdout, "No relevant information exists in database.\n");
                }
                else //mou dwses apantisi ok
                {
                    //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                    fprintf(stdout, "%s %s\n", countries.items[poios].country.c_str(), buf);
                }
                communicator.destroyBuffer(buf);
            }
            else //den exw country ara send to all
            {
                for (int i = 0; i < pid_in_out.size; i++) //gia kathe worker
                {
                    char *minima = new char[com.length() + 1];
                    strcpy(minima, com.c_str());                     //eidallws nmzei oti to com.c_str() einai const char *
                    char *buf = communicator.createBuffer();         //ftiaxnw ton buffer gia na steilw to mnm
                    communicator.put(buf, minima);                   //vazw to minima sto buf
                    communicator.send(buf, pid_in_out.items[i].out); //to kanw send ara write
                    communicator.destroyBuffer(buf);                 //yeet
                }
                //replies time
                //std::string replies[w];
                StringArray replies(w);
                for (int i = 0; i < w; i++) //receive apo olous
                {
                    char *buf = communicator.createBuffer();
                    communicator.recv(buf, pid_in_out.items[i].in);
                    if (string(buf) == "ERR") //an lathos command, fail++;
                    {
                        fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                        //break;
                    }
                    else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                    {
                        //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                        fprintf(stderr, "Worder %d doesn't have disease %s\n", countries.items[i].pid, comms[1].c_str());
                        //break;
                    }
                    else //mou dwses apantisi ok
                    {
                        //buf = "Xwra Num\nXwra Num\n etc"
                        std::string temp_reply(buf);
                        replies.insert(temp_reply);
                    }
                    communicator.destroyBuffer(buf);
                }
                //tupwnw
                for (int i = 0; i < replies.size; i++)
                {
                    fprintf(stdout, "%s", replies.items[i].c_str());
                }
                fprintf(stdout, "\n");
            }
        }
        else
        {
            failed++;
            std::cerr << "error\n";
        }
    } //end while(1)
*/