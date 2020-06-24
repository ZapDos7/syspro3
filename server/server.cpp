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

int posoi = 0;                     //plithos workers pou xeirizetai o server & kathe tou thread - mas to lene oi workers tin 1i fora
pthread_mutex_t posoi_worker_lock; //plithos workers mutex

//static pthread_barrier_t barrier_dis_freq; //otan den dinetai country orisma, otan pesei to barrier (exw POSOI answers) tote kanw sum (vasei mutexes)

bool finish = false;
void shutdown(int signo) //kleisimo
{
    finish = true;
    //clean up, shut down
    pthread_mutex_destroy(&skonaki_lock);
    pthread_mutex_destroy(&summ_file_lock);
    pthread_mutex_destroy(&posoi_worker_lock);
}

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
        int fd = pthreadargs->queue->obtain(); //pairnw aitima pou elava apo tin queue m

        if (fd <= 0) //ekmek
        {
            break;
        }
        else
        {
            int n = read(fd, &token, sizeof(token));

            if (n <= 0) //ekmek
            {
                perror("token read failed");
                break;
            }

            if (token == 'C')
            { // client
                int b = pthreadargs->getSocketBufferSize();
                int check = write(fd, &b, sizeof(b));
                if (check < 0)
                {
                    perror("write b");
                    exit(EXIT_FAILURE);
                }

                Communication communicator(b); //tha exei idi timi epeidi prwta m lene oi Worker o,ti theloun k meta egw apantaw
                //read command
                char *buf = communicator.createBuffer();
                communicator.recv(buf, fd);
                string command = buf;
                communicator.destroyBuffer(buf);

                if (command[0] != '/')
                {
                    command = "/" + command; //trexei kai me kai xwris / stis entoles
                }

                fprintf(stderr, "\n---------------\nelava to command '%s'\n", command.c_str());

                char *cstr = new char[command.length() + 1]; //auto 8a kanw tokenize
                strcpy(cstr, command.c_str());               //copy as string to line sto cstr
                char *pch;
                const char delim[2] = " ";
                pch = strtok(cstr, delim);
                std::string comms[8]; //i entoli me ta perissotera orismata einai h insertPatientRecord me d2
                int counter = 0;
                comms[0] = pch;
                //check first word to match with command, check entire command if correct
                if (comms[0] == "/diseaseFrequency") //8. /diseaseFrequency virusName date1 date2 [country]
                {
                    pthread_mutex_t result_lock;
                    pthread_mutex_init(&result_lock, NULL);
                    int sum = 0;
                    //pthread_barrier_init(&barrier_dis_freq, NULL, skonaki.getSize());
                    pthread_mutex_t rr_lock;
                    pthread_mutex_init(&rr_lock, NULL);
                    int replies_received = skonaki.getSize();
                    while (pch != NULL) //kovw tin entoli sta parts tis
                    {
                        comms[counter] = pch;
                        counter++;
                        pch = strtok(NULL, delim);
                    }
                    if ((counter > 5) || (counter < 4))
                    {
                        fprintf(stderr, "wrong amount of arguements\n");
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                    }
                    else if (counter == 5) //exw xwra ara paw se auti ti xwra --> se auto to process na kanw tin entoli
                    {
                        //find se poio worker einai auti i xwra
                        //uparxei genika auti i xwra?
                        bool uparxei = false;
                        pthread_mutex_lock(&skonaki_lock);
                        int poios = skonaki.has_country(comms[4]);
                        pthread_mutex_unlock(&skonaki_lock);
                        if (poios != -1)
                        {
                            uparxei = true;
                            //fprintf(stderr, "xwra %s sti thesi %d\n", comms[4].c_str(), poios);
                        }
                        if (uparxei == false)
                        {
                            fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                        }
                        char *minima = new char[command.length() + 1];
                        strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, minima);           //vazw to minima sto buf
                        pthread_mutex_lock(&skonaki_lock);
                        uint16_t portaki = skonaki.items[poios]->my_port;
                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[poios]->my_ip.c_str(), portaki);
                        communicator.send(buf, connecting_fd);
                        fprintf(stderr, "esteila to command '%s'\n------------------------------------\n", buf);
                        //close(connecting_fd);
                        pthread_mutex_unlock(&skonaki_lock);
                        communicator.destroyBuffer(buf); //yeet
                        bool found = false;

                        char *bufR = communicator.createBuffer();
                        //fprintf(stderr, "gonna recv now\n");
                        communicator.recv(bufR, connecting_fd);
                        //communicator.recv(buf, connecting_fd);
                        if (strlen(bufR) == 0)
                        {
                            fprintf(stderr, "den elava tpt\n");
                            exit(EXIT_FAILURE);
                        }
                        //else
                        fprintf(stderr, "reply: '%s'\n", bufR);
                        std::string replyStr(bufR);
                        communicator.destroyBuffer(bufR);

                        close(connecting_fd);

                        //////////////////////////////////////////////////
                        if (replyStr == "ERR") //an lathos command, fail++;
                        {
                            fprintf(stderr, "Lathos command wre \n");
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "ERR");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        else if (replyStr == "IDK") //opoios epistrefei "IDK", skip
                        {
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "IDK");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        else //mou dwses apantisi ok - 8a einai enas arithmos mono
                        {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                            found = true;
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, replyStr.c_str());
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        if (found == false) //teleiwsame kai kanenas den ton brike
                        {
                            fprintf(stdout, "This disease doesn't exist in the database.\n");
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "ERR");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                    }
                    else //if (counter==4) //ara den exw xwra ara ti zitaw apo olous
                    {

                        int conn_fds[skonaki.getSize()];
                        for (int i = 0; i < skonaki.getSize(); i++) //gia kathe worker
                        {
                            char *minima = new char[command.length() + 1];
                            strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                            char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                            communicator.put(buf, minima);           //vazw to minima sto buf
                            pthread_mutex_lock(&skonaki_lock);
                            uint16_t portaki = skonaki.items[i]->my_port;
                            int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
                            communicator.send(buf, connecting_fd);
                            conn_fds[i] = connecting_fd;
                            fprintf(stderr, "\n-----------\nesteila to command '%s' (%d)\n", buf, i);
                            pthread_mutex_unlock(&skonaki_lock);
                            communicator.destroyBuffer(buf); //yeet
                            //close(connecting_fd);
                        }

                        bool found = false;

                        for (int i = 0; i < skonaki.getSize(); i++) //send se olous
                        {
                            char *buf = communicator.createBuffer();
                            communicator.recv(buf, conn_fds[i]);
                            close(conn_fds[i]);
                            if (strlen(buf) == 0)
                            {
                                fprintf(stderr, "den elava tpt\n");
                                break;
                            }

                            fprintf(stderr, "elava '%s'\n", buf);

                            if (string(buf) == "ERR") //an lathos command, fail++;
                            {
                                fprintf(stderr, "error\n");
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                                //found == false akoma
                                // char *bufferr = communicator.createBuffer();
                                // communicator.put(bufferr, "ERR");
                                // communicator.send(bufferr, fd);
                                // communicator.destroyBuffer(bufferr);
                            }
                            else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                            {
                                fprintf(stderr, "unknown\n");
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                                //found == false akoma
                                //char *bufferr = communicator.createBuffer();
                                // communicator.put(bufferr, "IDK");
                                // communicator.send(bufferr, fd);
                                // communicator.destroyBuffer(bufferr);
                                //fprintf(stderr, "worker %d has not this record\n", pid_in_out.items[i].pid);
                            }
                            else //mou dwses apantisi ok
                            {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                                found = true;
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                                pthread_mutex_lock(&result_lock);
                                sum += atoi(buf);
                                fprintf(stderr, "afou kanw prosthesi+, sum = %d\n--------\n", sum);
                                pthread_mutex_unlock(&result_lock);
                            }

                            communicator.destroyBuffer(buf);
                            //metrites
                            if (replies_received == 0)
                            {
                                if (found == false) //teleiwsame kai kanenas den ton brike
                                {
                                    //pthread_barrier_wait(&barrier_dis_freq);
                                    fprintf(stdout, "This disease doesn't exist in the database.\n");

                                    char *bufferr = communicator.createBuffer();
                                    communicator.put(bufferr, "ERR");
                                    communicator.send(bufferr, fd);
                                    communicator.destroyBuffer(bufferr);

                                    pthread_mutex_destroy(&result_lock);
                                }
                                else
                                {
                                    //pthread_barrier_wait(&barrier_dis_freq);
                                    fprintf(stderr, "replies received = %d\n\n", replies_received);
                                    fprintf(stderr, "final reply = %d\n", sum);

                                    char *bufferr = communicator.createBuffer();
                                    communicator.put(bufferr, sum);
                                    communicator.send(bufferr, fd);
                                    communicator.destroyBuffer(bufferr);

                                    pthread_mutex_destroy(&result_lock);
                                }
                            }
                        }
                    }
                    //pthread_barrier_destroy(&barrier_dis_freq);
                }
                else if (comms[0] == "/topk-AgeRanges")
                { // /topk-AgeRanges k country disease d1 d2
                    while (pch != NULL)
                    {
                        comms[counter] = pch;
                        counter++;
                        pch = strtok(NULL, delim);
                    }
                    if (counter != 6)
                    {
                        std::cerr << "Wrong amount of agruements\n";
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                        break;
                    }
                    //is k anamesa se 1 kai 4?
                    int k = atoi(comms[1].c_str());
                    if ((k < 1) || (k > 4))
                    {
                        fprintf(stderr, "Wrong value for k.\n");
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                        break;
                    }
                    //uparxei genika auti i xwra?
                    bool uparxei = false;
                    pthread_mutex_lock(&skonaki_lock);
                    int poios = skonaki.has_country(comms[2]);
                    pthread_mutex_unlock(&skonaki_lock);
                    if (poios != -1)
                    {
                        uparxei = true;
                    }
                    if (uparxei == false)
                    {
                        fprintf(stderr, "Country %s not in database.\n", comms[2].c_str());
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                        break;
                    }
                    char *minima = new char[command.length() + 1];
                    strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                    char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                    communicator.put(buf, minima);           //vazw to minima sto buf
                    pthread_mutex_lock(&skonaki_lock);
                    uint16_t portaki = skonaki.items[poios]->my_port;
                    int connecting_fd = Communication::create_connecting_socket(skonaki.items[poios]->my_ip.c_str(), portaki);
                    communicator.send(buf, connecting_fd);
                    fprintf(stderr, "esteila to command '%s'\n------------------------------------\n", buf);
                    pthread_mutex_unlock(&skonaki_lock);
                    communicator.destroyBuffer(buf); //yeet

                    char *bufR = communicator.createBuffer();
                    //fprintf(stderr, "gonna recv now\n");
                    communicator.recv(bufR, connecting_fd);
                    //communicator.recv(buf, connecting_fd);
                    if (strlen(bufR) == 0)
                    {
                        fprintf(stderr, "den elava tpt\n");
                        exit(EXIT_FAILURE);
                    }
                    //else
                    fprintf(stderr, "reply: '%s'\n", bufR);
                    std::string replyStr(bufR);
                    communicator.destroyBuffer(bufR);

                    close(connecting_fd);

                    if (replyStr == "ERR") //an lathos command, fail++;
                    {
                        fprintf(stderr, "Lathos command wre \n");
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                    }
                    else if (replyStr == "IDK") //opoios epistrefei "IDK", skip
                    {
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "IDK");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                    }
                    else //mou dwses apantisi ok - 8a einai enas arithmos mono
                    {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, replyStr.c_str());
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                    }
                }
                else if (comms[0] == "/searchPatientRecord")
                { // /searchPatientRecord id
                    pthread_mutex_t rr_lock;
                    pthread_mutex_init(&rr_lock, NULL);
                    int replies_received = skonaki.getSize();
                    int conn_fds[skonaki.getSize()];
                    for (int i = 0; i < skonaki.getSize(); i++) //send se olous
                    {
                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, command.c_str());  //vazw oli tin entoli sto buf
                        pthread_mutex_lock(&skonaki_lock);
                        uint16_t portaki = skonaki.items[i]->my_port;
                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
                        //fprintf(stderr, "connecting fd == pou lew tin command se worker: %d\n", connecting_fd);
                        communicator.send(buf, connecting_fd);
                        conn_fds[i] = connecting_fd;
                        pthread_mutex_unlock(&skonaki_lock);
                        communicator.destroyBuffer(buf); //yeet
                        //fprintf(stderr, "esteila to command se kathe worker!\n");
                    }
                    bool found = false;
                    for (int i = 0; i < posoi; i++) //send se olous
                    {
                        std::string reply = "";
                        char *buf = communicator.createBuffer();
                        communicator.recv(buf, conn_fds[i]);
                        close(conn_fds[i]);
                        if (strlen(buf) == 0)
                        {
                            fprintf(stderr, "den elava tpt\n");
                            break;
                        }

                        fprintf(stderr, "elava '%s'\n", buf);
                        if (string(buf) == "ERR") //an lathos command, fail++;
                        {
                            fprintf(stderr, "error \n");
                            pthread_mutex_lock(&rr_lock);
                            replies_received--;
                            pthread_mutex_unlock(&rr_lock);
                            // char *bufferr = communicator.createBuffer();
                            // communicator.put(bufferr, "ERR");
                            // communicator.send(bufferr, fd);
                            // communicator.destroyBuffer(bufferr);
                        }
                        else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                        {
                            fprintf(stderr, "unkown \n");
                            pthread_mutex_lock(&rr_lock);
                            replies_received--;
                            pthread_mutex_unlock(&rr_lock);
                            // char *bufferr = communicator.createBuffer();
                            // communicator.put(bufferr, "IDK");
                            // communicator.send(bufferr, fd);
                            // communicator.destroyBuffer(bufferr);
                            //fprintf(stderr, "worker %d has not this record\n", pid_in_out.items[i].pid);
                        }
                        else //mou dwses apantisi ok
                        {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                            found = true;
                            pthread_mutex_lock(&rr_lock);
                            replies_received--;
                            pthread_mutex_unlock(&rr_lock);
                            //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                            fprintf(stdout, "apantisi %d: '%s'\n", i, buf);
                            reply = buf;
                            //reply.copy(buf, strlen(buf), 0);
                            fprintf(stderr, "reply as string: %s\n\n", reply.c_str());
                        }

                        communicator.destroyBuffer(buf);
                        //metrites
                        if (replies_received == 0)
                        {
                            if (found == false) //teleiwsame kai kanenas den ton brike
                            {
                                fprintf(stdout, "This record doesn't exist in the database.\n");
                                char *bufferr = communicator.createBuffer();
                                communicator.put(bufferr, "ERR");
                                communicator.send(bufferr, fd);
                                communicator.destroyBuffer(bufferr);
                            }
                            //else, print to apantisi: apo seira 521
                            else
                            {
                                char *bufferr = communicator.createBuffer();
                                char *apantisi = new char[reply.length() + 1];
                                strcpy(apantisi, reply.c_str());
                                communicator.put(bufferr, apantisi);
                                communicator.send(bufferr, fd);
                                communicator.destroyBuffer(bufferr);
                            }
                        }
                    }
                }
                else if (comms[0] == "/numPatientAdmissions")
                { //  /numPatientAdmissions disease d1 d2 [country]
                    pthread_mutex_t rr_lock;
                    pthread_mutex_init(&rr_lock, NULL);
                    int replies_received = skonaki.getSize();
                    while (pch != NULL) //kovw tin entoli sta parts tis
                    {
                        comms[counter] = pch;
                        counter++;
                        pch = strtok(NULL, delim);
                    }
                    if ((counter != 5) && (counter != 4))
                    {
                        fprintf(stderr, "wrong amount of arguements\n");
                        char *bufferr = communicator.createBuffer();
                        communicator.put(bufferr, "ERR");
                        communicator.send(bufferr, fd);
                        communicator.destroyBuffer(bufferr);
                    }
                    //uparxei genika auti i xwra?
                    if (counter == 5) //exw country
                    {
                        //uparxei?
                        bool uparxei = false;
                        pthread_mutex_lock(&skonaki_lock);
                        int poios = skonaki.has_country(comms[4]);
                        pthread_mutex_unlock(&skonaki_lock);
                        //fprintf(stderr, "xwra %s sti thesi %d\n", comms[4].c_str(), poios);
                        if (poios != -1)
                        {
                            uparxei = true;
                        }
                        if (uparxei == false)
                        {
                            fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "ERR");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                            break;
                        }
                        //send to worker
                        char *minima = new char[command.length() + 1];
                        strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                        communicator.put(buf, minima);           //vazw to minima sto buf
                        pthread_mutex_lock(&skonaki_lock);
                        uint16_t portaki = skonaki.items[poios]->my_port;
                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[poios]->my_ip.c_str(), portaki);
                        communicator.send(buf, connecting_fd);
                        fprintf(stderr, "esteila to command '%s'\n------------------------------------\n", buf);
                        //close(connecting_fd);
                        pthread_mutex_unlock(&skonaki_lock);
                        communicator.destroyBuffer(buf); //yeet

                        bool found = false;

                        char *bufR = communicator.createBuffer();
                        //fprintf(stderr, "gonna recv now\n");
                        communicator.recv(bufR, connecting_fd);
                        //communicator.recv(buf, connecting_fd);
                        if (strlen(bufR) == 0)
                        {
                            fprintf(stderr, "den elava tpt\n");
                            exit(EXIT_FAILURE);
                        }
                        //else
                        fprintf(stderr, "reply: '%s'\n", bufR);
                        std::string replyStr(bufR);
                        communicator.destroyBuffer(bufR);

                        close(connecting_fd);

                        //////////////////////////////////////////////////
                        if (replyStr == "ERR") //an lathos command, fail++;
                        {
                            fprintf(stderr, "Lathos command wre \n");
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "ERR");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        else if (replyStr == "IDK") //opoios epistrefei "IDK", skip
                        {
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "IDK");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        else //mou dwses apantisi ok - 8a einai enas arithmos mono
                        {    //an kapoios ton vrei, print else print oti den uparxei autos o patient
                            found = true;
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, replyStr.c_str());
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                        if (found == false) //teleiwsame kai kanenas den ton brike
                        {
                            fprintf(stdout, "This disease doesn't exist in the database.\n");
                            char *bufferr = communicator.createBuffer();
                            communicator.put(bufferr, "ERR");
                            communicator.send(bufferr, fd);
                            communicator.destroyBuffer(bufferr);
                        }
                    }
                    else //den exw country ara send to all
                    {
                        int conn_fds[skonaki.getSize()];
                        for (int i = 0; i < skonaki.getSize(); i++) //gia kathe worker
                        {
                            char *minima = new char[command.length() + 1];
                            strcpy(minima, command.c_str());         //eidallws nmzei oti to com.c_str() einai const char *
                            char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                            communicator.put(buf, minima);           //vazw to minima sto buf
                            pthread_mutex_lock(&skonaki_lock);
                            uint16_t portaki = skonaki.items[i]->my_port;
                            int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
                            communicator.send(buf, connecting_fd);
                            conn_fds[i] = connecting_fd;
                            fprintf(stderr, "\n-----------\nesteila to command '%s' (%d)\n", buf, i);
                            pthread_mutex_unlock(&skonaki_lock);
                            communicator.destroyBuffer(buf); //yeet
                        }
                        //replies time
                        //std::string replies[w];
                        StringArray replies(posoi);
                        bool found = false;
                        for (int i = 0; i < posoi; i++) //receive apo olous
                        {
                            char *buf = communicator.createBuffer();
                            communicator.recv(buf, conn_fds[i]);
                            close(conn_fds[i]);
                            if (strlen(buf) == 0)
                            {
                                fprintf(stderr, "den elava tpt\n");
                                break;
                            }

                            fprintf(stderr, "elava '%s'\n", buf);
                            if (string(buf) == "ERR") //an lathos command, fail++;
                            {
                                fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                                //found == false akoma
                                // char *bufferr = communicator.createBuffer();
                                // communicator.put(bufferr, "ERR");
                                // communicator.send(bufferr, fd);
                                // communicator.destroyBuffer(bufferr);
                            }
                            else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                            {
                                //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                                fprintf(stderr, "Worker [%s:%d] doesn't have disease %s\n", skonaki.items[i]->my_ip.c_str(), skonaki.items[i]->my_port, comms[1].c_str());
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                                //found == false akoma
                                // char *bufferr = communicator.createBuffer();
                                // communicator.put(bufferr, "ERR");
                                // communicator.send(bufferr, fd);
                                // communicator.destroyBuffer(bufferr);
                            }
                            else //mou dwses apantisi ok
                            {
                                //buf = "Xwra Num\nXwra Num\n etc"
                                std::string temp_reply(buf);
                                replies.insert(temp_reply);
                                found = true;
                                pthread_mutex_lock(&rr_lock);
                                replies_received--;
                                pthread_mutex_unlock(&rr_lock);
                            }
                            communicator.destroyBuffer(buf);
                        }
                        //tupwnw
                        if (replies_received == 0)
                        {
                            if (found == false) //teleiwsame kai kanenas den ton brike
                            {
                                //pthread_barrier_wait(&barrier_dis_freq);
                                fprintf(stdout, "This disease doesn't exist in the database.\n");

                                char *bufferr = communicator.createBuffer();
                                communicator.put(bufferr, "ERR");
                                communicator.send(bufferr, fd);
                                communicator.destroyBuffer(bufferr);
                            }
                            else
                            {
                                //pthread_barrier_wait(&barrier_dis_freq);
                                fprintf(stderr, "replies received = %d\n\n", replies_received);
                                std::string final_reply = "";
                                for (int aa = 0; aa < replies.getSize(); aa++)
                                {
                                    //fprintf(stderr, "reply %d = '%s'\n", aa, replies.items[aa].c_str());
                                    final_reply.append(replies.items[aa]);
                                }
                                fprintf(stderr, "final reply: %s\n", final_reply.c_str());
                                
                                char *bufferr = communicator.createBuffer();
                                char * frCstr = new char[final_reply.length()+1];
                                strcpy(frCstr, final_reply.c_str());
                                communicator.put(bufferr, frCstr);
                                communicator.send(bufferr, fd);
                                communicator.destroyBuffer(bufferr);
                            }
                        }
                    }
                }
                else if (comms[0] == "/numPatientDischarges")
                {                       //  /numPatientDischarges disease d1 d2 [country]
                    while (pch != NULL) //kovw tin entoli sta parts tis
                    {
                        comms[counter] = pch;
                        counter++;
                        pch = strtok(NULL, delim);
                    }
                    if ((counter != 5) && (counter != 4))
                    {
                        std::cerr << "error edw edw edw\n";
                    }
                    //uparxei genika auti i xwra?
                    if (counter == 5)
                    { //exw country
                        //uparxei?
                        //                        bool uparxei = false;
                        //                        pthread_mutex_lock(&skonaki_lock);
                        //                        int poios = skonaki.has_country(comms[4]);
                        //                        pthread_mutex_unlock(&skonaki_lock);
                        //                        if (poios != -1) {
                        //                            uparxei = true;
                        //                        }
                        //
                        //                        if (uparxei == false) {
                        //                            failed++;
                        //                            fprintf(stderr, "Country %s not in database.\n", comms[4].c_str());
                        //                            break;
                        //                        }
                        //                        //find se poio worker einai auti i xwra
                        //
                        //                        //send to worker
                        //                        std::cerr << comms[4] << "\n";
                        //                        char *minima = new char[command.size() + 1];
                        //                        strcpy(minima, command.c_str()); //eidallws nmzei oti to com.c_str() einai const char *
                        //                        char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                        //                        communicator.put(buf, minima); //vazw to minima sto buf
                        //                        pthread_mutex_lock(&skonaki_lock);
                        //                        uint16_t portaki = skonaki.items[poios]->my_port;
                        //                        int connecting_fd = Communication::create_connecting_socket(skonaki.items[poios]->my_ip.c_str(), portaki);
                        //                        communicator.send(buf, connecting_fd);
                        //                        pthread_mutex_unlock(&skonaki_lock);
                        //                        //communicator.destroyBuffer(buf); //yeet
                        //
                        //                        //char *buf = communicator.createBuffer();
                        //                        //uint16_t portaki = skonaki.items[poios]->my_port;
                        //                        //int listening_fd = Communication::create_listening_socket(portaki, 100);
                        //                        //communicator.recv(buf, listening_fd);
                        //                        communicator.recv(buf, pthreadargs->s);
                        //                        //communicator.recv(buf, pid_in_out.items[poios].in);
                        //                        if (string(buf) == "ERR") //an lathos command, fail++;
                        //                        {
                        //                            fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                        //                            break;
                        //                        } else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                        //                        {
                        //                            fprintf(stdout, "No relevant information exists in database.\n");
                        //                        } else //mou dwses apantisi ok
                        //                        {
                        //                            //fprintf(stderr, "worker %d has this record\n", pid_in_out.items[i].pid);
                        //                            fprintf(stdout, "%s %s\n", comms[4].c_str(), buf);
                        //                        }
                        //                        communicator.destroyBuffer(buf);
                    }
                    else
                    { //den exw country ara send to all
                        //                        for (int i = 0; i < skonaki.getSize(); i++) //gia kathe worker
                        //                        {
                        //                            char *minima = new char[command.length() + 1];
                        //                            strcpy(minima, command.c_str()); //eidallws nmzei oti to com.c_str() einai const char *
                        //                            char *buf = communicator.createBuffer(); //ftiaxnw ton buffer gia na steilw to mnm
                        //                            communicator.put(buf, minima); //vazw to minima sto buf
                        //                            pthread_mutex_lock(&skonaki_lock);
                        //                            uint16_t portaki = skonaki.items[i]->my_port;
                        //                            int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
                        //                            communicator.send(buf, connecting_fd);
                        //                            pthread_mutex_unlock(&skonaki_lock);
                        //                            communicator.destroyBuffer(buf); //yeet
                        //                        }
                        //                        //replies time
                        //                        StringArray replies(posoi);
                        //                        for (int i = 0; i < posoi; i++) //receive apo olous
                        //                        {
                        //                            char *buf = communicator.createBuffer();
                        //                            //uint16_t portaki = skonaki.items[i]->my_port;
                        //                            //int listening_fd = Communication::create_listening_socket(portaki, 100);
                        //                            //communicator.recv(buf, listening_fd);
                        //                            communicator.recv(buf, pthreadargs->s);
                        //                            if (string(buf) == "ERR") //an lathos command, fail++;
                        //                            {
                        //                                fprintf(stderr, "Error - unknown or wrongly typed command.\n");
                        //                                //break;
                        //                            } else if (string(buf) == "IDK") //opoios epistrefei "IDK", skip
                        //                            {
                        //                                //auto shmainei oti i astheneia leipei apo to skonaki tou worker ara pame ston epomeno worker
                        //                                fprintf(stderr, "Worker [%s:%d] doesn't have disease %s\n", skonaki.items[i]->my_ip.c_str(), skonaki.items[i]->my_port, comms[1].c_str());
                        //                                //break;
                        //                            } else //mou dwses apantisi ok
                        //                            {
                        //                                //buf = "Xwra Num\nXwra Num\n etc"
                        //                                std::string temp_reply(buf);
                        //                                replies.insert(temp_reply);
                        //                            }
                        //                            communicator.destroyBuffer(buf);
                        //                        }
                        //                        //tupwnw
                        //                        for (int i = 0; i < replies.getSize(); i++) {
                        //                            fprintf(stdout, "%s", replies.items[i].c_str());
                        //                        }
                        //                        fprintf(stdout, "\n");
                    }
                }
                // else if (comms[0] == "/exit")
                // {
                //     for (int i = 0; i < skonaki.getSize(); i++) //gia kathe worker
                //     {
                //         pthread_mutex_lock(&skonaki_lock);
                //         uint16_t portaki = skonaki.items[i]->my_port;
                //         int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
                //         communicator.send('E', connecting_fd);
                //         pthread_mutex_unlock(&skonaki_lock);
                //         communicator.destroyBuffer(buf); //yeet
                //         close(connecting_fd);
                //         //clean up?
                //     }
                // }
                else
                {
                    fprintf(stderr, "Error - unknown command\n");
                    char *bufferr = communicator.createBuffer();
                    communicator.put(bufferr, "ERR");
                    communicator.send(bufferr, fd);
                    communicator.destroyBuffer(bufferr);
                    exit(EXIT_FAILURE);
                }
            }
            else if (token == 'W')
            { // worker
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

                //array apo posoi workers opou bazw o,ti mou dinei kathe buf: IP:PORTNUM.Xwra,Xwra,Xwra
                char *buff = com.createBuffer();
                com.recv(buff, fd);

                char *cstr = new char[strlen(buff) + 1];
                strcpy(cstr, buff); //to cstr tha kanw tokenize

                char *saveptr = NULL;

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

                PortCountries *my_pc = new PortCountries(port, xwres.getSize(), ip);
                for (int i = 0; i < xwres.getSize(); i++)
                {
                    //fprintf(stderr, "Paw na insert %s\n", xwres.items[i].c_str());
                    my_pc->my_countries.insert(xwres.items[i]);
                }

                pthread_mutex_lock(&skonaki_lock);
                if (skonaki.has_pair(my_pc->my_ip, my_pc->my_port) == -1) //an den uparxei idi to ip-port duet
                {
                    //fprintf(stderr, "paw na insert:\t");
                    //my_pc->print_pc();
                    skonaki.insert(my_pc); // bale to PC auto
                }
                else //uparxei idi ara elegxw tis xwres -- kanonika tha erthei edw an pethanei kapoios worker
                {
                    int thesi = skonaki.has_pair(my_pc->my_ip, my_pc->my_port);
                    for (int j = 0; j < my_pc->my_countries.getSize(); j++)
                    { //update tis xwres autou tou PC
                        skonaki.items[thesi]->my_countries.insert(my_pc->my_countries.items[j]);
                    }
                }

                //fprintf(stderr, "skonaki capacity: %d and size: %d\ntha tupwsw ti exei mpei edw mesa tr!\n", skonaki.capacity, skonaki.size);
                //skonaki.print();
                pthread_mutex_unlock(&skonaki_lock);
                com.destroyBuffer(buff);

                // -------------------------------------------------
                // get summaries
                // -------------------------------------------------
                //fprintf(stderr, "Elava '%s'\n", buffSum);
                //                pthread_mutex_lock(&summ_file_lock);
                //                summ_file.open("sum_file.txt", ios::app);
                //                while (true) {
                //                    char *buffSum = com.createBuffer();
                //                    com.recv(buffSum, fd);
                //                    if (string(buffSum) == "BYE") //elava to eidiko mhnuma oti that's a nono
                //                    {
                //                        break;
                //                    }
                //                    summ_file << buffSum << "\n"; //and den einai BYE valto mesa
                //                    com.destroyBuffer(buffSum);
                //                }
                //                summ_file.close();
                //                pthread_mutex_unlock(&summ_file_lock);

                // --------------------------------------------
                pthread_mutex_lock(&summ_file_lock);
                cout << "------------------------------------ \n";
                skonaki.print();
                cout << "------------------------------------ \n";
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
    if (w > 50)
    {
        fprintf(stderr, "Number of threads should be no more than 50\n");
        exit(0);
    }

    //init mutexes?
    pthread_mutex_init(&skonaki_lock, NULL);
    pthread_mutex_init(&summ_file_lock, NULL);
    pthread_mutex_init(&posoi_worker_lock, NULL);

    int accept_client_fd = Communication::create_listening_socket(q);

    int accept_worker_fd = Communication::create_listening_socket(s);

    cout << "Accept clients at port " << q << " via FD: " << accept_client_fd << endl;

    cout << "Accept workers at port " << s << " via FD: " << accept_worker_fd << endl;

    pthread_t accept_children[2];

    ThreadArgs threadargs(q, s, w, b, accept_client_fd, accept_worker_fd, new Queue(b));

    pthread_create(&accept_children[0], NULL, accept_client, (void *)&threadargs);

    pthread_create(&accept_children[1], NULL, accept_workers, (void *)&threadargs);

    pthread_t service_children[w]; //var length

    for (int i = 0; i < w; i++)
    {
        pthread_create(&service_children[i], NULL, service, (void *)&threadargs);
    }

    fprintf(stderr, "Main thread waiting for children to exit ... \n");

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

    //piazza : termatizei me CTRL + C --> sigint signal handle
    static struct sigaction act;
    act.sa_handler = shutdown;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);

    // if (finish == true)
    // {
    //     Communication communicator(b);
    //     for (int i = 0; i < w; i++)
    //     {
    //         char *buf = communicator.createBuffer();
    //         std::string token = "/exit";
    //         char *msg = new char[token.length() + 1];
    //         communicator.put(buf, msg);
    //         pthread_mutex_lock(&skonaki_lock);
    //         uint16_t portaki = skonaki.items[i]->my_port;
    //         int connecting_fd = Communication::create_connecting_socket(skonaki.items[i]->my_ip.c_str(), portaki);
    //         communicator.send(buf, connecting_fd);
    //         pthread_mutex_unlock(&skonaki_lock);
    //         communicator.destroyBuffer(buf);
    //         close(connecting_fd);
    //     }
    // }

    return 0;
}