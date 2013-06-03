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

// Per mostrare il contenuto di un file
// e per l'estrazione viene utilizzata una variabile di tipo file

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

int main(int argc, char **argv) {
  int c;
  opterr = 0;

  // Apro il file di log..
  openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);
  
  // Utilizzo la syscall getopt() per i diversi flag utilizzati dall'applicazione
  while ((c = getopt (argc, argv, "f:cxt")) != -1) {
    switch (c) {
      case 'f':
        f_flag = 1;
        strcpy(filevalue, optarg);
        
        if(strcpy(dirvalue, argv[optind]) != NULL) {
          printf("%s\n", dirvalue);
          strcpy(dirvalue, argv[optind]);
        } else {
          continue;
        }

        break;
      case 'c':
        c_flag = 1;
        break;
      case 'x':
        x_flag = 1;
        break;
      case 't':
        t_flag = 1;
        break;
      case '?':
        if (optopt == 'f') {
          fprintf(stderr, "Option -%c requires the archive as an argument\n", optopt);
        } else if (isprint (optopt)) {
          fprintf(stderr, "Unknown option '-%c'\n", optopt);
          printHelp();
        } else {
          fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
        }
        return 1;
      default:
        abort ();
    }
    manage();
  }

  return 0;
}

// Gestisco tutti i casi di input che posso ricevere dall'utente:
// * se il flag -c è utilizzato ma sia il file che la directory sono nulli mostro che deve essere inserito un archivio
//   utilizzando anche il flag -f
// * se il flag -c e il flag -f sono a 1 ma non vengono inseriti nè un file nè una directory
//   comunico all'utente che li deve inserire
// * se viene utilizzato il flag -x per estrarre un archivio ma non viene specificato l'archivio allora
//   viene visualizzato il relativo messaggio di errore
// * se viene utilizzato il flag -x in concomitanza con il flag -f allora significa che l'utente ha utilizzato
//   il primo dei due per segnalare di voler estrarre l'archivio passato come parametro utilizzando il flag -f
// * se viene utilizzato il flag -t per visualizzare il contenuto di un archivio ma l'archivio non viene specificato
//   come parametro mostro il relativo messaggio di errore
// * infine, se vengono utilizzati sia il flag t ed il flag f allora eseguo il metodo che mostra
//   il contenuto di un file di backup passato come parametro

void manage() {
  if(c_flag == 1 && (filevalue == NULL && dirvalue == NULL)) {
    printf("You must use the -f flag to specify the archive you want to create\n");

  } else if((c_flag == 1 && f_flag == 1) && (filevalue != NULL && dirvalue != NULL)) {
    // Crea l'archivio
    printf("Creo l'archivio chiamato: %s della cartella: %s\n", filevalue, dirvalue);
    makeBackup(dirvalue);
  } else if(x_flag == 1 && (filevalue == NULL && dirvalue == NULL)) {
    printf("You must use the -f flag to specify the archive you want to extract\n");

  } else if((x_flag == 1 && f_flag == 1) && (filevalue != NULL && dirvalue != NULL)) {
    printf("Estraggo l'archivio: %s\n", filevalue);
    extractBackup(filevalue);

  } else if(t_flag == 1 && (filevalue == NULL && dirvalue == NULL)) {
    printf("You must use the -f flag to specify the archive you want to analyze\n");

  } else if ((t_flag == 1 && f_flag == 1) && (filevalue != NULL) {
    // Mostra il contenuto dell'archivio
    printf("%s\n", filevalue);
    showBackupContent(filevalue);
  }
}

// Metodo che contiene della breve documentazione che viene mostrato all'utente
// se tenta di eseguire l'utility con un flag che non è stato implementato o se i
// parametri utilizzati non rispettano quelli supportati dell'applicazione.

void printHelp() {
  printf("Usage: mkbkp [-c] [-x] [-t] [-f]\n");
  printf("\t -f to create or extract an archive");
  printf("\n\t -c to create a new archive");
  printf("\n\t -x to extract an archive in the current directory");
  printf("\n\t -t to display the content of an archive\n");
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
  int access_err;

  FILE *openedfile;
  char* buffer = NULL;
  int flength = 0;

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
}

// Funzione che controlla se una determinata stringa
// inizia con un prefisso, const char* pre, passato in input
// la stringa da controllare viene presa in input da un'altra
// variabile chiamata const char* str
// Ritorna in output 1 se la condizione è vera
// 0 altrimenti

int startsWithPre(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

// Funzione utilizzata per mostrare il contenuto di un archivio
// Prende in inpute il path dell'archivio da analizzare, successivamente
// crea un file con il path passato in input, lo analizza, e stampa a video i file trovati
// La presenza di un file all'interno di un archivio viene rilevata in questo modo:
//  Viene analizzata ogni riga di del file, se quella riga inizia con il separatore
//  FILE= allora quella riga conterrà la path di un file da visualizzare in output

void showBackupContent(char* archive) {
  char workingdir[PATH_MAX_LENGTH];
  getcwd(workingdir, PATH_MAX_LENGTH);
  strcat(workingdir, "/");
  strcat(workingdir, archive);

  const char* pref = "FILE=";

  char buff[PATH_MAX_LENGTH];
  char ch;
  unsigned long chars;

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
}

// Questo metodo viene utilizzato per creare le sottocartelle
// ricorsivamente quando viene estratto un archivio
// questa funzione è stata ricavata da Internet.

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

  char ch;
  unsigned long chars;

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
    // 

    fclose(archivetoshow);
    FILE* archivetoshow = fopen(workingdir, "r");
    FILE* temp = NULL;

    // Continua a leggere il file fino a quando non arriva alla fine
    // ad ogni ciclo apre il file destinatario per esportare il file
    // dal file di backup alla cartella di destinazione

    while(fgets(buff2, PATH_MAX_LENGTH, archivetoshow) != NULL) {
      if(startsWithPre(pref, buff2) == 1) {
        printf("%s", buff2);
        char *token;
        char *search = "=";

        token = strtok(buff2, search);
        token = strtok(NULL, search);

        temp = fopen(token, "a+");
        printf("%s\n", token );
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
}