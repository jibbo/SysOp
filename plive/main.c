#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>

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
	int pgrp;
	int session;
	int tty;
	int tpgid;
	unsigned flags;
	unsigned long minfaults;
	unsigned long majfaults;
	unsigned long utime;
	unsigned long stime;
	unsigned long ctime;
}PROC;

//struct che rappresenta un processo 
//contine solo i primi 13 dati contenuti
//in /proc/pid/stat perche' sono i primi
//utili.
typedef struct mini_process_t
{
    int pid;
    char* name;
    int ppid;
    unsigned long cpu;
}MINI_PROC;


//stampa l'intero array di processi
void stmpOfArray(PROC *proc,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
      printf("pid: %d\t%s\t\n",proc[i].pid,proc[i].name);
    }
}

//stampa i processi interessati
//il numero e' definito da 
void stmpProc(MINI_PROC *proc)
{
    int i;
    for(i=0; i<n; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
        printw("pid: %d  \tppid: %d  \tname: %s  \tcpu:%lu\n",proc[i].pid,proc[i].ppid,
            proc[i].name,proc[i].cpu);
    }
    refresh();
}

//Function that set the number of
//procces that will be displayed
int checkflag(int argc, char **argv) 
{
    int index=0;
    opterr = 0;
    int c = getopt (argc, argv, "n:");
    switch (c)
    {
        case 'n':
        //convert the value passed from the command line to an integer
        n = atoi(optarg);
        //check if the argument passed is valid
        if(n<=0) {
            fprintf (stderr, "Invalid argument passed to `-n' option.\n",optopt);
            return -1;
        }
        return 1;
        case '?':
        if (optopt == 'n')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
            fprintf (stderr,
               "Unknown option character `\\x%x'.\n",
               optopt);
        return -1;
        //case for nothing passed as option
        default:
        return 1;
    }
}

void *updateVariables()
{
    while(true)
    {
        char c = getch();
        if(c>='0' && c<='9')
            seconds=atoi(&c);
        else if(c=='q'){
            endwin();
            exit(EXIT_SUCCESS);
        }
    }

}

//return the number of all the process running
//avoiding useless folder 
int  numberOfProcess()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc/");
    int nt=0;
    while ((dir = readdir(d)) != NULL)
    {
      if(dir->d_name[0]>='0' && dir->d_name[0]<='9')
        nt++;
    }
    closedir(d);
    return nt;
}

//Riempe l'array di processi che deve guardare
//ritorna il numero di cartelle in proc che non devono essere
int listOfProcess(PROC *proc,int len) 
{
    int i;
    DIR *d = opendir("/proc/");
    FILE *f;
    struct dirent *dir;
    int nt=0;
    for(i=0; i<len; i++)
    {
        dir= readdir(d);
        char path[300];
        strcpy(path,"/proc/");
        if(dir->d_name[0]>='0' && dir->d_name[0]<='9')
        {
            strcat(path,dir->d_name);
            strcat(path,"/stat");
            f= fopen(path,"r");
            if(f){
                //leggo il contenuto di /proc/pid/stat
                fscanf(f,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu",
                    &proc[i].pid,proc[i].name,&proc[i].status,&proc[i].ppid,
                    &proc[i].pgrp,&proc[i].session,&proc[i].tty,&proc[i].tpgid,
                    &proc[i].flags,&proc[i].minfaults,&proc[i].majfaults,
                    &proc[i].utime,&proc[i].stime, &proc[i].ctime);
                //removes parenthesis from the name of a process
                char *name = proc[i].name;
                int j=0;
                for(j=1;j<strlen(name)-1; j++)
                    proc[i].name[j-1]=name[j];
                proc[i].name[j-1]='\0';
                close(f);
            }
        }
        else i--; //this is to avoid useless folder.
    }   
    closedir(d);
}

void getTotalTime(int *out){
   FILE *f= fopen("/proc/stat","r"); 
   if(f)
   {
        int user=0,nice=0,sys=0;
        char cpu[10];
        fscanf(f,"%s %d %d %d",cpu,&user,&nice,&sys);
        *out=user+nice+sys;
        close(f);
   }
}

int cmpfunc (const void * a, const void * b)
{
    MINI_PROC *l =(MINI_PROC *)a;
    MINI_PROC *r =(MINI_PROC *)b;
    if(l->cpu > r->cpu)
        return 1;
    if(l->cpu < r->cpu)
        return -1;
    return 0;
}
void *topTimes(PROC* before,PROC* after, MINI_PROC* out,int len,
    int timeTotalBefore,int timeTotalAfter)
{
    MINI_PROC tmp[len];
    int i=0;
    for(i=0;i<len;i++)
    {
        tmp[i].pid=after[i].pid;
        tmp[i].ppid=after[i].ppid;
        tmp[i].name=after[i].name;
        float timeTotalAfter=after[i].stime+after[i].utime;
        float timeTotalBefore=before[i].stime+before[i].utime;
        if(after[i].status=='S')
            tmp[i].cpu=0;
        else
            tmp[i].cpu=100.0*(((after[i].stime+after[i].utime)-
                (before[i].stime+before[i].utime))/
                (timeTotalAfter-timeTotalBefore));
    }
    qsort(tmp, len, sizeof(MINI_PROC), cmpfunc);
    int j=0;
    for(i=n-1;i>=0;i--)
    {
        out[j].pid=tmp[i].pid;
        out[j].ppid=tmp[i].ppid;
        out[j].name=tmp[i].name;
        j++;
    }
}

void copyProc(PROC* old,PROC* last,int len)
{
    int i=0;
    for(i=0;i<len;i++)
    {
        old[i]=last[i];
    }
}

int main(int argc, char **argv) 
{
    if(checkflag(argc,argv)!=-1) 
    {
         int np=numberOfProcess();
         pthread_t look;
         PROC old_proc[np];
         listOfProcess(old_proc,np);
         //stmpOfArray(proc,np);
         unsigned long oldTimes[np];
         initscr();
         pthread_create(&look, NULL, updateVariables, NULL);
         int timeTotalBefore=0;
         getTotalTime(&timeTotalBefore);
         while(1)
         {
            sleep(seconds);
            PROC proc[np];
            listOfProcess(proc,np);
            MINI_PROC out[n];
            int timeTotalAfter=0;
            getTotalTime(&timeTotalAfter);
            topTimes(old_proc,proc,out,np,timeTotalBefore,timeTotalAfter);
            copyProc(old_proc,proc,np);
            clear();
            // printw("TotalTime spent: %d\n",timeTotalAfter-timeTotalBefore);
            // refresh();
            timeTotalBefore=timeTotalAfter;
            stmpProc(out);
            
        }
        endwin();
    }
    return 0;
}
