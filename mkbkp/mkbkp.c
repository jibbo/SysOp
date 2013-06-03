/*
    Anno accademico     2012/2013
    Corso studio        Informatica
    Progetto            #1 MKBKP
    Componenti:
        Zen Roberto             151737
        Giovanni De Francesco   152080
        Perantoni Nicola        151709
*/

#include "mkbkp.h"

#define PATH_MAX_LENGTH 256

// Struct utilizzato per gestire l'inizializzazione del file
// di backup quando viene costruito (durante la creazione)
typedef struct {
  char path[PATH_MAX_LENGTH];
  FILE* file;
  char* line;
  size_t size;
  ssize_t read;
} str_file;

FILE* backup;

// Stringhe utilizzate per gestire le directory
char filevalue[PATH_MAX_LENGTH];
char dirvalue[PATH_MAX_LENGTH];
char cwd[PATH_MAX_LENGTH];
char subpath[PATH_MAX_LENGTH];

// Variabili che gestiscono i flag della syscall getopt()
int f_flag = 0;
int c_flag = 0;
int x_flag = 0;
int t_flag = 0;

char *f_value = NULL;
char *t_value = NULL;

int main(int argc, char **argv) {
  int c;
  opterr = 0;

  // Apro il file di log..
  openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);
  
  // Utilizzo la syscall getopt() per i diversi flag utilizzati dall'applicazione
  while ((c = getopt (argc, argv, "f:cxt:")) != -1) {
    switch (c) {
      case 'f':
        f_flag = 1;
        f_value = optarg; 
        break;
      case 'c':
        c_flag = 1;
        break;
      case 'x':
        x_flag = 1;
        break;
      case 't':
        t_flag = 1;
        t_value = optarg;
        break;
      default:
        syslog(LOG_INFO, "Flag not available");
        exit(EXIT_FAILURE);
    }
  }

  manage();
  return 0;
}

// Funzione che gestisce tutte le condizioni di input che può avere l'utility
// se viene utilizzato il flag -t allora viene segnalato che il suddetto flag non può essere utilizzato insieme ad altri
// infatti per mostrare il contenuto di un file bisogna eseguire: ./mkbkp -t <nomefile>.bkp

// se viene utilizzato il flag -x avviene il medesimo controllo, con la differenza che viene controllato
// che sia presente anche il flag -f perché quel flag serve ad indicare il file da estrarre

// se viene utilizzato il flag -c vale la medesima condizione che è già stata illustrata per il flag -x
// con la differenza che in questo caso viene esguito il backup di un file.

// se viene passato input solo il flag -f viene loggato l'errore, il flag non può essere utilizzato da solo

void manage() {
  // T_FLAG
  if(t_flag == 1) {
    if(f_flag == 1 || x_flag == 1 || c_flag == 1) {
      printf("-t flag must be used without any flags\n");
      syslog(LOG_INFO, "-t flag must be used without any flags");
    } else if(t_value == NULL) {
      printf("-t requires a parameter\n");
      syslog(LOG_INFO, "-t requires a parameter");
    } else {
      printf("ready to show the archive content\n");
      syslog(LOG_INFO, "ready to show the archive content");
      showBackupContent(t_value);
    }
  }

  // X_FLAG
  if(x_flag == 1) {
    if(f_flag == 1) {
      printf("ready for archive extraction\n");
      syslog(LOG_INFO, "ready for archive extraction");
      extractBackup(f_value);
    } else if(f_value == NULL) {
      printf("the archive provided with the -f flag cannot be empty\n");
      syslog(LOG_INFO, "the archive provided with the -f flag cannot be empty");
    } else if(c_flag == 1) {
      printf("-x and -c cannot be used together\n");
      syslog(LOG_INFO, "-x and -c cannot be used together");
    } else {
      printf("-x cannot be used without -f\n");
      syslog(LOG_INFO, "-x cannot be used without -f");
    }
  }

  // C_FLAG
  if(c_flag == 1) {
    if(f_flag == 0) {
      printf("-f flag is not present\n");
      syslog(LOG_NOTICE, "-f flag is not present");
    } else if(f_value == NULL) {
      printf("no target provided for the -f flag\n");
      syslog(LOG_NOTICE, "no target provided for the -f flag");
    } else {
      printf("ready to backup the provided folder\n");
      syslog(LOG_INFO, "ready to backup the provided folder");
      makeBackup(f_value);
    }
  }

  // F_FLAG
  if(f_flag == 1) {
    if(t_flag == 0 && x_flag == 0 && c_flag == 0) {
      printf("-t flag requires a parameter: <the archive> as *.bpk\n");
      syslog(LOG_INFO, "-t flag requires a parameter: <the archive> as *.bpk");
    }
  }
}


// Metodo che contiene della breve documentazione che viene mostrato all'utente
// se tenta di eseguire l'utility con un flag che non è stato implementato o se i
// parametri utilizzati non rispettano quelli supportati dell'applicazione.

void printHelp() {
  printf("Usage: mkbkp [-c] [-x] [-t] [-f]\n");
  printf("\t -f <archive>.bkp to create or extract an archive");
  printf("\n\t -c to create a new archive");
  printf("\n\t -x to extract an archive in the current directory");
  printf("\n\t -t <archive>.bkp to display the content of an archive\n");
}

// Metodo che si occupa di eseguire il backup di un file
// prende come input la cartella di cui deve essere eseguito il backup
// il nome del file destinatario del backup viene preso tramite la variabile 
// globale filevalue

void makeBackup(char* path) {
  struct dirent* direntry;
  DIR* dir;
  struct stat stbuf;
  int is_dir;

  // La funzione opendir apre un directory stream.
  // Restituisce un puntatore ad un oggetto di tipo DIR in caso di successo e NULL in caso di errore.
  // Inoltre posiziona lo stream sulla prima voce contenuta nella directory.
  if ((dir = opendir(path)) == NULL) { 
      printf("Can't open the folder %s\n", path);
      syslog(LOG_INFO, "Can't open the folder %s", path);
      return;
  }

  // La funzione readdir legge la voce corrente nella directory, posizionandosi sulla voce successiva.
  // Restituisce un puntatore al directory stream in caso di successo e NULL in caso di errore.
  // Loop on directory entries
  while ((direntry = readdir(dir)) != NULL) {
    if (strcmp(direntry -> d_name, ".") == 0 || strcmp(direntry -> d_name, "..") == 0) {
      continue;
    }

    size_t length = strlen(path) + strlen(direntry -> d_name) + 2;
    char * concat = (char *) malloc(sizeof(char) * length);
    snprintf(concat, length, "%s/%s", path, direntry -> d_name);
    stat(concat, &stbuf);

    // Legge la cartella corrente
    getcwd(cwd, PATH_MAX_LENGTH);
    strcat(cwd, "/");
    strcat(cwd, filevalue);

    // Apro il file di backup in Append
    backup = fopen(cwd, "a+");
    if(backup == NULL) {
      perror(cwd);
      exit(EXIT_FAILURE);
    }

    // Bool che indica se è una directory
    is_dir = ((stbuf.st_mode & S_IFMT) == S_IFDIR);

    // Se è una directory
    if (is_dir) {
      strcpy(subpath, path);
      strcat(subpath, "/");
      strcat(subpath, direntry -> d_name);

      fprintf(backup, "\nDIR=");
      fprintf(backup, "%s", subpath);

      makeBackup(subpath);
    } else {
      // allora è un file
      str_file* temp;

      temp = (str_file* ) malloc(sizeof(str_file));
      strcpy(temp -> path, path);
      strcat(temp -> path, "/");
      strcat(temp -> path, direntry -> d_name);

      temp -> file = fopen(temp -> path, "rb");
      if(temp -> file == NULL) {
        perror(temp -> path);
        exit(EXIT_FAILURE);
      }

      fseek(temp -> file, 0, SEEK_END);
      temp -> size = ftell(temp -> file);
      fseek(temp -> file, 0, SEEK_SET);

      temp -> line = (char* ) malloc(temp -> size);
      temp -> read = fread(temp -> line, temp -> size, 1, temp -> file);

      // Scrive il delimitatore del file all'interno dell'archivio
      fprintf(backup, "\nFILE=");
      fprintf(backup, "%s", temp -> path);
      fprintf(backup, "\n");

      // Scrive il file appena letto all'interno dell'archivio
      fwrite(temp->line, 1, temp -> size, backup);

      fprintf(backup, "\nENDFILE");

      fclose(temp -> file);

      free(temp -> line);
      free(temp);
    }

    fclose(backup);
  }

  closedir(dir);
  syslog(LOG_INFO, "file written to the backup correctly");
}

int startsWithPre(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

void showBackupContent(char* archive) {
  char workingdir[PATH_MAX_LENGTH];
  getcwd(workingdir, PATH_MAX_LENGTH);
  strcat(workingdir, "/");
  strcat(workingdir, archive);

  const char* pref = "FILE=";

  char buff[PATH_MAX_LENGTH];

  FILE* archivetoshow = fopen(workingdir, "r");
  if(archivetoshow == NULL) {
    perror(workingdir);
    exit(EXIT_FAILURE);
  } else {
    while(fgets(buff, PATH_MAX_LENGTH, archivetoshow) != NULL) {
      if(startsWithPre(pref, buff) == 1) {
        printf("%s", buff);
      }
    }
  }
  fclose(archivetoshow);
  syslog(LOG_INFO, "utility showed content of the backup file correctly");
}

// Questo metodo viene utilizzato per creare le sottocartelle
// ricorsivamente quando viene estratto un archivio

static void recursiveDirMake(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof (tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {

            *p = 0;
            mkdir(tmp, 0777);
            *p = '/';
        }
    mkdir(tmp, 0777);
}


// Metodo che si occupa di estrarre un archivio che viene 
// passato come input
// Viene assunto che l'archivio verrà estratto nella cartella corrente di lavoro,
// infatti la prima operazione che viene eseguita è ottenere la cartella di lavoro
// per estrarre lì l'archivio

void extractBackup(char* archive) {
  char workingdir[PATH_MAX_LENGTH];
  getcwd(workingdir, PATH_MAX_LENGTH);
  strcat(workingdir, "/");
  strcat(workingdir, archive);

  const char* pref = "FILE=";

  char buff[PATH_MAX_LENGTH];
  char buff2[PATH_MAX_LENGTH];

  FILE* archivetoshow = fopen(workingdir, "r");
  if(archivetoshow == NULL) {
    perror(workingdir);
    exit(EXIT_FAILURE);
  } else {

    // Viene fatto due volte il parsing del file che deve essere estratto
    // La prima volta è costituita dal ciclo while qui sotto; durante il primo
    // ciclo sul file vengono identificate le directory che sono presenti
    // all'interno dello stesso per poi estrarre i relativi file all'interno delle stesse

    while(fgets(buff, PATH_MAX_LENGTH, archivetoshow) != NULL) {
      FILE* temp = NULL;
      if(startsWithPre("DIR=", buff) == 1) {
        char *tokenDir;
        char *search = "=";

        tokenDir = strtok(buff, search);
        tokenDir = strtok(NULL, search);
        recursiveDirMake(tokenDir);
      }
    }

    // Viene chiuso il file archivio perchè il parsing  è arrivato a fine file
    // Viene successivamente riaperto per poter essere letto una seconda volta
    // durante la quale verranno identificati i file all'interno di esso

    // Le cartelle vengono memorizzate nel seguente modo:
    // 
    // DIR=<path assoluto della cartella>
    // \n
    // <contenuto del file>ENDFILE

    // I File invece vengono memorizzati nel seguente modo:
    // 
    // FILE=<path assoluto del file>
    // \nENDFILE

    fclose(archivetoshow);
    FILE* archivetoshow = fopen(workingdir, "r");
    FILE* temp = NULL;

    // Continua a leggere il file fino a quando non arriva alla fine
    // ad ogni ciclo apre il file destinatario per esportare il file
    // dal file di backup alla cartella di destinazione

    while(fgets(buff2, PATH_MAX_LENGTH, archivetoshow) != NULL) {
      if(startsWithPre(pref, buff2) == 1) {
        char *token;
        char *search = "=";

        token = strtok(buff2, search);
        token = strtok(NULL, search);

        //è un file
        temp = fopen(token, "a+");
        if(temp == NULL) {
          exit(EXIT_FAILURE);
        }

      } else if((int)strncmp(buff2, "ENDFILE", 7) == 0) {
          fclose(temp);
          FILE* temp = NULL;
      } else if(startsWithPre("DIR=", buff2)) { 
          continue;
      } else {
          fprintf(temp, "%s", buff2);
      }
    }
    fclose(archivetoshow); 
  }
  syslog(LOG_INFO, "backup file was extraced correctly");
}