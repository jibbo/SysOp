#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <syslog.h>

typedef struct {
    char * path;
    FILE * file;
    char * line;
    size_t size;
    ssize_t read;
} file;

void scanworkingdir(char*, int);
void printhelp();
void showparametererror(int optvalue, int flag, int parameter);
void createarchive(char* filename);
bool filealredyexists(const char*);

int main(int argc, char **argv) {
  int c_flag = 0;
  int x_flag = 0;
  int t_flag = 0;
  int f_flag = 0;

  char *dirvalue = NULL;
  int index;
  int c;
     
  opterr = 0;
  
  while ((c = getopt (argc, argv, "f:cxt")) != -1) {
    switch (c) {
      case 'f':
        f_flag = 1;
        dirvalue = optarg;
        //printf("Creating an archive from the directory: %s\n", dirvalue);
        scanworkingdir(dirvalue, 2);
        break;

      case 'c':
        c_flag = 1;    
        showparametererror(optopt, f_flag, c);
        break;

      case 'x':
        x_flag = 1;
        showparametererror(optopt, f_flag, c);
        break;

      case 't':
        t_flag = 1;
        showparametererror(optopt, f_flag, c);
        break;

      case '?':
        if (optopt == 'f') {
          fprintf (stderr, "Option -%c requires the archive as an argument\n", optopt);
        } else if (isprint (optopt)) {
          fprintf (stderr, "Unknown option '-%c'\n", optopt);
          printhelp();
        } else {
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;

      default:
        abort ();
    }
  }
  return 0;
}

void printhelp() {
  printf("Usage: \n");
  printf("\t -f to create or extract an archive");
  printf("\n\t -c to create a new archive");
  printf("\n\t -x to extract an archive in the current directory");
  printf("\n\t -t to display the content of an archive\n");
}

void showparametererror(int optvalue, int flag, int parameter) {
  printf("optvalue: %c\n", optvalue);
  printf("flag: %i\n", flag);
  printf("parameter: %c\n", parameter);

  if(optvalue = 'f' && !flag) {
    if(parameter == 'c') {
      printf("You must use the -f flag to specify the archive you want to create\n");
    } else if(parameter == 'x') {
      printf("You must use the -f flag to specify the archive you want to extract\n");
    } else if(parameter == 't') {
      printf("You must use the -f flag to specify the archive you want to analyze\n");
    }
  }
}

void createarchive(char* filename) {

}

bool filealredyexists(const char* filename) {
  FILE * file = fopen(filename, "r");
  if(file) {
    fclose(file);
    return true;
  }
  return false;
}

void scanworkingdir(char* path, int indent) {
  struct dirent* direntry;
  DIR* dir;   // file descriptor usato per la gestione del directory stream
  struct stat stbuf;
  int is_dir;
  int access_err;

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

    // Gestisce l'indentazione della stampa delle directory
    int i = 0;
    while(i++ < indent) { 
      printf("  ");
    }

    is_dir = ((stbuf.st_mode & S_IFMT) == S_IFDIR);
    if (is_dir) {
      printf ("%s/\n" , direntry -> d_name );
      scanworkingdir(concat, indent + 1);
    } else {
      printf ("%s\n" , direntry -> d_name);
    }
    free(concat);
  }

  // Chiude il directory stream.
  // La funzione restituisce 0 in caso di successo e -1 altrimenti, 
  closedir(dir);
}