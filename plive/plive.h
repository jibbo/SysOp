/*
    Anno accademico     2012/2013
    Corso studio        Informatica
    Progetto            #1 Plive
    Componenti:
        Zen Roberto             151737
        Giovanni De Francesco   152080
        Perantoni Nicola        151709
*/

#ifndef PLIVE_H
#define PLIVE_H        

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>
#include <syslog.h>

//number of process show
int n=10; 
//refresh interval
int seconds=1; 

//struct che rappresenta un processo 
//contine solo i primi 13 dati contenuti
//in /proc/pid/stat perche' sono i primi
//utili.
typedef struct process_t
{
    int pid;
    char name[256];
    char status;
    int ppid;
    float utime;
}PROC;

//struct che rappresenta un processo 
//contine solo i dati vermente utili 
typedef struct mini_process_t
{
    int pid;
    char name[256];
    int ppid;
    float cpu;
}MINI_PROC;

//funzione che viene chiamato quando 
//qualcosa di brutto acade al 
//nostro programma (e.g. Segmentation fault)
void errors(int sig);

//stampa i processi interessati
//il numero e' definito da 
void stmpNProc(MINI_PROC *proc,int len);

//Function that set the number of
//procces that will be displayed
//return -1 in case of error
int checkflag(int argc, char **argv);

//this is the function that is 
//called by a thread to watch
//user's input
void *updateVariables();

//return the number of all the process running
//avoiding useless folder 
int  numberOfProcess();

//rimuove le parentesi dal nome dei processi
void removeParentesis(char *in, char * out);

//Riempe l'array di processi che deve guardare
//ritorna il numero di cartelle in proc effettivamente utilizzate
int listOfProcess(PROC *proc, int len) ;

//ritorna per quanto tempo e' stata utilizzata la cpu
float getTotalTime();

//funzione per confrontare due
//processi in base al pid
int cmpPID (const void * a, const void * b);

//funzione per confrontare due
//mini processi in base alla cpu utilizzata
int cmpCPU (const void * a, const void * b);

//funzione principale
int topTimes(PROC* before,PROC* after, MINI_PROC* out,int len1,int len2);

//elimina il contenuto di old
//e copia tutto il contenuto di last
//in old
void copyProc(PROC* old,PROC* last,int len2);

#endif