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

void getTotalTime(unsigned long *out){
   FILE *f= fopen("/proc/stat","r"); 
   if(f)
   {
        int user=0,nice=0,sys=0;
        char cpu[10];
        fscanf(f,"%s %d %d %d",cpu,&user,&nice,&sys);
        //*out=user+nice+sys;
        out=sys;
        close(f);
   }
}

int cmpfunc (const void * a, const void * b)
{
    return ((*(MINI_PROC*)a).cpu - (*(MINI_PROC*)b).cpu);
}
void *topTimes(PROC* before,PROC* after, MINI_PROC* out,int len,int len2,
    unsigned long *timeTotalBefore,unsigned long *timeTotalAfter)
{
    MINI_PROC tmp[len];
    int i=0;
    int j=0;
    for(i=0;i<len;i++)
    {
        bool end=false;
        while(before[i].pid!=after[j].pid && !end)
        {
            if(j>=len2-1)
                end=true;
            j++;
        }
        if(end)
            continue;
        tmp[i].pid=after[j].pid;
        tmp[i].ppid=after[j].ppid;
        tmp[i].name=after[j].name;
        /*if(after[j].status=='S')
            tmp[i].cpu=0;
        else*/
            // tmp[i].cpu=100.0*(((after[j].stime+after[j].utime)-
            //     (before[i].stime+before[i].utime))/
            //     (*timeTotalAfter-*timeTotalBefore));
            tmp[i].cpu=100.0*(after[j].stime-before[i].stime)/ (timeTotalAfter-timeTotalBefore);
        j=0;
    }
    qsort(tmp, len, sizeof(MINI_PROC), cmpfunc);
    j=0;
    for(i=n-1;i>=0;i--)
    {
        out[j].pid=tmp[i].pid;
        out[j].ppid=tmp[i].ppid;
        out[j].name=tmp[i].name;
        j++;
    }
}
/*

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
    */
void copyProc(PROC* old,PROC* last,int len,int len2)
{
    int i=0;
    int j=0;
    for(i=0;i<len;i++)
    {
        bool end=false;
        while(old[i].pid!=last[j].pid && !end)
        {
            if(j>=len2-1)
                end=true;
            j++;
        }
        if(end)
            continue;
        old[i].pid=last[j].pid;
        old[i].status=last[j].status;
        old[i].ppid=last[j].ppid;
        old[i].session=last[j].session;
        old[i].tty=last[j].tty;
        old[i].tpgid=last[j].tpgid;
        old[i].flags=last[j].flags;
        old[i].minfaults=last[j].majfaults;
        old[i].utime=last[j].utime;
        old[i].stime=last[j].stime;
        old[i].ctime=last[j].ctime;
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
         unsigned long timeTotalBefore=0;
         getTotalTime(&timeTotalBefore);
         while(1)
         {
            sleep(seconds);
            int nnp=numberOfProcess();
            PROC proc[nnp];
            listOfProcess(proc,nnp);
            MINI_PROC out[n];
            unsigned long timeTotalAfter=0;
            getTotalTime(&timeTotalAfter);
            topTimes(old_proc,proc,out,np,nnp,&timeTotalBefore,&timeTotalAfter);
            clear();
            stmpProc(out);
            copyProc(old_proc,proc,np,nnp);
            // printw("TotalTime spent: %d\n",timeTotalAfter-timeTotalBefore);
            // refresh();
            timeTotalBefore=timeTotalAfter;
        }
        endwin();
    }
    return 0;
}
