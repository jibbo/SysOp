#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <limits.h>
#include <string.h>

#define PATH_MAX_LENGTH 256

typedef struct {
  char path[PATH_MAX_LENGTH];
  FILE* file;
  char* line;
  size_t size;
  ssize_t read;
} str_file;

FILE* backup;

char filevalue[PATH_MAX_LENGTH];
char dirvalue[PATH_MAX_LENGTH];
char cwd[PATH_MAX_LENGTH];
char subpath[PATH_MAX_LENGTH];

int f_flag = 0;
int c_flag = 0;
int x_flag = 0;
int t_flag = 0;

void manage();
void printHelp();
void makeBackup(char* path);
void showBackupContent(char* archive);

int main(int argc, char **argv) {
  int c;
  opterr = 0;
  
  while ((c = getopt (argc, argv, "f:cxt")) != -1) {
    switch (c) {
      case 'f':
        f_flag = 1;
        strcpy(filevalue, optarg);
        //strcpy(dirvalue, argv[optind]); 
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
    // Estrae l'archivio
    printf("Estraggo l'archivio: %s\n", filevalue);

  } else if(t_flag == 1 && (filevalue == NULL && dirvalue == NULL)) {
    printf("You must use the -f flag to specify the archive you want to analyze\n");

  } else if ((t_flag == 1 && f_flag == 1) && filevalue != NULL) {
    // Mostra il contenuto dell'archivio
    showBackupContent(filevalue);
  }
}

void printHelp() {
  printf("Usage: mkbkp [-c] [-x] [-t] [-f]\n");
  printf("\t -f to create or extract an archive");
  printf("\n\t -c to create a new archive");
  printf("\n\t -x to extract an archive in the current directory");
  printf("\n\t -t to display the content of an archive\n");
}

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
      printf("scanworkingdir: can't open %s\n", path);
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

    // se è una directory
    if (is_dir) {
      strcpy(subpath, path);
      strcat(subpath, "/");
      strcat(subpath, direntry -> d_name);

      fprintf(backup, "\nDIR=");
      fprintf(backup, "%s", subpath);
      fprintf(backup, "\n");

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
      fprintf(backup, "FILE=");
      fprintf(backup, "%s", temp -> path);
      fprintf(backup, "\n");

      // Scrive il file appena letto all'interno dell'archivio
      fwrite(temp->line, 1, temp -> size, backup);
      fclose(temp -> file);

      free(temp -> line);
      free(temp);
    }

    fclose(backup);
  }

  closedir(dir);
}

void showBackupContent(char* archive) {
  char workingdir[PATH_MAX_LENGTH];
  getcwd(workingdir, PATH_MAX_LENGTH);
  strcat(workingdir, "/");
  strcat(workingdir, archive);

  char buff[PATH_MAX_LENGTH];

  FILE* archivetoshow = fopen(workingdir, "r");
  if(archivetoshow == NULL) {
    perror(workingdir);
    exit(EXIT_FAILURE);
  } else {
    while(fgets(buff, PATH_MAX_LENGTH, archivetoshow) != NULL) {
      printf("%s\n", buff);
    }
  }

  fclose(archivetoshow);
}