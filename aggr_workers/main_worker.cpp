/*	
Ioanna Zapalidi, sdi1400044	
System Programming Project #2, Spring 2020	
 */
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h>      /* gethostbyaddr */

#include "date.h"   //my date class
#include "ht.h"     //hash table - diki mas domi
#include "aht.h"    //"advanced" hash table
#include "tree.h"   //bst
#include "record.h" //record class
#include "bb.h"     //blocks and buckets
#include "heap.h"
#include "quicksort.h"
#include "date_format.h"
#include "../Communication.h"
#include "StringArray.h"
#include "main_worker.h"
//#include "Summ.h"
#include "SummArray.h"

// bool refresh_scheduled = false;

// void catchinterrupt2(int signo)
// {
//     // refresh stin main gia na exw prosvasi se mi global vars
//     refresh_scheduled = true;
// }

// bool shutdown_scheduled = false;

// void catchinterrupt3(int signo)
// {
//     shutdown_scheduled = true;
// }

int main_worker(char *in_dir, int b, string name_out, string name_in)
{
    int child_pid = getpid();
    //std::cerr << "!!i am child " << child_pid << " kai elava to indir " << in_dir << "\n";
    Communication communicator(b);
    ht my_ht(50000);
    aht diseaseHT(50000, 1024);
    aht countryHT(50000, 1024);

    StringArray countries(300);

    int out_fd = open(name_out.c_str(), O_RDONLY); //edw diavazw ti m leei o AGGR
    int in_fd = open(name_in.c_str(), O_WRONLY);   //edw grafw ston AGGR

    //cout << "child opened pipes " << child_pid << endl;

    while (true)
    {
        char *buf = communicator.createBuffer();
        communicator.recv(buf, out_fd);
        if (string(buf) == "BYE")
        { //elava to eidiko mhnuma oti that's a nono
            break;
        }
        string country = buf;
        countries.insert(country);
        communicator.destroyBuffer(buf);
    }

    //fprintf(stderr, "o worker %d exei %d xwres\n", child_pid, countries.size);

    char *bufsip = communicator.createBuffer();
    communicator.recv(bufsip, out_fd);
    string serverip(bufsip);
    communicator.destroyBuffer(bufsip);

    char *bufpw = communicator.createBuffer();
    communicator.recv(bufpw, out_fd);
    int posoi_worker = atoi(bufpw);
    communicator.destroyBuffer(bufpw);

    char *bufsport = communicator.createBuffer();
    communicator.recv(bufsport, out_fd);
    uint16_t serverport = (uint16_t)atoi(bufsport);
    communicator.destroyBuffer(bufsport);

    fprintf(stderr, "worker %d has received serverIP: %s and serverPort: %d\n", child_pid, serverip.c_str(), serverport);

    uint16_t client_sin_port = 0;

    int listening_fd = Communication::create_listening_socket(client_sin_port);

    int connecting_fd = Communication::create_connecting_socket(serverip.c_str(), serverport);

    //cout << "client port: " << client_sin_port << endl;

    //eimai worker
    communicator.send('W', connecting_fd);
    //buffer size < aggr
    communicator.send(b, connecting_fd);
    //posoi eimaste
    communicator.send(posoi_worker, connecting_fd);

    std::string ip_port = "";
    ip_port.append(serverip);
    ip_port.append(":");
    ip_port.append(to_string(client_sin_port));
    ip_port.append(".");
    ip_port.append(countries.items[0]);
    for (int i = 1; i < countries.size; i++)
    {
        ip_port.append(",");
        ip_port.append(countries.items[i]);
    }
    char *thasteilw = new char[ip_port.length() + 1];
    strcpy(thasteilw, ip_port.c_str());
    communicator.send(thasteilw, connecting_fd);

    //sto connecting_fd stelnw summaries
    std::ifstream dataset; //edw 8a kanw open to dataset
    // //array apo Xwra/DD-MM-YYYY string, ti exw diavasei
    StringArray ta_arxeia_mou(300);
    for (int posa = 0; posa < countries.size; posa++)
    {
        std::string summary = "";

        int posa_arxeia = 0; //posa date arxeia exei mesa to folder gia mia xwra
        DIR *dir;
        struct dirent *entry; //xwra

        std::string monopati(in_dir);
        monopati.append("/");
        monopati.append(countries.items[posa]);

        //fprintf(stderr, "o worker %d anoigei to %s\n\n", child_pid, monopati.c_str());

        dir = opendir(monopati.c_str());
        if (dir == NULL)
        {
            std::cerr << "critical error opening input directory\n";
            exit(-1);
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue; //skip ., ..
                }
            }
            else //eimai se xwra/date
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue; //skip ., ..
                }
                posa_arxeia++;
                std::string tmp(entry->d_name);
                ta_arxeia_mou.insert(tmp);
            }
        }
        closedir(dir);

        //     //fprintf(stderr, "1o closedir apo %d, posa arxeia: %d \n", child_pid, posa_arxeia);

        std::string *date_file_names = new string[posa_arxeia];

        //ksananoigw na ta valw sto array
        int posa_date_arxeia = 0; //posa evala sto array
        dir = opendir(monopati.c_str());
        if (dir == NULL)
        {
            std::cerr << "critical error opening input directory\n";
            exit(-1);
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue; //skip ., ..
                }
            }
            else //eimai se xwra/date
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue; //skip ., ..
                }
                date_file_names[posa_date_arxeia] = entry->d_name;
                posa_date_arxeia++;
            }
        }
        closedir(dir);

        quickSort(date_file_names, 0, posa_date_arxeia - 1);

        for (int akak = 0; akak < posa_date_arxeia; akak++)
        {
            summary.append(date_file_names[akak]);
            summary.append("\n"); //date
            summary.append(countries.items[posa]);
            summary.append("\n"); //country
            //fprintf(stderr, "\t%s\n", summary.c_str());
            SummArray summaries(1000);
            std::string path = monopati;
            path.append("/");
            path.append(date_file_names[akak]);
            //fprintf(stderr, "o %d paei na anoiksei to %s\n", child_pid, path.c_str());
            dataset.open(path);

            if (!dataset.is_open())
            {
                std::cerr << "critical error - cannot open dataset\n";
                exit(-1);
            }
            else if (dataset.is_open())
            {
                //fprintf(stderr, "paw ma diavasw getline apo %d\n", child_pid);
                std::string line;

                while (std::getline(dataset, line)) //diavazei olo to this date --> we gotta do it for all files in this folder
                {
                    std::string wannabe = "";
                    std::string help[6]; //boithitiki domi me ta tokens tou line pou 8a ftiaksw swsta
                    const char *c_string = line.c_str();
                    int counter = 0;
                    char *buffer = strdup(c_string);
                    char *token = strtok(buffer, " ");

                    while (token)
                    {
                        //std::cerr << token << '\t';
                        help[counter] = token;
                        token = strtok(NULL, " ");
                        counter++;
                    }
                    wannabe += help[0]; //wannabe="ID"
                    wannabe += " ";
                    wannabe += help[2]; //wannabe="ID Fname"
                    wannabe += " ";
                    wannabe += help[3]; //wannabe="ID Fname Lname"
                    wannabe += " ";
                    wannabe += help[4]; //wannabe="ID Fname Lname Disease"
                    wannabe += " ";
                    wannabe += countries.items[posa]; //wannabe="ID Fname Lname Disease Country"
                    wannabe += " ";
                    wannabe += help[5]; //wannabe="ID Fname Lname Disease Country Age"
                    wannabe += " ";
                    //fprintf(stderr, "o %d elegxei to record %s", child_pid, wannabe.c_str());
                    if (help[1] == "ENTER") //exoume eisodo
                    {
                        wannabe += date_file_names[akak];
                        //fprintf(stderr, "o %d elegxei to record %s\n", child_pid, wannabe.c_str());
                        record temp_r(wannabe);                  //temp record gia insert
                        record *elegxos = my_ht.insert(&temp_r); //edw ginetai kai elegxos gia unique IDs
                        if (elegxos == NULL)
                        {
                            std::cerr << "den evala to " << elegxos->get_id() << "\n";
                            continue;
                            //break; //sto piazza eipw8ike oti an brethei kapoio ID duplicate, na proxwrame stis entoles & na mhn sunexizoun ta insertions.
                        }
                        else
                        {
                            summaries.insert_rec(elegxos);
                            //fprintf(stderr, "o %d evale to %s\n", child_pid, elegxos->get_id().c_str());
                            diseaseHT.ainsert(elegxos, false);
                            countryHT.ainsert(elegxos, true);
                        }
                    }
                    else if (help[1] == "EXIT")
                    {
                        //fprintf(stderr, "o %d elegxei to record %s gia exit\n", child_pid, wannabe.c_str());
                        ht_item *h = my_ht.search(help[0]); //bres an uparxei to record me auto to ID
                        if (h == NULL)
                        {
                            //fprintf(stderr, "o %d den brike ton %s\n", child_pid, help[0].c_str());
                            continue; //oops this record doesn't exist, akuro, bye
                        }
                        else
                        //if (h!=NULL)
                        {
                            date d2(date_file_names[akak]);         //d2 = ti paw na valw
                            date *d1 = h->rec->get_exitDatePtr();   //to uparxon exit date
                            bool magkas = d1->set;                  //ama eixa prin set=true ara eixa idi exit date, true, else false
                            date *din = h->rec->get_entryDatePtr(); //to entry date
                            if (isLater(d2, *din) == 1)             //pas na mou baleis kati pou den einai later tou entry date m
                            {
                                std::cerr << "error\n";
                            }
                            else //valid basei entry date
                            {
                                h->rec->set_exitD(d2.get_date_as_string()); //twra exei exit date
                                if (magkas == false)                        //prin den imoun set ara update counters:
                                {
                                    block *blokaki1 = diseaseHT.search(help[4]);
                                    if (blokaki1 != NULL)
                                    {
                                        blokaki1->update_c_in(false);
                                    }

                                    block *blokaki2 = countryHT.search(countries.items[posa]);
                                    if (blokaki2 != NULL)
                                    {
                                        blokaki2->update_c_in(false);
                                    }
                                }
                                //eidallws oi metrites den allazoun!
                            }
                        }
                    }
                    //continue;
                }
            }
            dataset.close();
            //fprintf(stderr, "worker %d going to %d apo %d\n", child_pid, akak, posa_date_arxeia);
            //send summary:
            for (int f = 0; f < summaries.size; f++)
            {
                summary.append(summaries.items[f].diseaseName); //onoma astheneias
                summary.append("\n");
                summary.append("Age range 0-20 years: ");
                summary.append(to_string(summaries.items[f].metr1));
                summary.append(" cases\n");
                summary.append("Age range 21-40 years: ");
                summary.append(to_string(summaries.items[f].metr2));
                summary.append(" cases\n");
                summary.append("Age range 41-60 years: ");
                summary.append(to_string(summaries.items[f].metr3));
                summary.append(" cases\n");
                summary.append("Age range 60+ years: ");
                summary.append(to_string(summaries.items[f].metr4));
                summary.append(" cases\n");
                summary.append("\n");
            }
            //fprintf(stderr, "\t%s\n", summary.c_str());
        }
        char *buf = communicator.createBuffer();
        communicator.put(buf, summary);
        communicator.send(buf, connecting_fd);
        communicator.destroyBuffer(buf);
        //closedir(dir);
        //fprintf(stderr, "telos apo %d gia xwra= %s\n", child_pid, countries.items[posa].c_str());
    }

    //esteila kai summaries
    char *bufsum = communicator.createBuffer();
    communicator.put(bufsum, "BYE");
    communicator.send(bufsum, connecting_fd);
    communicator.destroyBuffer(bufsum);

    char * ok = communicator.createBuffer();
    communicator.put(ok, "OK");
    communicator.send(ok, in_fd);
    communicator.destroyBuffer(ok);

    fprintf(stderr, "Worker %d ready\n", child_pid);

    //sto listening_fd dexomai Queries

    long int total = 0;
    long int success = 0;
    long int failed = 0;
    //sundesi me server

    while (true)
    {
        //accept listening_fd
        fprintf(stderr, "Waiting for command...\n");
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);
        int new_socket;
        if ((new_socket = accept(listening_fd, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("accept-worker");
            break;
        }
        //else all these:
        char *buf = communicator.createBuffer();
        communicator.recv(buf, listening_fd);
        std::string com(buf); //com is the command as std::string
        communicator.destroyBuffer(buf);
        fprintf(stderr, "command: %s\n", com.c_str());
        char *cstr = new char[com.length() + 1]; //auto 8a kanw tokenize
        strcpy(cstr, com.c_str());               //copy as string to line sto cstr
        char *pch;
        const char delim[2] = " "; // \0
        pch = strtok(cstr, delim);
        std::string comms[6]; //i entoli me ta perissotera orismata einai h topk age ranges (6)
        int counter = 0;
        comms[0] = pch;
        total++;
        fprintf(stderr, "o worker %d elave %s\n", child_pid, comms[0].c_str());
        //check first word to match with command, check entire command if correct
        if (comms[0] == "/exit")
        {
            //workers -> log files
            ofstream logfile;
            std::string onomaarxeiou = "log_file.";
            onomaarxeiou += to_string(child_pid);
            logfile.open(onomaarxeiou);
            for (int i = 0; i < countries.size; i++) //grafw poies einai oi xwres m
            {
                logfile << countries.items[i] << "\n";
            }
            logfile << "TOTAL: " << total << "\n";     //posa erwthmata mou irthan
            logfile << "SUCCESS: " << success << "\n"; //posa success
            logfile << "FAIL: " << failed << "\n";     //posa fail
            logfile.close();

            std::string results = "";
            results.append(to_string(total));
            results.append(",");
            results.append(to_string(success));
            results.append(",");
            results.append(to_string(failed));
            char *buf = communicator.createBuffer();
            communicator.put(buf, results);
            //communicator.send(buf, in_fd);
            communicator.send(buf, connecting_fd); //ta stelnw ston server

            return 0;
        }
        else if (comms[0] == "/diseaseFrequency") //8. /diseaseFrequency virusName date1 date2 [country]
        {
            while (pch != NULL)
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if (counter == 5) //mou edwses onoma xwras
            {
                std::string cntrName = comms[4];
                if (countries.has(cntrName) == false) //den exw auti ti xwra
                {
                    //eidiki episimansi oti mhn me laveis uposin AGGR
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                }
                else //auti einai i xwra m ara let's go
                {
                    std::string virusName = comms[1];
                    std::string wannabedate1 = comms[2];
                    std::string wannabedate2 = comms[3];
                    if ((date_format(wannabedate1) == false) || (date_format(wannabedate2) == false))
                    {
                        char *buf = communicator.createBuffer();
                        communicator.put(buf, "ERR");
                        //communicator.send(buf, in_fd);
                        communicator.send(buf, connecting_fd); //ta stelnw ston server
                        communicator.destroyBuffer(buf);
                        failed++;
                        //break;
                    }
                    //else
                    date d1(wannabedate1);
                    date d2(wannabedate2);
                    if (isLater(d1, d2) == -1) //an d1>d2, epistrefei -1
                    {
                        char *buf = communicator.createBuffer();
                        communicator.put(buf, "ERR");
                        //communicator.send(buf, in_fd);
                        communicator.send(buf, connecting_fd); //ta stelnw ston server
                        communicator.destroyBuffer(buf);
                        failed++;
                        //break;
                    }
                    //else ok
                    block *apantisi = diseaseHT.search(virusName);
                    if (apantisi == NULL) //mou zitas na brw kati pou den exw sti vasi m
                    {
                        failed++;
                        char *buf = communicator.createBuffer();
                        communicator.put(buf, "IDK");
                        //communicator.send(buf, in_fd);
                        communicator.send(buf, connecting_fd); //ta stelnw ston server
                        communicator.destroyBuffer(buf);
                    }
                    else
                    {
                        long int apantisi_num = apantisi->statsC(d1, d2, cntrName);
                        std::string reply = to_string(apantisi_num);
                        char *buf = communicator.createBuffer();
                        communicator.put(buf, reply.c_str());
                        //communicator.send(buf, in_fd);
                        communicator.send(buf, connecting_fd); //ta stelnw ston server
                        communicator.destroyBuffer(buf);
                        success++;
                    }
                }
            }
            else if (counter == 4) // den exei xwra ara stelnw ta panta
            {
                std::string virusName = comms[1];
                std::string wannabedate1 = comms[2];
                std::string wannabedate2 = comms[3];
                if ((date_format(wannabedate1) == false) || (date_format(wannabedate2) == false))
                {
                    //std::cerr << "error1\n";
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "ERR");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                    break;
                }
                //else
                date d1(wannabedate1);
                date d2(wannabedate2);
                if (isLater(d1, d2) == -1) //an d1>d2, epistrefei -1
                {
                    //std::cerr << "error2\n";
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "ERR");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                    break;
                }
                //else ok
                block *apantisi = diseaseHT.search(virusName);
                if (apantisi == NULL) //mou zitas na brw kati pou den exw sti vasi m
                {
                    //std::cerr << "error5\n";
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                }
                else
                {
                    std::string reply = to_string(apantisi->stats(d1, d2));
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, reply.c_str());
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
            }
            else //mou edwses alla m nt alla orismata
            {
                //std::cerr << "error6\n";
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                failed++;
            }
        }
        else if (comms[0] == "/topk-AgeRanges") // /topk-AgeRanges k country disease d1 d2
        {
            //thelei polles alages akoma se polla epipeda domwn
            while (pch != NULL)
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if (counter != 6)
            {
                //fprintf(stderr, "error\n");
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
            }
            int k = atoi(comms[1].c_str());
            std::string countryName = comms[2];
            std::string diseaseName = comms[3];
            block *b = countryHT.search(countryName);
            if (b == NULL)
            {
                //std::cerr << "error\n"; //else print
                char *buf = communicator.createBuffer();
                communicator.put(buf, "IDK");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                //std::cerr << countryName << " 0\n";
            } //age ranges: 0-20, 21-40, 41-60, 60+
            else
            {
                std::string wannabedate1 = comms[4];
                std::string wannabedate2 = comms[5];
                if ((date_format(wannabedate1) == false) || (date_format(wannabedate2) == false))
                {
                    //std::cerr << "error\n";
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "ERR");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                    //break;
                }
                //else dates are ok
                date d1(wannabedate1);
                date d2(wannabedate2);
                if (isLater(d1, d2) == -1) //an d1>d2, epistrefei -1
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "ERR");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                    //std::cerr << "error\n";
                    //break;
                } //else dates are ok ok ara kaloume
                //return ta age ranges
                std::string apantisi = b->top_k_age_ranges(k, d1, d2, diseaseName);
                char *buf = communicator.createBuffer();
                communicator.put(buf, apantisi.c_str());
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                success++;
            }
        }
        else if (comms[0] == "/searchPatientRecord")
        {
            //  /searchPatientRecord recordID
            while (pch != NULL) //kovw tin entoli sta parts tis
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            if (counter != 2)
            {
                //std::cerr << "error1\n";
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                failed++;
            }
            else
            {
                ht_item *anazitisis = my_ht.search(comms[counter - 1]);
                if (anazitisis == NULL)
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    failed++;
                }
                else
                {
                    std::string apantisi = "";
                    apantisi += anazitisis->rec->get_id();
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_fname();
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_lname();
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_disease();
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_country();
                    apantisi += " ";
                    apantisi += to_string(anazitisis->rec->get_age());
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_entryDate().get_date_as_string();
                    apantisi += " ";
                    apantisi += anazitisis->rec->get_exitDate().get_date_as_string();
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, apantisi);
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    success++;
                }
            }
        }
        else if (comms[0] == "/numPatientAdmissions") //  /numPatientAdmissions disease d1 d2 [country]
        {
            while (pch != NULL)
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            std::string dis(comms[1]);
            std::string wannabedate1 = comms[2];
            std::string wannabedate2 = comms[3];
            if ((date_format(wannabedate1) == false) || (date_format(wannabedate2) == false))
            {
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                failed++;
                //break;
            }
            //else
            date d1(wannabedate1);
            date d2(wannabedate2);
            if (isLater(d1, d2) == -1) //an d1>d2, epistrefei -1
            {
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                failed++;
                //break;
            }
            //else ok
            if ((counter > 5) || (counter < 4))
            {
                failed++;
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
            }
            else if (counter == 4) //oxi basei country
            {
                block *blockPtr = diseaseHT.search(dis);
                if (blockPtr == NULL)
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
                else
                {
                    //gia kathe xwra m
                    std::string reply = "";
                    for (int me = 0; me < countries.size; me++)
                    {
                        std::string onoma_xwras;
                        onoma_xwras = countries.items[me];
                        long int apantisi = blockPtr->statsC(d1, d2, onoma_xwras);
                        reply.append(onoma_xwras);
                        reply.append(" ");
                        reply.append(to_string(apantisi));
                        reply.append("\n");
                    }
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, reply.c_str());
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
            }
            else if (counter == 5) //nai basei country
            {
                std::string countryName(comms[4]);
                block *blockPtr = diseaseHT.search(dis);
                if (blockPtr == NULL)
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                }
                else
                {
                    long int apantisi = blockPtr->statsC(d1, d2, countryName);
                    std::string reply = to_string(apantisi);
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, reply.c_str());
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
            }
        }
        else if (comms[0] == "/numPatientDischarges") //  /numPatientDischarges disease d1 d2 [country]
        {
            while (pch != NULL)
            {
                comms[counter] = pch;
                counter++;
                pch = strtok(NULL, delim);
            }
            std::string dis(comms[1]);
            std::string wannabedate1 = comms[2];
            std::string wannabedate2 = comms[3];
            if ((date_format(wannabedate1) == false) || (date_format(wannabedate2) == false))
            {
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                failed++;
                //break;
            }
            //else
            date d1(wannabedate1);
            date d2(wannabedate2);
            if (isLater(d1, d2) == -1) //an d1>d2, epistrefei -1
            {
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
                failed++;
                //break;
            }
            //else ok
            if ((counter > 5) || (counter < 4))
            {
                failed++;
                char *buf = communicator.createBuffer();
                communicator.put(buf, "ERR");
                //communicator.send(buf, in_fd);
                communicator.send(buf, connecting_fd); //ta stelnw ston server
                communicator.destroyBuffer(buf);
            }
            else if (counter == 4) //oxi basei country
            {
                block *blockPtr = diseaseHT.search(dis);
                if (blockPtr == NULL)
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
                else
                {
                    //gia kathe xwra m
                    std::string reply = "";
                    for (int me = 0; me < countries.size; me++)
                    {
                        std::string onoma_xwras;
                        onoma_xwras = countries.items[me];
                        long int apantisi = blockPtr->statsExitC(d1, d2, onoma_xwras);
                        reply.append(onoma_xwras);
                        reply.append(" ");
                        reply.append(to_string(apantisi));
                        reply.append("\n");
                    }
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, reply.c_str());
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
            }
            else if (counter == 5) //nai basei country
            {
                std::string countryName(comms[4]);
                block *blockPtr = diseaseHT.search(dis);
                if (blockPtr == NULL)
                {
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, "IDK");
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    failed++;
                }
                else
                {
                    long int apantisi = blockPtr->statsExitC(d1, d2, countryName);
                    std::string reply = to_string(apantisi);
                    char *buf = communicator.createBuffer();
                    communicator.put(buf, reply.c_str());
                    //communicator.send(buf, in_fd);
                    communicator.send(buf, connecting_fd); //ta stelnw ston server
                    communicator.destroyBuffer(buf);
                    success++;
                }
            }
        }
        else
        {
            //std::cerr << "Unknown Command!\n"; //doesn't exit the program, gives the user another chance to type properly this time.
            std::cerr << "error\n";
            failed++;
        }
    } //end while(1)

    //shutdown
    // ofstream logfile;
    // std::string onomaarxeiou = "log_file.";
    // onomaarxeiou += to_string(child_pid);
    // logfile.open(onomaarxeiou);
    // for (int i = 0; i < countries.size; i++) //grafw poies einai oi xwres m
    // {
    //     logfile << countries.items[i] << "\n";
    // }
    // logfile << "TOTAL: " << total << "\n";     //posa erwthmata mou irthan
    // logfile << "SUCCESS: " << success << "\n"; //posa success
    // logfile << "FAIL: " << failed << "\n";     //posa fail
    // logfile.close();
    // std::string results = "";
    // results.append(to_string(total));
    // results.append(",");
    // results.append(to_string(success));
    // results.append(",");
    // results.append(to_string(failed));
    // char *buf = communicator.createBuffer();
    // communicator.put(buf, results);
    //communicator.send(buf, in_fd);
    //communicator.send(buf, connecting_fd); //ta stelnw ston server
    close(connecting_fd);
    return 0;
}