#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
#include "lib_equal.h"
#include "struct_stack.h"
*/



// ---------------------------------------------------------------------------------------------------------------------------------
#ifndef STRUCT_STACK_H

#define STRUCT_STACK_H
#define DIM 1000

enum boolean { TRUE, FALSE };

typedef struct {
  int indice;
  int* elem;
} stack;

void init(stack * );
int emptyp (const stack *);
int fullp (const stack *);
int push (int, stack *);
int top (int *, const stack *);
int pop (stack *);

#endif

// ---------------------------------------------------------------------------------------------------------------------------------

//#include "struct_stack.h"

int emptyp (const stack * s) {
  return (s->indice==0);
}

int fullp (const stack * s) {
  return (s->indice==DIM);
}

void init (stack * s) {
  s->indice = 0;
}

int top (int * n, const stack * s) {
    if (!emptyp(s)) {
        *n = s->elem[s->indice-1];
        return 1;
    }
    return -1;
}

int push (int n, stack * s) {
  if (!fullp(s)) {
    s->elem[s->indice++]=n;
    return 1;
  }
  return -1;
}
    
int pop (stack * s) {
  if (!emptyp(s)) {
    s->indice--;
    return 1;
  }
  return -1;
}

// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef EQUAL_H

#define EQUAL_H

#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <syslog.h>

typedef struct {
    char * path;
    FILE * file;
    char * line;
    size_t size;
    ssize_t read;
} str_file;

void dirwalk(char *, char *, int, char);
void diffBetweenFiles(str_file *, str_file *);

#endif

// ---------------------------------------------------------------------------------------------------------------------------------


#include "lib_equal.h"

void diffBetweenFiles(str_file * file1, str_file * file2) {
    // Files case..
    //printf("Both paths are files!\n");

    if ((file1->file = fopen(file1->path,"rb")) == NULL)  {
        // Impossibile aprire path1
        perror(file1->path);
        exit(EXIT_FAILURE);
    }
    else if ((file2->file = fopen(file2->path,"rb")) == NULL)  {
        // Impossibile aprire path1
        perror(file2->path);
        exit(EXIT_FAILURE);
    }
    else
    {
        // Obtain file1 dimension in bytes
        fseek(file1->file, 0, SEEK_END);
        file1->size = ftell(file1->file);
        file1->line = (char *) malloc(file1->size);
        fseek(file1->file, 0, SEEK_SET);  // Return to the top of the file

        // Obtain file2 dimension in bytes
        fseek(file2->file, 0, SEEK_END);
        file2->size = ftell(file2->file);
        file2->line = (char *) malloc(file2->size);
        fseek(file2->file, 0, SEEK_SET);  // Return to the top of the file

        printf("file1->path: %s\n", file1->path);
        printf("file1->size: %d bytes\n", file1->size);
        printf("file2->path: %s\n", file2->path);
        printf("file2->size: %d bytes\n", file2->size);
        printf("\n-----------------------------------");
        
        // Read the entire file1 and file2
        file1->read = fread(file1->line, file1->size, 1, file1->file);
        file2->read = fread(file2->line, file2->size, 1, file2->file);

        printf("\nfile1:\n%s\n----------------------------", file1->line);
        printf("\nfile2:\n%s\n----------------------------", file2->line);

        printf("\n\n");
        if(strcmp(file1->line , file2->line)) {
            printf("I files sono diversi!\n");
        } else {
            printf("I files sono uguali!\n");
        }

        free(file1->line);
        free(file2->line);
        fclose(file1->file);
        fclose(file2->file);
    }
}

void dirwalk(char * path1, char * path2, int indent, char symb)
{
    struct dirent *dirent1, *dirent2;
    DIR *dir1, *dir2;
    struct stat stat1, stat2;
    int is_dir1, is_dir2;

    int indent_tab = 0;         // serve per dare una giusta indentazione al momento della stampa in console.
    int i = 0;                  // tiene conto di quanti tab sono stati stampati
    int found = 0;              // serve per vedere se il file1 è stato trovato all'interno del path2.

    // La funzione opendir apre un directory stream.
    // Restituisce un puntatore ad un oggetto di tipo DIR in caso di successo e NULL in caso di errore.
    // Inoltre posiziona lo stream sulla prima voce contenuta nella directory.

    if ((dir1 = opendir(path1)) == NULL) { 
        printf("Cannot open %s\n", path1);
        syslog(LOG_INFO, "Cannot open %s.\nTerminated.\n", path1);
        exit(EXIT_FAILURE);
    }

    // Cicla tutti i files e directory presenti all'interno del path specificato.
    // La funzione readdir legge la voce corrente nella directory, posizionandosi sulla voce successiva.
    // Restituisce un puntatore al directory stream in caso di successo e NULL in caso di errore.
    while ((dirent1 = readdir(dir1)) != NULL) {

        if (strcmp(dirent1->d_name, ".") != 0 && strcmp(dirent1->d_name, "..") != 0) {

            // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
            // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
            size_t length1 = strlen(path1) + strlen(dirent1->d_name) + 2;
            char * completePath1 = (char *) malloc(sizeof(char) * length1);
            snprintf(completePath1, length1, "%s/%s", path1, dirent1->d_name);

            //printf("analizzando %s\n\n", dirent1->d_name);

            found = 0;

            // Valuto se si tratta di una sub-directory..
            stat(completePath1, &stat1);
            is_dir1 = ((stat1.st_mode & S_IFMT) == S_IFDIR);

            if ((dir2 = opendir(path2)) == NULL) { 
                printf("Cannot open %s\n", path2);
                syslog(LOG_INFO, "Cannot open %s.\nTerminated.\n", path2);
                exit(EXIT_FAILURE);
            }

            // Scorro tutta la seconda directory e controllo se il file è presente
            while ((dirent2 = readdir(dir2)) != NULL) {

                // Verifico che non siano le entries di default delle directories..
                if (strcmp(dirent2->d_name, ".") != 0 && strcmp(dirent2->d_name, "..") != 0) {

                    // Se i files o le directories hanno lo stesso nome..

                    //printf("dirent1 = %s , dirent2 = %s\n", dirent1->d_name, dirent2->d_name);

                    if(strcmp(dirent1->d_name, dirent2->d_name) == 0) {

                        // Setto il flag di found a 1 così riconosco che due files o directories hanno lo stesso nome..
                        found = 1;

                        // Sto visualizzando il file per la prima volta..
                        if(symb == '+') {

                            // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
                            // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
                            size_t length2 = strlen(path2) + strlen(dirent2->d_name) + 2;
                            char * completePath2 = (char *) malloc(sizeof(char) * length2);
                            snprintf(completePath2, length2, "%s/%s", path2, dirent2->d_name);

                            stat(completePath2, &stat2);
                            is_dir2 = ((stat2.st_mode & S_IFMT) == S_IFDIR);

                            printf("Differences in %s\n", dirent1->d_name);
                            printf("------------------------------\n");

                            if (is_dir1 && is_dir2) {
                                printf("\n");

                                //dirwalk(completePath1, completePath2, indent + 1, '+');
                                //dirwalk(completePath2, completePath1, indent + 1, '-');
                            } else {
                                //diffBetweenFiles(path1, path2);
                                //diffBetweenFiles(path2, path1);
                            }

                            printf("------------------------------\n");
                            free(completePath2);
                        }
                    }
                }
            }

            // A seconda che il file sia presente nel path2 stampo una + o -
            if(!found) {
                for(i = 0; i < indent_tab; i++) { printf("  "); }
                printf ("%c %s\n", symb, dirent1->d_name);
            }

            free(completePath1);
            closedir(dir2);
        }
    }

    // Chiude il directory stream.
    // La funzione restituisce 0 in caso di successo e -1 altrimenti, 
    closedir(dir1);
}

// ---------------------------------------------------------------------------------------------------------------------------------

/*
    #include <sys/types.h>
    #include <dirent.h>
    DIR * opendir(const char *dirname)
    Apre un directory stream.
    La funzione restituisce un puntatore al directory stream in caso di successo e NULL per un errore,
    nel qual caso errno assumer`
    a i valori EACCES, EMFILE, ENFILE, ENOENT, ENOMEM e ENOTDIR.

    #include <sys/types.h>
    #include <dirent.h>
    struct dirent *readdir(DIR *dir)
    Legge una voce dal directory stream.
    La funzione restituisce il puntatore alla struttura contenente i dati in caso di successo e NULL
    altrimenti, in caso di directory stream non valido errno assumer`
    a il valore EBADF, il valore NULL
    viene restituito anche quando si raggiunge la fine dello stream.

    struct dirent {
        ino_t d_ino ;
        off_t d_off ;
        unsigned short int d_reclen ;
        unsigned char d_type ;
        char d_name [256];
    };
*/

int main (int argc, char **argv) {

    // Start with project
    // .....................................................................................

    str_file * file1;
    str_file * file2;

    // Apro il file di log..
    openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);
    syslog(LOG_INFO, "\nUtility started: %s\n", argv[0]);

    file1 = (str_file *) malloc(sizeof(str_file));
    file2 = (str_file *) malloc(sizeof(str_file));

    // Copio i due percorsi passati per parametro nel campo path delle rispettive strutture precedentemente dichiarate..
    file1->path = (char*)malloc(sizeof(char) * strlen(argv[1]));
    strcpy(file1->path, argv[1]);
    file2->path = (char*)malloc(sizeof(char) * strlen(argv[2]));
    strcpy(file2->path, argv[2]);

    syslog(LOG_INFO, "\nPath 1: %s\n", file1->path);
    syslog(LOG_INFO, "\nPath 2: %s\n", file2->path);

    printf("PATH1:  %s\n", file1->path);
    printf("PATH2:  %s\n", file2->path);
    printf("\n\n");

    // Dichiaro due strutture di tipo stat che mi serviranno per estrarre informazioni relative ai due percorsi dei files..
    struct stat stbuf1;
    struct stat stbuf2;

    // Valuto se i due percorsi hanno particolari restrizioni di accesso..
    // In caso di errore esco e aggiungo un nuovo messaggio al file di log..
    int access_err1 = (stat(file1->path, &stbuf1) == -1);
    int access_err2 = (stat(file2->path, &stbuf2) == -1);
    if ( access_err1 ) {
        fprintf(stderr, "Error: can't access %s\n", file1->path);
        syslog(LOG_ERR, "Error: can't access %s\n", file1->path);
        exit(EXIT_FAILURE);
    }
    if ( access_err2) {
        fprintf(stderr, "Error: can't access %s\n", file2->path);
        syslog(LOG_ERR, "Error: can't access %s\n", file2->path);
        exit(EXIT_FAILURE);
    }

    // Valuto se i due percorsi sono delle directories..
    int is_dir_1 = ((stbuf1.st_mode & S_IFMT) == S_IFDIR);
    int is_dir_2 = ((stbuf2.st_mode & S_IFMT) == S_IFDIR);

    if(is_dir_1 && is_dir_2) {

        // I due percorsi rappresentano entrambi due directories..
        syslog(LOG_INFO, "Both paths are directories.\nTerminated.\n");

        int indent = 1;

        // Differences from path1 to path2
        dirwalk(file1->path, file2->path, indent, '+');

        // Differences from path2 to path1
        dirwalk(file2->path, file1->path, indent, '-');

        printf("\n--------------------------------------------------------------------------------------------------\n");
    }
    else if(!is_dir_1 && !is_dir_2) {

        // I due percorsi rappresentano entrambi due files..
        syslog(LOG_INFO, "Both paths are files.\nTerminated.\n");
        diffBetweenFiles(file1, file2);
    }
    else {

        // I due percorsi non possono essere comparati..
        syslog(LOG_INFO, "Paths cannot be compared.\nTerminated.\n");
        printf("You cannot compare two different kind of files (one directory one file)!\n");
        exit(EXIT_FAILURE);
    }

    // Libero la memoria allocata dalle strutture files
    free(file1);
    free(file2);
    exit(EXIT_SUCCESS);
}