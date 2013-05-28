#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>

void scanworkingdir(char*, int);
void createarchive(char* filename);
void printhelp();

int main(int argc, char **argv) {
  int c_flag = 0;
  int x_flag = 0;
  int t_flag = 0;

  char *dirvalue = NULL;
  int index;
  int c;
     
  opterr = 0;
  
  while ((c = getopt (argc, argv, "f:cxt")) != -1) {
    switch (c) {
      // In questo caso l'utility deve creare (od estrarre) l'archivio che viene passato come parametro
      case 'f':
        dirvalue = optarg;
        printf("Creating an archive from the directory: %s\n", dirvalue);
        scanworkingdir(dirvalue, 2);
        break;

      // In questo caso l'utility deve creare l'archivio
      case 'c':
        c_flag = 1;
        break;

      // In questo caso l'utility deve estrarre l'archivio nella directory corrente
      case 'x':
        x_flag = 1;
        break;

      // In questo caso l'utility deve visualizzare un elenco del contenuto dell'archivio
      case 't':
        t_flag = 1;
        break;

      // 
      case '?':
        if (optopt == 'f') {
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        } else if (isprint (optopt)) {
          fprintf (stderr, "Unknown option `-%c'\n", optopt);
          printhelp();
        } else {
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;

      default:
        abort ();
    }
  }

  printf ("\nc = %d, x = %d, t = %d, dirvalue = %s\n",  c_flag, x_flag, t_flag, dirvalue);

  for (index = optind; index < argc; index++)
   printf ("Non-option argument %s\n", argv[index]);

  return 0;
}

void printhelp() {
  printf("Usage: \n");
  printf("\t -f to create or extract an archive");
  printf("\n\t -c to create a new archive");
  printf("\n\t -x to extract an archive in the current directory");
  printf("\n\t -t to displat the content of an archive\n");
}

void createarchive(char* filename) {

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