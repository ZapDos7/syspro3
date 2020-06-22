/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #3, Spring 2020	
 */
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "date.h"   //my date class
#include "ht.h"     //hash table - diki mas domi
#include "aht.h"    //"advanced" hash table
#include "tree.h"   //bst
#include "record.h" //record class
#include "bb.h"     //blocks and buckets
#include "heap.h"
#include "PairArray.h"
#include "PidArray.h"
#include "StringArray.h"
#include "../Communication.h"
#include "main_worker.h"
#include "TripleArray.h"
#include "Triplette.h"

/* Wait for all dead child processes 
void sigchld_handler (int sig) {
        while (waitpid(-1, NULL, WNOHANG) > 0);
}*/

void catchinterrupt(int signo)
{
    fclose(stdin);
}

// bool child_died = false;

// void birth_anew(int signo)
// {
//     //pethane ena paidi mou & prepei na to ksanaftiaksw
//     //alla edw den kserw poio ara prepei na mpw sti main
//     child_died = true;
//     // pid_t pid; //poio paidi?
//     // int status;
//     // while((pid = waitpid(-1, &status, WNOHANG)) > 0) ; //perimene mexri na paithanei auto to paidi
// }

int main(int argc, char const *argv[])
{
    //Anagnwsi params
    char in_dir[256]; //input directory
    int w = -1;       //number of workers
    int b = -1;       //bufferSize
    char ip[256];     //i server ip
    int sport = -1;   //to post tou server
    PairArray countries(300);

    for (int i = 0; i < argc; i++)
    {
        if (strcmp("-i", argv[i]) == 0)
        {
            strcpy(in_dir, argv[i + 1]);
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
            strcpy(ip, argv[i + 1]);
        }
        if (strcmp("-p", argv[i]) == 0)
        {
            sport = atoi(argv[i + 1]);
        }
    }
    if ((w < 0) || (b < 0) || (sport < 0))
    {
        printf("critical error: arguements");
        exit(-1);
    }

    //directory metablites
    DIR *dir;
    struct dirent *entry; //xwra
    Communication communicator(b);

    std::ifstream dataset; //edw 8a kanw open to dataset

    //anoigw to input directory pou exei mesa subfolders "Country"
    dir = opendir(in_dir);
    if (dir == NULL)
    {
        std::cerr << "error opening input directory\n";
        exit(-1);
    }
    //else
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            Pair p(entry->d_name); //eidallws einai country ara to vazoume sto zeugari (pid den exw akoma)
            countries.insert(p);   //add to pair stis xwres m
        }
    }

    PidArray processIds(w);
    StringArray names_in(w);
    StringArray names_out(w);
    TripleArray pid_in_out(w);

    // create pipes - onomatodosia vasei AGGREGATOR (out = pros worker, in = apo worker)
    for (int i = 0; i < w; i++)
    { //he normal, successful return value from mkfifo is 0 . In the case of an error, -1 is returned.
        string name_in = to_string(i) + "_in.fifo";

        unlink(name_in.c_str());
        int test = mkfifo(name_in.c_str(), 0644);
        if (test == -1)
        {
            perror(" Failed to make pipe_in");
            exit(1);
        }

        string name_out = to_string(i) + "_out.fifo";

        unlink(name_out.c_str());
        test = mkfifo(name_out.c_str(), 0644);
        if (test == -1)
        {
            perror(" Failed to make pipe_in");
            exit(1);
        }

        names_in.insert(name_in);
        names_out.insert(name_out);
    }

    // create children
    for (int i = 0; i < w; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            perror(" Failed to fork");
            exit(1);
        }
        if (child_pid == 0)
        { //paidi
            // call worker main

            child_pid = getpid();
            //cout << "child started with PID " << child_pid << endl;
            //kalw ti main tou paidiou
            return main_worker(in_dir, b, names_out.items[i].c_str(), names_in.items[i].c_str());
        }
        else //gonios
        {    //krataw ta pIDs twn paidiwn m
            processIds.insert(child_pid);
            Triplette t(child_pid);
            pid_in_out.insert(t);
        }
    }

    //apo ti stigmi pou exw ftiaksei paidia mporei kapoio na pethanei
    //ftiakse neo child an xathei kapoio
    // static struct sigaction act3;    //diavazw times deikti (an oldact --> gemizw times, not what i want here)
    // act3.sa_handler = birth_anew;    //i sinartisi - idio an evaza &catchinterrupt
    // sigfillset(&(act3.sa_mask));     //ola mask
    // sigaction(SIGCHLD, &act3, NULL); //ta 2 m shmata

    for (int i = 0; i < w; i++)
    {
        //xwria oi onomasies twn pipes
        //edw ANOIGEI ta PIPES o AGGR kai 8a ta ksanakleisei sto telos
        int out_fd = open(names_out.items[i].c_str(), O_WRONLY);
        pid_in_out.items[i].out = out_fd;
        int in_fd = open(names_in.items[i].c_str(), O_RDONLY);
        pid_in_out.items[i].in = in_fd;

        //cout << "aggregator opened pipes for worker: " << processIds.items[i] << endl;

        for (int j = 0; j < countries.size; j++)
        {
            if (j % w == i)
            { //round robin diaxwrismos twn xwrwn ana worker
                countries.items[j].pid = processIds.items[i];
                countries.items[j].out = out_fd;
                countries.items[j].in = in_fd;

                char *buf = communicator.createBuffer();
                communicator.put(buf, countries.items[j].country);
                communicator.send(buf, countries.items[j].out);
                communicator.destroyBuffer(buf);
            }
        }
        //aploikos elegxos gia liksi apostolis xwrwn se worker - epeidi den kseroume posa tha parei o ekastote worker
        char *buf = communicator.createBuffer();
        communicator.put(buf, "BYE");
        communicator.send(buf, out_fd);
        communicator.destroyBuffer(buf);
    }
    for (int i = 0; i < w; i++) //steile se olous tin IP tou server
    {
        char *buf = communicator.createBuffer();
        communicator.put(buf, ip);
        communicator.send(buf, pid_in_out.items[i].out);
        communicator.destroyBuffer(buf);
    }
    for (int i = 0; i < w; i++) //steile se olous posoi worker paizoun
    {
        char *buf = communicator.createBuffer();
        communicator.put(buf, w);
        communicator.send(buf, pid_in_out.items[i].out);
        communicator.destroyBuffer(buf);
    }
    for (int i = 0; i < w; i++) //steile se olous ton server port number
    {
        char *buf = communicator.createBuffer();
        communicator.put(buf, sport);
        communicator.send(buf, pid_in_out.items[i].out);
        communicator.destroyBuffer(buf);
    }

    fprintf(stderr, "Aggregator done!\n");

    long int total = 0;
    long int success = 0;
    long int failed = 0;

    //an o aggr lavei SIGINT/SIGQUIT (px Ctrl+C) tote paei kai ektelei tin catchinterrupt pou kleinei to while ara paei apo katw kanei shutdown ektos tis while kai eimaste ok
    //twra egkathistatai o handler
    static struct sigaction act;     //diavazw times deikti (an oldact --> gemizw times, not what i want here)
    act.sa_handler = catchinterrupt; //i sinartisi - idio an evaza &catchinterrupt
    sigfillset(&(act.sa_mask));      //ola mask
    sigaction(SIGINT, &act, NULL);   //ta 2 m shmata
    sigaction(SIGQUIT, &act, NULL);  // pou ekteloun tin function panw panw
    //mporousa na kanw kai signal set gia auta ta 2 giati exoun idia antimetwpisi

    //enimerosi tou master oti prepei na iksoun ola

    //shutdown
    //lave apo kathe worker ta succ/fail/total
    for (int i = 0; i < w; i++)
    {
        char *buf = communicator.createBuffer();
        communicator.put(buf, "/exit");
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
        strcpy(cstr2, results[i].c_str());
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

    // lambanw w * OK --> we done 'ere
    int okay_counter = w;
    for (int j = 0; j < w ; j++)
    {
        char * ok = communicator.createBuffer();
        communicator.recv(ok, pid_in_out.items[j].in);
        if (string(ok)=="OK")
        {
            okay_counter--;
        }
        communicator.destroyBuffer(ok);
    }
    if (okay_counter!=0)
    {
        fprintf(stderr, "Something is wrong with my kids...\n");
        exit(EXIT_FAILURE);
    }

    //kleinw pipes workers' ws AGGR
    for (int j = 0; j < countries.size; j++)
    {
        close(countries.items[j].out);
        close(countries.items[j].in);
    }

    // perimenw ta paidia m
    for (int i = 0; i < w; i++)
    {
        pid_t p = processIds.items[i];
        if (waitpid(p, NULL, 0) <= 0)
        {
            perror("wait ");
        }
        else
        {
            cout << "child finished with PID " << p << endl;
        }
    }

    // destroy pipes (elegxomeni diagrafi twn files autwn) ws AGGR
    for (int i = 0; i < w; i++)
    {
        unlink(names_in.items[i].c_str());
        unlink(names_out.items[i].c_str());
    }

    cout << "Aggregator finished " << endl;
    return 0;
}