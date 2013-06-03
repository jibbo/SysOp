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

    syslog(LOG_INFO, "started: %s", argv[0]);

    if(checkflag(argc,argv)!=-1) 
    {
        initscr();

        syslog(LOG_INFO, "called plive with: %d", n);

        signal(SIGSEGV, errors);


        //operazioni da fare per avere
        //tutti i dati da processare
        int numProcbefore=numberOfProcess();

        PROC *before_proc = (PROC *) malloc(sizeof(PROC)*numProcbefore);
        numProcbefore=listOfProcess(before_proc,numProcbefore);
        float timeTotalBefore= getTotalTime();
        qsort(before_proc, numProcbefore, sizeof(PROC), cmpPID);

        //creo il thread che aspetta 
        //l'input dell'utente
        pthread_t look;
        pthread_create(&look, NULL, updateVariables, NULL);

        int actualNumProc=0;
        while(1)
        {
            sleep(seconds);

            //leggo i nuovi dati
            actualNumProc=numberOfProcess();
            PROC *proc = (PROC *) malloc(sizeof(PROC)*actualNumProc);
            actualNumProc=listOfProcess(proc,actualNumProc);
            qsort(proc, actualNumProc, sizeof(PROC), cmpPID);

            //preparo le variabili per i calcoli 
            MINI_PROC out[(n>actualNumProc)?actualNumProc:n];
            float timeTotalAfter=getTotalTime();

            int nloc=topTimes(before_proc,proc,out,numProcbefore,actualNumProc,timeTotalBefore,timeTotalAfter);

            //pulisco e stampo a video
            clear();
            stmpNProc(out,nloc);

            //Aggiorno i dati per il prossimo "giro"
            copyProc(before_proc,proc,actualNumProc);
            numProcbefore=actualNumProc;
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

    //segnalo che c'e' stato un segmentation fault
    syslog(LOG_ERR, " program exited because of signal %d", sig);

    //chiudo tutto.
    closelog();
    endwin();
    exit(EXIT_FAILURE);
}

//stampa i processi interessati
void stmpNProc(MINI_PROC *proc,int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        //divido la cpu calcolata per 10 per una migliore visualizzazione
        printw("pid: %d  \tppid: %d  \tname: %s  \tcpu:%2.2f\n",proc[i].pid,proc[i].ppid,
        proc[i].name,proc[i].cpu/10);
    }
    refresh();
}

//funzione che fa un parsing delle opzioni dati al programma
//quindi in posta il numero di processi da visualizzare
//ritorna -1 nel caso di errori
int checkflag(int argc, char **argv) 
{
    opterr = 0;
    int c = getopt (argc, argv, "n:");
    switch (c)
    {
        case 'n':
            n = atoi(optarg);
            //controllo se il numero passato e' valido
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
                syslog(LOG_ERR,"Unknown option character `\\x%x'.\n",optopt);
                fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
            }
            return -1;
        //il caso in cui non viene passato nulla
        default:
            return 1;
    }
}

//funzione che viene usata da 
//un thread per gestire l'input
//dell' utente.
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

//ritorna il numero di processi attivi sul sistema
//evitando le cartelle che non hanno un PID
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

//rimuove le parentesi dal nome dei processi
void removeParentesis(char * in, char * out)
{    
    int i=0;
    //poiche' i processi nello stat vengono 
    //memorizzati come (nome)
    //mi basta iniziare a copiare da 1 
    // e finire a strlen(nome letto)-1
    for(i=1;i<strlen(in)-1;i++){
        out[i-1]=in[i];
    }
    //le stringe in C sono 0 terminated quindi
    //lo aggiungo alla fine.
    out[i-1]='\0';
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
                char* tmp = (char *) malloc(sizeof(char)*256);
                //leggo il contenuto di /proc/pid/stat
                fscanf(f,"%d %s %c %d",
                &proc[i].pid,tmp,&proc[i].status,&proc[i].ppid);
                int j=0;
                removeParentesis(tmp,proc[i].name);
                while(j<10){
                    fscanf(f,"%s",tmp);
                    j++;
                }
                proc[i].utime=atof(tmp);
                free(tmp);  
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
        out=(float)user;
        fclose(f);
    }
    return out;
}

//funzione per confrontare due
//processi in base al PID
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

//questa funzione calcola la quantita' di cpu utilizzata da ogni processo
//e genera l'array di processi da mostrare all'utente 
int topTimes(PROC* before,PROC* after, MINI_PROC* out,int lenBefore,int lenAfter,float timeTotalBefore,float timeTotalAfter)
{
    MINI_PROC tmp[lenAfter];
    int i=0;

    for(i=0;i<lenAfter;i++)
    {
        //copio le info necessarie nell'array di appoggio
        tmp[i].pid=after[i].pid;
        tmp[i].ppid=after[i].ppid;
        strcpy(tmp[i].name,after[i].name);

        //calcolo la cpu utilizzata da ogni processo
        int j=0;

        //cerco se il processo era attivo anche nella lettura
        //precendente
        while(after[i].pid!=before[j].pid && j<lenBefore){j++;}
        if()
        //controllo se l'ho trovato
        if(after[i].pid==before[j].pid)
        {
            //controllo se l'output sarebbe positivo
            //e in caso non lo fosse giro i fattori
            if(after[i].utime-before[j].utime>0)
            {
                tmp[i].cpu=100*(after[i].utime-before[j].utime)/(timeTotalAfter-timeTotalBefore);
            }
            else
            {
                tmp[i].cpu=100*(before[i].utime-after[j].utime)/(timeTotalAfter-timeTotalBefore);
            }
        }
        else
        {
            //vuol dire che il processo e' stato appena avviato
            //e quindi non ho dati di letture precendenti
            tmp[i].cpu=100*after[i].utime/timeTotalAfter;
        }
    }

    //ordino l'array per utilizzo di cpu in modo crescente
    qsort(tmp, lenAfter, sizeof(MINI_PROC), cmpCPU);

    //controllo che l'utente non abbia chiesto di visualizzare
    //piu' processi di quelli che sono attivi
    int nloc = (n>lenAfter) ? lenAfter:n;

    //prendo i primi n processi
    for(i=0;i<nloc;i++)
    {
        out[i].pid=tmp[i].pid;
        out[i].ppid=tmp[i].ppid;
        strcpy(out[i].name,tmp[i].name);
        out[i].cpu=tmp[i].cpu;
    }
    return nloc;
}

//rimpiazza la vecchia lettura
//con l'ultima
void copyProc(PROC* before,PROC* last,int lenAfter)
{
    before = (PROC *) malloc(sizeof(PROC)*lenAfter);
    int i=0;
    for(i=0;i<lenAfter;i++)
    {
        before[i].pid=last[i].pid;
        strcpy(before[i].name,last[i].name);
        before[i].status=last[i].status;
        before[i].ppid=last[i].ppid;
        before[i].utime=last[i].utime;
    }
    free(last);
}