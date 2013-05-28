#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>


int n=10; //number of process
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

//stampa i processi interessati.
void stmpProc(PROC *proc)
{
    int i;
    for(i=0; i<n; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
				printf("pid: %d\t%s\t\n",proc[i].pid,proc[i].name);
		}
}

/*
 *
 * Function that set the number of
 * procces that will be displayed
 */
int checkflag(int argc, char **argv) {
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

/*
 * return the number of all the process running
 * avoiding useless folder 
 */
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

/*
* Riempie l'array di processi che deve guardare
* ritorna il numero di cartelle in proc che non devono essere  
*/
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
        //out[i]=dir->d_name;
				char path[300];
				strcpy(path,"/proc/");
				if(dir->d_name[0]>='0' && dir->d_name[0]<='9')
				{
					strcat(path,dir->d_name);
					strcat(path,"/stat");
					f= fopen(path,"r");
					if(f){
					  //leggo il contenuto di /proc/pid/stat
					  fscanf(f,"%d %s %c %d %d %d %d %d %u %ul %ul %ul %ul %ul",
					  &proc[i].pid,proc[i].name,&proc[i].status,&proc[i].ppid,
					  &proc[i].pgrp,&proc[i].session,&proc[i].tty,&proc[i].tpgid,
					  &proc[i].flags,&proc[i].minfaults,&proc[i].majfaults,
					  &proc[i].utime,&proc[i].stime, &proc[i].ctime);
					
					  //rimuovo le parentesi dal nome.
					  char *name = proc[i].name;
					  int j=0;
					  for(j=1;j<strlen(name)-1; j++)
					    proc[i].name[j-1]=name[j];
					  proc[i].name[j-1]='\0'; 
					}
				}else i--; //this is to avoid useless folder.
    }
    closedir(d);
}


int main(int argc, char **argv) {
    if(checkflag(argc,argv)!=-1) {
			//printf ("n:%d\n",n);
			int np=numberOfProcess();
			//printf ("total process:%d\n",np);
			PROC proc[np];
			listOfProcess(proc,np);
			//stmpOfArray(proc,np);
			while(1){
			  stmpProc(proc);
			  sleep(5);
			}
    }
    return 0;
}
