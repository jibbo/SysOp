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
    char name[256];
    int ppid;
    float cpu;
}MINI_PROC;


//stampa i processi interessati
//il numero e' definito da 
void stmpAllProc(PROC *proc,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
        printw("pid: %d  \tppid: %d  \tname: %s\n",proc[i].pid,proc[i].ppid,
            proc[i].name);
    }
    refresh();
}

//stampa i processi interessati
//il numero e' definito da 
void stmpProc(MINI_PROC *proc)
{
    int i;
    for(i=0; i<n; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
        printw("pid: %d  \tppid: %d  \tname: %s  \tcpu:%f\n",proc[i].pid,proc[i].ppid,
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
//ritorna il numero di cartelle in proc effettivamente utilizzate
int listOfProcess(PROC *proc) 
{
    DIR *d = opendir("/proc/");
    FILE *f;
    struct dirent *dir;
    int i=0;
    while( ( dir=readdir(d) ) != NULL)
    {
        //char * path = (char *) malloc(sizeof(char)*300);
        char path[300];
        strcpy(path,"/proc/");
        if(dir->d_name[0]>='0' && dir->d_name[0]<='9')
        {
            strcat(path,dir->d_name);
            strcat(path,"/stat");
            f= fopen(path,"r");
            if(f)
            {
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
                i++;
            }
            close(f);
        }
    }  
    closedir(d);
    return i;
}

//ritorna per quanto tempo e' stata utilizzata la cpu
float getTotalTime(){
   FILE *f= fopen("/proc/stat","r"); 
   float out=0;
   if(f)
   {
        unsigned long user=0,nice=0,sys=0;
        char cpu[10];
        fscanf(f,"%s %lu %lu %lu",cpu,&user,&nice,&sys);
        //*out=user+nice+sys;
        out=(float)user+sys;
        close(f);
   }
   return out;
}

//funzione per confrontare due 
//processi in base al pid
int cmpPID (const void * a, const void * b)
{
    return ((*(PROC*)a).pid - (*(PROC*)b).pid);
}

//funzione per confrontare due 
//mini processi in base alla cpu utilizzata
//il -1 e' per fare l'ordinamento dcrescente
int cmpCPU (const void * a, const void * b)
{
    return ((*(MINI_PROC*)a).cpu - (*(MINI_PROC*)b).cpu) *-1;
    //return 1;
}

//funzione principale
void *topTimes(PROC* before,PROC* after, MINI_PROC* out,int len,int len2,
    float timeTotalBefore,float timeTotalAfter)
{
    MINI_PROC tmp[len2];
    int i=0;

    for(i=0;i<len2;i++)
    {
        tmp[i].pid=after[i].pid;
        tmp[i].ppid=after[i].ppid;
        strcpy(tmp[i].name,after[i].name);
        //calcolo la cpu utilizzata da ogni processo
        if(i<len && after[i].pid==before[i].pid)
          tmp[i].cpu=100*((after[i].stime+after[i].utime)-
            (before[i].utime+before[i].stime))/ 
            (timeTotalAfter-timeTotalBefore);
         else
          tmp[i].cpu=100* (after[i].stime+after[i].utime) / (timeTotalAfter-timeTotalBefore);
    }
    //ordino l'array per utilizzo di cpu in modo crescente
    qsort(tmp, len2, sizeof(MINI_PROC), cmpCPU);
    
    
    //questa e' per controllare che non vada
    // l'utente non chieda piu' processi di
    //quelli  presenti nel sistema 
    n = (n>len2) ? len2:n;

    //prendo i primi n processi
    for(i=0;i<n;i++)
    {
        out[i].pid=tmp[i].pid;
        out[i].ppid=tmp[i].ppid;
        strcpy(out[i].name,tmp[i].name);
        out[i].cpu=tmp[i].cpu;
    }
}

//elimina il contenuto di old
//e copia tutto il contenuto di last
//in old
void copyProc(PROC* old,PROC* last,int len2)
{
    free(old);
    old = (PROC *) malloc(sizeof(PROC)*len2);
    int i=0;
    for(i=0;i<len2;i++)
    {
        old[i].pid=last[i].pid;
        //old[i].name=last[i].name;
        old[i].status=last[i].status;
        old[i].ppid=last[i].ppid;
        old[i].pgrp=last[i].pgrp;
        old[i].session=last[i].session;
        old[i].tty=last[i].tty;
        old[i].tpgid=last[i].tpgid;
        old[i].flags=last[i].flags;
        old[i].minfaults=last[i].minfaults;
        old[i].majfaults=last[i].majfaults;
        old[i].utime=last[i].utime;
        old[i].stime=last[i].stime;
        old[i].ctime=last[i].ctime;
    }
    free(last);
}


int main(int argc, char **argv) 
{
    if(checkflag(argc,argv)!=-1) 
    {
         initscr();
         
         //operazioni da fare per avere
         //tutti i dati da processare
         int np=numberOfProcess();
        
         //PROC old_proc[np];
         PROC *old_proc = (PROC *) malloc(sizeof(PROC)*np);
         np=listOfProcess(old_proc);
         float timeTotalBefore= getTotalTime();
         qsort(old_proc, np, sizeof(PROC), cmpPID);
         
          //creo il thread che aspetta 
         //l'input dell'utente
         pthread_t look;
         pthread_create(&look, NULL, updateVariables, NULL);
         int nnp=0;
         while(1)
         {
            sleep(seconds);
            
            //leggo i nuovi dati
            
            nnp=numberOfProcess();
            //PROC proc[nnp];
            PROC *proc = (PROC *) malloc(sizeof(PROC)*nnp);
            nnp=listOfProcess(proc);
            qsort(proc, nnp, sizeof(PROC), cmpPID);

            //debug FINO QUA NESSUN SEGFAULT
            //clear();
            //stmpAllProc(proc,nnp);
            
            MINI_PROC out[n];
            float timeTotalAfter=getTotalTime();
            topTimes(old_proc,proc,out,np,nnp,timeTotalBefore,timeTotalAfter);
            
            //pulisco e stampo a video
            clear();
            stmpProc(out);

            //Aggiorno i dati per il prossimo "giro"
            copyProc(old_proc,proc,nnp);
            np=nnp;
            timeTotalBefore=timeTotalAfter;
        }
        endwin();
    }
    return 0;
}

