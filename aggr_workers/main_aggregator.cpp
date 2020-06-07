/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #2, Spring 2020	
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
#include "Communication.h"
#include "main_worker.h"
#include "TripleArray.h"
#include "Triplette.h"

void catchinterrupt(int signo)
{
    fclose(stdin);
}

bool child_died = false;

void birth_anew(int signo)
{
    //pethane ena paidi mou & prepei na to ksanaftiaksw
    //alla edw den kserw poio ara prepei na mpw sti main
    child_died = true;
    // pid_t pid; //poio paidi?
    // int status;
    // while((pid = waitpid(-1, &status, WNOHANG)) > 0) ; //perimene mexri na paithanei auto to paidi
}

int main(int argc, char const *argv[])
{
    //Anagnwsi params
    char in_dir[256]; //input directory
    int w = -1;       //number of workers
    int b = -1;       //bufferSize
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
    }
    if ((w < 0) || (b < 0))
    {
        std::cerr << "error\n";
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
    static struct sigaction act3;    //diavazw times deikti (an oldact --> gemizw times, not what i want here)
    act3.sa_handler = birth_anew;    //i sinartisi - idio an evaza &catchinterrupt
    sigfillset(&(act3.sa_mask));     //ola mask
    sigaction(SIGCHLD, &act3, NULL); //ta 2 m shmata

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

    //elegxos
    //names_in.print();
    //names_out.print();
    //countries.print_lc();
    //processIds.print();
    //pid_in_out.print();
    //return 0;
    /*
    for (int i = 0; i < w; i++) {
        char *buf = communicator.createBuffer();
        communicator.put(buf, "hi\n");
        communicator.send(buf, pid_in_out.items[i].out);
        communicator.destroyBuffer(buf);
    }

    for (int i = 0; i < w; i++) {
        char *buf = communicator.createBuffer();
        communicator.recv(buf, pid_in_out.items[i].in);

        fprintf(stderr, "Elava apo to worker %d to minima: '%s' \n", i, buf);

        if (string(buf) != "yo") {
            fprintf(stderr, "Lathos sti xeirapsia me to worker %d to minima: %s \n", i, buf);
            exit(1);
        }
        communicator.destroyBuffer(buf);
    }
*/

    ofstream summ_file;
    std::string onomaarxeiousum = "sum_file.txt";
    summ_file.open(onomaarxeiousum);

    //lipsi summaries
    for (int i = 0; i < w; i++)
    {
        while (true)
        {
            char *buf = communicator.createBuffer();
            communicator.recv(buf, pid_in_out.items[i].in);
            //fprintf(stderr, "Elava apo to worker %d to minima: '%ld' \n", i, sizeof(buf));
            if (string(buf) == "BYE")
            { //elava to eidiko mhnuma oti that's a nono
                break;
            }
            //and en einai BYE valto mesa
            summ_file << buf << "\n";
            communicator.destroyBuffer(buf);
        }
    }
    summ_file.close();
    fprintf(stderr, "Gonios ready!\n");
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

    //fprintf(stderr, "Stelnw sto paidi %d ena SIGINT na gelasoume\n", pid_in_out.items[0].pid);
    //kill(pid_in_out.items[0].pid,SIGINT);

    //commands
    std::string com; //command
    //anagnosi tis apo ton aggr (pipe)

    while (printf("?") && std::getline(std::cin, com))
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

    //stelnw sigkill sta paidia
    for (int i = 0; i < w; i++)
    {
        kill(pid_in_out.items[i].pid, SIGKILL);
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
    //ta da
    return 0;
}