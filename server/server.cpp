/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #3, Spring 2020	
 */
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <fstream>
#include "../Communication.h"
#include "../StringArray.h"
#include "PortCountries.h"
#include "PCArray.h"
#include "ThreadArgs.h"
#include "Queue.h"

PCArray skonaki;              //edw o server & its threads kseroun poio socket (IP:port) antistoixei se poia/es xwra/es
pthread_mutex_t skonaki_lock; //mutex gia elegxo poios grafei sto skonaki

ofstream summ_file;             //arxeio opou o server grafei ta summaries
pthread_mutex_t summ_file_lock; //o mutex tou summaries file

std::ifstream queries;
pthread_mutex_t query_file_lock;

int posoi = 0;
pthread_mutex_t posoi_worker_lock;

void *accept_client(void *args)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    ThreadArgs *pthreadargs = (ThreadArgs *)args;
    int new_socket;

    while (true)
    {
        cout << "Waiting for a client ... " << endl;
        if ((new_socket = accept(pthreadargs->accept_client_fd, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("accept");
            break;
        }
        else
        {
            pthreadargs->queue->place(new_socket);
        }
    }

    return 0;
}

void *accept_workers(void *args)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    ThreadArgs *pthreadargs = (ThreadArgs *)args;
    int new_socket;

    while (true)
    {
        cout << "Waiting for a worker ... " << endl;
        if ((new_socket = accept(pthreadargs->accept_worker_fd, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("accept");
            break;
        }
        else
        {
            pthreadargs->queue->place(new_socket);
        }
    }

    return 0;
}

void *service(void *args)
{
    ThreadArgs *pthreadargs = (ThreadArgs *)args;
    char token;

    while (true)
    {
        cout << "Waiting to service a node ... " << endl;
        int fd = pthreadargs->queue->obtain();

        if (fd <= 0)
        {
            break;
        }
        else
        {
            int n = read(fd, &token, sizeof(token));

            if (n <= 0)
            {
                perror("token read failed");
                break;
            }

            if (token == 'C') // client
            {
                //tha exei idi timi epeidi prwta m lene oi Worker o,ti theloun k meta egw apantaw
                Communication communicator(pthreadargs->b);
                // read command from client
                // char *buff = communicator.createBuffer();
                // communicator.recv(buff, fd);
                // fprintf(stderr, "entoli apo client: %s\n", buff);
                // communicator.destroyBuffer(buff);

                //temporary solution - prin ftiaksoume ton client exoume moufa client == file
                pthread_mutex_lock(&query_file_lock);
                queries.open("../client.txt");
                long int total = 0;
                long int success = 0;
                long int failed = 0;
                std::string command;
                while (std::getline(queries, command))
                {
                    if (command.length() == 0)
                    {
                        continue; //ama m dwseis enter, sunexizw na zhtaw entoles
                    }
                    if (command[0] != '/')
                    {
                        command = "/" + command; //trexei kai me kai xwris / stis entoles
                    }
                    fprintf(stderr, "Elava tin entoli: '%s'\n", command.c_str());
                    char *cstr = new char[command.length() + 1]; //auto 8a kanw tokenize
                    strcpy(cstr, command.c_str());               //copy as string to line sto cstr
                    char *pch;
                    const char delim[2] = " ";
                    pch = strtok(cstr, delim);
                    std::string comms[8]; //i entoli me ta perissotera orismata einai h insertPatientRecord me d2
                    int counter = 0;
                    comms[0] = pch;
                    //check first word to match with command, check entire command if correct
                    if (comms[0] == "/exit") //paei se olous
                    {
                        for (int i = 0; i < posoi; i++) //send se kathe worker
                        {
                            char *buf = communicator.createBuffer();
                            communicator.put(buf, comms[0]);
                            uint16_t portaki = skonaki.items[i].my_port;
                            int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                            communicator.send(buf, connecting_fd);
                            communicator.destroyBuffer(buf);
                        }
                        std::string *results = new std::string[posoi];
                        for (int i = 0; i < posoi; i++)
                        {
                            char *buf = communicator.createBuffer();
                            uint16_t portaki = skonaki.items[i].my_port;
                            int listening_fd = Communication::create_listening_socket(portaki, 100);
                            communicator.recv(buf, listening_fd);
                            std::string tmp(buf);
                            results[i] = tmp;
                            communicator.destroyBuffer(buf);
                        }
                        int metritis = 0;
                        int *noumera = new int(posoi);
                        noumera[0] = total;
                        noumera[1] = success;
                        noumera[2] = failed;
                        for (int i = 0; i < posoi; i++)
                        {
                            char *cstr2 = new char[results[i].length() + 1];
                            strcpy(cstr2, command.c_str());
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
                        for (int i = 0; i < skonaki.size; i++) //grafw poies einai oi xwres m
                        {
                            for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                            {
                                logfile << skonaki.items[i].my_countries.items[j] << "\n";
                            }
                        }
                        logfile << "TOTAL: " << total << "\n";     //posa erwthmata mou irthan
                        logfile << "SUCCESS: " << success << "\n"; //posa success
                        logfile << "FAIL: " << failed << "\n";     //posa fail
                        logfile.close();
                        std::cout << "C exiting\n";
                        break;
                    }                                         //exit
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
                            for (int i = 0; i < skonaki.size; i++)
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4])
                                    {
                                        uparxei = true;
                                    }
                                }
                            }
                            if (uparxei == false)
                            {
                                failed++;
                                fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                            }
                            int poios = 0;
                            for (int i = 0; i < skonaki.size; i++) //gia kathe xwra
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4]) //an eisai auti pou psaxnw
                                    {
                                        //send to worker
                                        //std::cerr << comms[4] << "\n";
                                        char *minima = new char[command.length() + 1];
                                        strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                        communicator.put(buf, minima);           //vazw to minima sto buf
                                        uint16_t portaki = skonaki.items[i].my_port;
                                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                        communicator.send(buf, connecting_fd);
                                        communicator.destroyBuffer(buf); //yeet
                                        //o,ti lavw to tupwnw
                                        poios = i;
                                        break;
                                    }
                                }
                            }
                            bool found = false;
                            char *buf = communicator.createBuffer();

                            uint16_t portaki = skonaki.items[poios].my_port;
                            int listening_fd = Communication::create_listening_socket(portaki, 100);
                            communicator.recv(buf, listening_fd);
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
                            for (int i = 0; i < skonaki.size; i++) //gia kathe worker
                            {
                                char *minima = new char[command.length() + 1];
                                strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                communicator.put(buf, minima);           //vazw to minima sto buf
                                uint16_t portaki = skonaki.items[i].my_port;
                                int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                communicator.send(buf, connecting_fd);
                                communicator.destroyBuffer(buf); //yeet
                            }

                            bool found = false;
                            int sum = 0;

                            for (int i = 0; i < skonaki.size; i++) //send se olous
                            {
                                char *buf = communicator.createBuffer();

                                uint16_t portaki = skonaki.items[i].my_port;
                                int listening_fd = Communication::create_listening_socket(portaki, 100);
                                communicator.recv(buf, listening_fd);
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
                        //uparxei genika auti i xwra?
                        bool uparxei = false;
                        fprintf(stderr, "elegxw xwra: %s\n", comms[2].c_str());
                        for (int i = 0; i < skonaki.size; i++)
                        {
                            for (int j = 0; j < skonaki.items[i].my_countries.size; i++)
                            {
                                if (skonaki.items[i].my_countries.items[j] == comms[2])
                                {
                                    uparxei = true;
                                }
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
                        for (int i = 0; i < skonaki.size; i++) //gia kathe xwra
                        {
                            for (int j = 0; j < skonaki.items[i].my_countries.size; i++)
                            {
                                if (skonaki.items[i].my_countries.items[j] == comms[2]) //an eisai auti pou psaxnw
                                {
                                    //send to worker
                                    //std::cerr << comms[4] << "\n";
                                    char *minima = new char[command.length() + 1];
                                    strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                    char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                    communicator.put(buf, minima);           //vazw to minima sto buf
                                    uint16_t portaki = skonaki.items[i].my_port;
                                    int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                    communicator.send(buf, connecting_fd);
                                    communicator.destroyBuffer(buf); //yeet
                                    poios = i;
                                    break;
                                }
                            }
                        }

                        //paw na dw tin apantisi
                        char *buf = communicator.createBuffer();
                        uint16_t portaki = skonaki.items[poios].my_port;
                        int listening_fd = Communication::create_listening_socket(portaki, 100);
                        communicator.recv(buf, listening_fd);
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
                        for (int i = 0; i < skonaki.size; i++) //send se olous
                        {
                            char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                            communicator.put(buf, command.c_str());  //vazw oli tin entoli sto buf
                            uint16_t portaki = skonaki.items[i].my_port;
                            int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                            communicator.send(buf, connecting_fd);
                            communicator.destroyBuffer(buf); //yeet
                        }
                        bool found = false;
                        for (int i = 0; i < posoi; i++) //send se olous
                        {
                            char *buf = communicator.createBuffer();
                            uint16_t portaki = skonaki.items[i].my_port;
                            int listening_fd = Communication::create_listening_socket(portaki, 100);
                            communicator.recv(buf, listening_fd);
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
                            for (int i = 0; i < skonaki.size; i++)
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4])
                                    {
                                        uparxei = true;
                                    }
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
                            for (int i = 0; i < skonaki.size; i++) //gia kathe xwra
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4]) //an eisai auti pou psaxnw
                                    {
                                        //send to worker
                                        std::cerr << comms[4] << "\n";
                                        char *minima = new char[command.size() + 1];
                                        strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                        communicator.put(buf, minima);           //vazw to minima sto buf
                                        uint16_t portaki = skonaki.items[i].my_port;
                                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                        communicator.send(buf, connecting_fd);
                                        communicator.destroyBuffer(buf); //yeet
                                        poios = i;
                                        break;
                                    }
                                }
                            }
                            char *buf = communicator.createBuffer();
                            uint16_t portaki = skonaki.items[poios].my_port;
                            int listening_fd = Communication::create_listening_socket(portaki, 100);
                            communicator.recv(buf, listening_fd);
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
                                fprintf(stdout, "%s %s\n", comms[4].c_str(), buf);
                            }
                            communicator.destroyBuffer(buf);
                        }
                        else //den exw country ara send to all
                        {
                            for (int i = 0; i < skonaki.size; i++) //gia kathe worker
                            {
                                char *minima = new char[command.length() + 1];
                                strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                communicator.put(buf, minima);           //vazw to minima sto buf
                                uint16_t portaki = skonaki.items[i].my_port;
                                int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                communicator.send(buf, connecting_fd);
                                communicator.destroyBuffer(buf); //yeet
                            }
                            //replies time
                            //std::string replies[w];
                            StringArray replies(posoi);
                            for (int i = 0; i < posoi; i++) //receive apo olous
                            {
                                char *buf = communicator.createBuffer();
                                uint16_t portaki = skonaki.items[i].my_port;
                                int listening_fd = Communication::create_listening_socket(portaki, 100);
                                communicator.recv(buf, listening_fd);
                                if (string(buf) == "ERR") //an lathos command, fail++;
                                {
                                    fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                                    //break;
                                }
                                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                                {
                                    //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                                    fprintf(stderr, "Worker [%s:%d] doesn't have disease %s\n", skonaki.items[i].my_ip.c_str(), skonaki.items[i].my_port, comms[1].c_str());
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
                            for (int i = 0; i < skonaki.size; i++)
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4])
                                    {
                                        uparxei = true;
                                    }
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
                            for (int i = 0; i < skonaki.size; i++) //gia kathe xwra
                            {
                                for (int j = 0; j < skonaki.items[i].my_countries.size; j++)
                                {
                                    if (skonaki.items[i].my_countries.items[j] == comms[4]) //an eisai auti pou psaxnw
                                    {
                                        //send to worker
                                        std::cerr << comms[4] << "\n";
                                        char *minima = new char[command.size() + 1];
                                        strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                        communicator.put(buf, minima);           //vazw to minima sto buf
                                        uint16_t portaki = skonaki.items[i].my_port;
                                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                        communicator.send(buf, connecting_fd);
                                        communicator.destroyBuffer(buf); //yeet
                                        poios = i;
                                        break;
                                    }
                                }
                            }
                            char *buf = communicator.createBuffer();
                            uint16_t portaki = skonaki.items[poios].my_port;
                            int listening_fd = Communication::create_listening_socket(portaki, 100);
                            communicator.recv(buf, listening_fd);
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
                                fprintf(stdout, "%s %s\n", comms[4].c_str(), buf);
                            }
                            communicator.destroyBuffer(buf);
                        }
                        else //den exw country ara send to all
                        {
                            for (int i = 0; i < skonaki.size; i++) //gia kathe worker
                            {
                                char *minima = new char[command.length() + 1];
                                strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                                char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                                communicator.put(buf, minima);           //vazw to minima sto buf
                                uint16_t portaki = skonaki.items[i].my_port;
                                int connecting_fd = Communication::create_connecting_socket(skonaki.items[i].my_ip.c_str(), portaki);
                                communicator.send(buf, connecting_fd);
                                communicator.destroyBuffer(buf); //yeet
                            }
                            //replies time
                            StringArray replies(posoi);
                            for (int i = 0; i < posoi; i++) //receive apo olous
                            {
                                char *buf = communicator.createBuffer();
                                uint16_t portaki = skonaki.items[i].my_port;
                                int listening_fd = Communication::create_listening_socket(portaki, 100);
                                communicator.recv(buf, listening_fd);
                                if (string(buf) == "ERR") //an lathos command, fail++;
                                {
                                    fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                                    //break;
                                }
                                else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                                {
                                    //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                                    fprintf(stderr, "Worker [%s:%d] doesn't have disease %s\n", skonaki.items[i].my_ip.c_str(), skonaki.items[i].my_port, comms[1].c_str());
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
                    /*
                    else
                    {
                        failed++;
                        std::cerr << "error\n";
                    }
                } //end while(1)
                */
                    // send res to client
                }
                pthread_mutex_unlock(&query_file_lock);
            }
            else if (token == 'W') // worker
            {
                int b;
                n = read(fd, &b, sizeof(b)); //posa diavasa g t b

                if (n <= 0)
                {
                    perror("b read failed");
                    break;
                }
                //mou steile to size b gia ton kukliko buffer
                pthreadargs->setSocketBufferSize(b);

                Communication com(b);

                // pthread_mutex_lock(&pthreadargs->cout_lock);
                // cout << "token = " << token << ", b = " << b << endl;
                // pthread_mutex_unlock(&pthreadargs->cout_lock);

                //alli 1 read gia posoi worker
                pthread_mutex_lock(&posoi_worker_lock);
                n = read(fd, &posoi, sizeof(posoi));
                //fprintf(stderr, "8a exw %d workers\n", posoi);
                pthread_mutex_unlock(&posoi_worker_lock);

                pthread_mutex_lock(&skonaki_lock);
                if (skonaki.capacity != posoi)
                {
                    skonaki.capacity = posoi; //thetw twra to capacity tou - den exei tpt mesa i exei idi
                }                             //else, don't do anything, exei idi tethei auti i timi apo allou worker to diavasma
                pthread_mutex_unlock(&skonaki_lock);

                //array apo posoi workers opou bazw o,ti mou dinei kathe buf: IP:PORTNUM.Xwra,Xwra,Xwra
                char *buff = com.createBuffer();
                com.recv(buff, fd);
                char *cstr = new char[strlen(buff)];
                strcpy(cstr, buff); //to cstr tha kanw tokenize
                char *saveptr;
                char *pch = strtok_r(cstr, ":", &saveptr); //strtok_r gia thread safe klisi
                std::string ip(pch);
                pch = strtok_r(NULL, ".", &saveptr);
                int port = atoi(pch);
                //fprintf(stderr, "mou eipes IP = %s, PORT =%d, kai xwres...", ip.c_str(), port);
                StringArray xwres(10); //kathe tetoio afora se 1 worker
                while (pch != NULL)
                {
                    pch = strtok_r(NULL, ",", &saveptr);
                    if (pch != NULL)
                    {
                        //fprintf(stderr, "exw %s\n", pch);
                        std::string tempPch(pch);
                        //fprintf(stderr, "exw %s\n", tempPch.c_str());
                        xwres.insert(tempPch);
                    }
                }
                PortCountries my_pc(port, xwres.size, ip);
                for (int i = 0; i < xwres.size; i++)
                {
                    //fprintf(stderr, "Paw na insert %s\n", xwres.items[i].c_str());
                    my_pc.my_countries.insert(xwres.items[i]);
                }
                pthread_mutex_lock(&skonaki_lock);
                if (skonaki.has_pair(my_pc.my_ip, my_pc.my_port) == -1) //an den uparxei idi to ip-port duet
                {
                    skonaki.insert(my_pc); // bale to PC auto
                }
                else //uparxei idi ara elegxw tis xwres -- kanonika den prepei na erthe edw pote
                {
                    int thesi = skonaki.has_pair(my_pc.my_ip, my_pc.my_port);
                    for (int j = 0; j < my_pc.my_countries.size; j++)
                    { //update tis xwres autou tou PC
                        skonaki.items[thesi].my_countries.insert(my_pc.my_countries.items[j]);
                    }
                }

                pthread_mutex_unlock(&skonaki_lock);

                // fprintf(stderr, "tha tupwsw ti exei mpei edw mesa tr!\n");
                // pthread_mutex_lock(&skonaki_lock);
                // skonaki.print();
                // pthread_mutex_unlock(&skonaki_lock);
                com.destroyBuffer(buff);

                // get summaries

                //fprintf(stderr, "Elava '%s'\n", buffSum);
                pthread_mutex_lock(&summ_file_lock);
                summ_file.open("sum_file.txt");
                while (true)
                {
                    char *buffSum = com.createBuffer();
                    com.recv(buffSum, fd);
                    if (string(buffSum) == "BYE") //elava to eidiko mhnuma oti that's a nono
                    {
                        break;
                    }
                    summ_file << buffSum << "\n"; //and den einai BYE valto mesa
                    com.destroyBuffer(buffSum);
                }
                summ_file.close();
                pthread_mutex_unlock(&summ_file_lock);
            }
            else
            {
                perror("unknown token");
                break;
            }
        }

        close(fd);
    }

    return 0;
}

int main(int argc, char const *argv[])
{

    uint16_t q = -1; //query port num
    uint16_t s = -1; //workers port num
    int w = -1;      //num of threads
    int b = -1;      //bufferSize

    for (int i = 0; i < argc; i++)
    {
        if (strcmp("-q", argv[i]) == 0)
        {
            q = (uint16_t)atoi(argv[i + 1]);
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
            s = (uint16_t)atoi(argv[i + 1]);
        }
    }
    if ((w < 0) || (b < 0) || (s < 0) || (q < 0))
    {
        perror("critical error: arguements");
        exit(EXIT_FAILURE);
    }

    int accept_client_fd = Communication::create_listening_socket(q, 100);

    int accept_worker_fd = Communication::create_listening_socket(s, 100);

    cout << "Accept clients at port " << q << " via FD: " << accept_client_fd << endl;

    cout << "Accept workers at port " << s << " via FD: " << accept_worker_fd << endl;

    pthread_t accept_children[2];

    ThreadArgs threadargs(q, s, w, b, accept_client_fd, accept_worker_fd, new Queue(b));

    pthread_create(&accept_children[0], NULL, accept_client, (void *)&threadargs);

    pthread_create(&accept_children[1], NULL, accept_workers, (void *)&threadargs);

    pthread_t service_children[w];

    for (int i = 0; i < w; i++)
    {
        pthread_create(&service_children[i], NULL, service, (void *)&threadargs);
    }

    cout << "Main thread waiting for children to exit ... " << endl;

    for (int i = 0; i < 2; i++)
    {
        pthread_join(accept_children[i], NULL);
    }

    for (int i = 0; i < w; i++)
    {
        pthread_join(service_children[i], NULL);
    }

    close(accept_client_fd);

    close(accept_worker_fd);

    return 0;
}