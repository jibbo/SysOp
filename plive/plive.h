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

//numero dei processi da visualizzare di default
int n=10; 

//intervallo di tempo usato per
//aggiornare la visualizzazione
//di default
int seconds=1; 

//struct che rappresenta un processo 
//contenuto in /proc/pid/stat
//dove vengono salvati solo i dati utili al calcolo
typedef struct process_t
{
    int pid;
    char name[256];
    char status;
    int ppid;
    float utime;
}PROC;

//struct che rappresenta un processo 
//contenuto in /proc/pid/stat
//dove vengono salvati solo i dati utili alla visualizzazione
typedef struct mini_process_t
{
    int pid;
    char name[256];
    int ppid;
    float cpu;
}MINI_PROC;

//funzione che viene chiamata quando 
//qualcosa di brutto accade al 
//nostro programma (e.g. Segmentation fault)
void errors(int sig);

//stampa i processi interessati
void stmpNProc(MINI_PROC *proc,int len);

//funzione che fa un parsing delle opzioni dati al programma
//quindi in posta il numero di processi da visualizzare
//ritorna -1 nel caso di errori
int checkflag(int argc, char **argv);

//funzione che viene usata da 
//un thread per gestire l'input dell' utente.
void *updateVariables();

//ritorna il numero di processi attivi sul sistema
//evitando le cartelle che non hanno un PID
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

//questa funzione calcola la quantita' di cpu utilizzata da ogni processo
//e genera l'array di processi da mostrare all'utente 
int topTimes(PROC* before,PROC* after, MINI_PROC* out,int lenBefore,int lenAfter,float timeTotalBefore,float timeTotalAfter);

//rimpiazza la vecchia lettura
//con l'ultima
void copyProc(PROC* before,PROC* last,int lenAfter);

#endif