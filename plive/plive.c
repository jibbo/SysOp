/*
    Anno accademico     2012/2013
    Corso studio        Informatica
    Progetto            #1 Plive
    Componenti:
        Zen Roberto             151737
        Giovanni De Francesco   152080
        Perantoni Nicola        151709
*/

#include "plive.h"

int main(int argc, char **argv) 
{
    openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);

    syslog(LOG_INFO, "plive started: %s", argv[0]);

    if(checkflag(argc,argv)!=-1) 
    {
         initscr();

         syslog(LOG_INFO, "called plive with: %d", n);

         signal(SIGSEGV, errors);


         //operazioni da fare per avere
         //tutti i dati da processare
         int numProcOld=numberOfProcess();

         PROC *old_proc = (PROC *) malloc(sizeof(PROC)*numProcOld);
         numProcOld=listOfProcess(old_proc,numProcOld);
         //float timeTotalBefore= getTotalTime();
         qsort(old_proc, numProcOld, sizeof(PROC), cmpPID);
         
          //creo il thread che aspetta 
         //l'input dell'utente
         pthread_t look;
         pthread_create(&look, NULL, updateVariables, NULL);

         int actualNumProc=0;
         while(1)
         {
            sleep(seconds);
            
            //leggo i nuovi dati
            actualNumProc=numberOfProcess()+1;
            PROC *proc = (PROC *) malloc(sizeof(PROC)*actualNumProc);
            actualNumProc=listOfProcess(proc,actualNumProc);
            qsort(proc, actualNumProc, sizeof(PROC), cmpPID);
            
            
            MINI_PROC out[n];
            //float timeTotalAfter=getTotalTime();
            int nloc=topTimes(old_proc,proc,out,numProcOld,actualNumProc);
            
            //pulisco e stampo a video
            clear();
            stmpNProc(out,nloc);

            //Aggiorno i dati per il prossimo "giro"
            copyProc(old_proc,proc,nloc);
            numProcOld=nloc;
        }
        endwin();
    }
    syslog(LOG_INFO, "plive ended", n);
    closelog();
    return 0;
}

//funzione che viene chiamato quando 
//qualcosa di brutto acade al 
//nostro programma (e.g. Segmentation fault)
void errors(int sig) {
  endwin();
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  syslog(LOG_ERR, " program exited because of signal %d", sig);
  //backtrace_symbols_fd(array, size, 2);
  closelog();
  exit(EXIT_FAILURE);
}

//stampa i processi interessati
//il numero e' definito da 
void stmpNProc(MINI_PROC *proc,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        //printf("%d\t%s\n",proc[i].pid,proc[i].name);
         printw("pid: %d  \tppid: %d  \tname: %s  \tcpu:%2.2f\n",proc[i].pid,proc[i].ppid,
            proc[i].name,proc[i].cpu);
    }
    refresh();
}

//Function that set the number of
//procces that will be displayed
//return -1 in case of error
int checkflag(int argc, char **argv) 
{
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
            syslog(LOG_ERR, "Invalid argument passed to `-n' option.");
            return -1;
        }
        return 1;
        case '?':
        if (optopt == 'n')
        {
            syslog(LOG_ERR, "Option `-n` requires an argument.");
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        }
        else if (isprint (optopt))
        {
            syslog(LOG_ERR, "Unknown option `-%c`.",optopt);
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        }
        else
        {
            syslog(stderr,"Unknown option character `\\x%x'.\n",optopt);
            fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        }
        
         return -1;
        //case for nothing passed as option
        default:
            return 1;
    }
}

//this is the function that is 
//called by a thread to watch
//user's input
void *updateVariables()
{
    while(true)
    {
        char c = getch();
        if(c>='0' && c<='9')
        {
            seconds=atoi(&c);
            syslog(LOG_INFO, "updated refresh time to: %c", c);
        }
        else if(c=='q'){
            syslog(LOG_INFO, "plive ended by user");
            endwin();
            exit(EXIT_SUCCESS);
            closelog();
        }
        else
            syslog(LOG_ERR, "invalid input %c",c);
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
      //guardo solo le cartelle che iniziano con numeri per essere
      //sicuro che siano pid di processi  
      if(dir->d_name[0]>='0' && dir->d_name[0]<='9')
        nt++;
    }
    closedir(d);
    return nt;
}

//Riempe l'array di processi che deve guardare
//ritorna il numero di cartelle in proc effettivamente utilizzate
int listOfProcess(PROC *proc, int len) 
{
    DIR *d = opendir("/proc/");
    FILE *f;
    struct dirent *dir;
    int i=0;
    while( ( dir=readdir(d) ) != NULL && i<len)
    {
        char *path = (char *) malloc(sizeof(char)*267);
        strcpy(path,"/proc/");
        if(dir->d_name[0]>='1' && dir->d_name[0]<='9')
        {
            strcat(path,dir->d_name);
            strcat(path,"/stat");
            f= fopen(path,"r");
            if(f)
            {
                //leggo il contenuto di /proc/pid/stat
                fscanf(f,"%d %s %c %d",
                    &proc[i].pid,proc[i].name,&proc[i].status,&proc[i].ppid);
                int j=0;
                char* tmp = (char *) malloc(sizeof(char)*10);
                while(j<10){
                    fscanf(f,"%s",tmp);
                    j++;
                }
                proc[i].utime=atof(tmp);
                free(tmp);
                //TODO remove parenthesis   
                i++;
            }
            fclose(f);
        }
        free(path);
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
        fclose(f);
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
int cmpCPU (const void * a, const void * b)
{
    return ((*(MINI_PROC*)a).cpu - (*(MINI_PROC*)b).cpu)*-1;
}

//funzione principale
int topTimes(PROC* before,PROC* after, MINI_PROC* out,int len1,int len2)
{
    int first=0;
    int minlen=0;
    int maxlen=0;
    if(len2>len1)
    {
        minlen=len1;
        maxlen=len2;
        first=1;
    }
    else
    {
        minlen=len2;
        maxlen=len1;
        first=0;
    }
    MINI_PROC tmp[maxlen];
    int i=0;

    for(i=0;i<minlen;i++)
    {
        tmp[i].pid=after[i].pid;
        tmp[i].ppid=after[i].ppid;
        strcpy(tmp[i].name,after[i].name);
        //calcolo la cpu utilizzata da ogni processo
        if(after[i].pid==before[i].pid)
            tmp[i].cpu=(after[i].utime-before[i].utime)/(1.0*seconds);

    }
    for(i=minlen-1;i<maxlen;i++)
    {
        if(first)
        {
            tmp[i].pid=after[i].pid;
            tmp[i].ppid=after[i].ppid;
            strcpy(tmp[i].name,after[i].name);
            //tmp[i].cpu=after[i].utime/seconds;
            tmp[i].cpu=(float)after[i].utime;
        }
        else
        {
            tmp[i].pid=before[i].pid;
            tmp[i].ppid=before[i].ppid;
            strcpy(tmp[i].name,before[i].name);
            tmp[i].cpu=0;
        }
        
    }
    //ordino l'array per utilizzo di cpu in modo crescente
    qsort(tmp, maxlen, sizeof(MINI_PROC), cmpCPU);
    
    //questa e' per controllare che
    // l'utente non chieda piu' processi di
    //quelli  presenti nel sistema 
    int nloc = (n>maxlen) ? maxlen:n;

    //prendo i primi n processi
    for(i=0;i<n;i++)
    {
        out[i].pid=tmp[i].pid;
        out[i].ppid=tmp[i].ppid;
        strcpy(out[i].name,tmp[i].name);
        out[i].cpu=tmp[i].cpu;
    }
    return nloc;
}

//elimina il contenuto di old
//e copia tutto il contenuto di last
//in old
void copyProc(PROC* old,PROC* last,int len2)
{
    free(old);
    old = (PROC *) malloc(sizeof(PROC)*len2+1);
    int i=0;
    for(i=0;i<len2;i++)
    {
        old[i].pid=last[i].pid;
        strcpy(old[i].name,last[i].name);
        old[i].status=last[i].status;
        old[i].ppid=last[i].ppid;
        old[i].utime=last[i].utime;
    }
    free(last);
}