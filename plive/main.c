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
	char* name;
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
 */
int  numberOfProcess()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("/proc/");
    int nt=0;
    while ((dir = readdir(d)) != NULL)
    {
        nt++;
    }
    closedir(d);
    return nt;
}

//void listOfProcess(char *out[256],int len) 
void listOfProcess(PROC *proc,int len) 
{
    int i;
    DIR *d = opendir("/proc/");
		FILE *f;
    struct dirent *dir;
		for(i=0; i<len; i++)
    {
        dir= readdir(d);
        //out[i]=dir->d_name;
				char path[300];
				strcpy(path,"/proc/");
				if(dir->d_name[0]>='0'&& dir->d_name[0]<='9')
				{
					strcat(path,dir->d_name);
					strcat(path,"/stat");
					f= fopen(path,"r");
					if(f){
						fscanf(f,"%d",&proc[i].pid);
						//fscanf(f,"%s",proc[i].name);
					}
				}
    }
    closedir(d);
}
//void stmpOfArray(char** arr,int len)
void stmpOfArray(PROC *proc,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
				printf("pid: %d\t%s\t potenza!\n",proc[i].pid,proc[i].name);
		}
}


int main(int argc, char **argv) {
    if(checkflag(argc,argv)!=-1) {
			printf ("n:%d\n",n);
			//stmpOfArray(listOfProcess("/proc/"));
			int np=numberOfProcess();
			printf ("total process:%d\n",np);
			PROC proc[np];
			listOfProcess(proc,np);
			//stmpOfArray(proc,np);
    }
    return 0;
}
