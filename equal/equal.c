#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
#include "lib_equal.h"
*/

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
void diffBetweenFiles(char *, char *, int);
int are_equals(char *, char *);
int are_equals_directories(char *, char *);
int empty_directories(char *, char *);
void printIndented(char *, int);

#endif

// ---------------------------------------------------------------------------------------------------------------------------------


//#include "lib_equal.h"

void printIndented(char * str, int indent_limit) {
    int indent_tab;
    for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf("  "); }
        printf("%s\n", str);
}

void diffBetweenFiles(char * path1, char * path2, int indent_limit) {

    str_file * file1;
    str_file * file2;
    file1 = (str_file *) malloc(sizeof(str_file));
    file2 = (str_file *) malloc(sizeof(str_file));

    // Copio i due percorsi passati per parametro nel campo path delle rispettive strutture precedentemente dichiarate..
    file1->path = (char*)malloc(sizeof(char) * strlen(path1));
    strcpy(file1->path, path1);
    file2->path = (char*)malloc(sizeof(char) * strlen(path2));
    strcpy(file2->path, path2);

    if ((file1->file = fopen(file1->path,"rb")) == NULL) {
        // Impossibile aprire path1
        perror(file1->path);
        exit(EXIT_FAILURE);
    }
    
    if ((file2->file = fopen(file2->path,"rb")) == NULL)  {
        // Impossibile aprire path1
        perror(file2->path);
        exit(EXIT_FAILURE);
    }

    /*
    printf("file1->path: %s\n", file1->path);
    printf("file1->size: %d bytes\n", file1->size);
    printf("file2->path: %s\n", file2->path);
    printf("file2->size: %d bytes\n", file2->size);
    printf("\n-----------------------------------");
    */

    // Read the entire file1 and file2
    file1->read = fread(file1->line, file1->size, 1, file1->file);
    file2->read = fread(file2->line, file2->size, 1, file2->file);

    /*
    printf("\nfile1:\n%s\n----------------------------", file1->line);
    printf("\nfile2:\n%s\n----------------------------", file2->line);
    */

    if(strcmp(file1->line , file2->line) == 0) {
        printIndented("I files sono uguali!", indent_limit);
    } else {
        printIndented("I files sono diversi!", indent_limit);
    }

    fclose(file1->file);
    fclose(file2->file);
}

int empty_directories(char * completePath1, char * completePath2) {

    struct dirent *dirent;
    DIR *dir;

    if ((dir = opendir(completePath1)) == NULL) {
        printf("Cannot open %s\n", completePath1);
        syslog(LOG_INFO, "Cannot open %s. Terminated.", completePath1);
        exit(EXIT_FAILURE);
    }

    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0) {
            closedir(dir);
            return 0;
        }
    }
    closedir(dir);

    if ((dir = opendir(completePath2)) == NULL) {
        printf("Cannot open %s\n", completePath2);
        syslog(LOG_INFO, "Cannot open %s. Terminated.", completePath2);
        exit(EXIT_FAILURE);
    }

    while ((dirent = readdir(dir)) != NULL) {
        if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0) {
            closedir(dir);
            return 0;
        }
    }
    closedir(dir);
    printf("vuote\n");
    return 1;
}

int are_equals_directories(char * path1, char * path2) {

    struct dirent *dirent1, *dirent2;
    DIR *dir1, *dir2;
    struct stat stat1, stat2;
    int is_dir1, is_dir2;
    int equal_dir = 0;

    // Verifico se entrambe le directories sono vuote..
    if (empty_directories(path1, path2) == 1) {
        //printf("%s e %s sono vuote\n", path1, path2 );
        return 1;
    }

    if ((dir1 = opendir(path1)) == NULL) {
        printf("Cannot open %s\n", path1);
        syslog(LOG_INFO, "Cannot open %s. Terminated.", path1);
        exit(EXIT_FAILURE);
    }

    while ((dirent1 = readdir(dir1)) != NULL) {

        if (strcmp(dirent1->d_name, ".") != 0 && strcmp(dirent1->d_name, "..") != 0) {

            // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
            // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
            size_t length1 = strlen(path1) + strlen(dirent1->d_name) + 2;
            char * completePath1 = (char *) malloc(sizeof(char) * length1);
            snprintf(completePath1, length1, "%s/%s", path1, dirent1->d_name);

            // Valuto se si tratta di una sub-directory..
            stat(completePath1, &stat1);
            is_dir1 = ((stat1.st_mode & S_IFMT) == S_IFDIR);

            if ((dir2 = opendir(path2)) == NULL) { 
                printf("Cannot open %s\n", path2);
                syslog(LOG_INFO, "Cannot open %s. Terminated.", path2);
                exit(EXIT_FAILURE);
            }

            equal_dir = 0;
            //printf("%d con %s\n", equal_dir, dirent1->d_name );

            // Scorro tutta la seconda directory e controllo se il file è presente
            while ( !equal_dir && (dirent2 = readdir(dir2)) != NULL) {

                // Verifico che non siano le entries di default delle directories..
                if (strcmp(dirent2->d_name, ".") != 0 && strcmp(dirent2->d_name, "..") != 0) {

                    //printf("analizzando %s\n", dirent2->d_name);
                    // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
                    // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
                    size_t length2 = strlen(path2) + strlen(dirent2->d_name) + 2;
                    char * completePath2 = (char *) malloc(sizeof(char) * length2);
                    snprintf(completePath2, length2, "%s/%s", path2, dirent2->d_name);

                    // Se i files o le directories hanno lo stesso nome..
                    if(strcmp(dirent1->d_name, dirent2->d_name) == 0) {

                        stat(completePath2, &stat2);
                        is_dir2 = ((stat2.st_mode & S_IFMT) == S_IFDIR);

                        if (is_dir1 && is_dir2) {

                            // Procedo in maniera ricorsiva sulle sub-directory aventi lo stesso nome in entrambi i path..
                            equal_dir = equal_dir || are_equals_directories(completePath1, completePath2);
                            //printf("equal_dif = %d , dopo equal_directories  tra %s e %s\n", equal_dir, completePath1, completePath2 );
                        }
                        else {
                            equal_dir = equal_dir || are_equals(completePath1, completePath2);
                        }

                        //printf("%d con %s vs %s\n", equal_dir, completePath1, completePath2);
                        free(completePath2);
                    }
                }
                // else { equal_dir = 1; }
            }

            if( equal_dir == 0 ) { /* printf("diverse..- %s\n", completePath1); */ return equal_dir; }

            free(completePath1);
            closedir(dir2);
        }
    }
    closedir(dir1);
    return equal_dir;
}

int are_equals(char * file_path1, char * file_path2) {

    str_file * file1;
    str_file * file2;
    file1 = (str_file *) malloc(sizeof(str_file));
    file2 = (str_file *) malloc(sizeof(str_file));

    // Copio i due percorsi passati per parametro nel campo path delle rispettive strutture precedentemente dichiarate..
    file1->path = (char*)malloc(sizeof(char) * strlen(file_path1));
    strcpy(file1->path, file_path1);
    file2->path = (char*)malloc(sizeof(char) * strlen(file_path2));
    strcpy(file2->path, file_path2);

    if ((file1->file = fopen(file1->path,"rb")) == NULL)  {
        // Impossibile aprire path1
        perror(file1->path);
        exit(EXIT_FAILURE);
    }
    else if ((file2->file = fopen(file2->path,"rb")) == NULL)  {
        // Impossibile aprire path2
        perror(file2->path);
        exit(EXIT_FAILURE);
    }
    else
    {
        // Return value
        int ris = 0;

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

        // Read the entire file1 and file2
        file1->read = fread(file1->line, file1->size, 1, file1->file);
        file2->read = fread(file2->line, file2->size, 1, file2->file);

        // Confronta se i files sono identici
        ris = strcmp(file1->line , file2->line) == 0 ? 1 : 0;

        fclose(file1->file);
        fclose(file2->file);
        free(file1);
        free(file2);

        // Ritorna il risultato del confronto: 1 se i files sono identici.
        return ris;
    }
}

void dirwalk(char * path1, char * path2, int indent_limit, char symb)
{
    struct dirent *dirent1, *dirent2;
    DIR *dir1, *dir2;
    struct stat stat1, stat2;
    int is_dir1, is_dir2;

    int indent_tab = 0; // serve per dare una giusta indentazione al momento della stampa in console.
    int found = 0;      // serve per vedere se il file1 è stato trovato all'interno del path2.

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

            // Setto il flag a zero per capire che finora non c'è alcun file uguale..
            found = 0;

            // Apro la directory rappresentata dal path2..
            if ((dir2 = opendir(path2)) == NULL) { 
                printf("Cannot open %s\n", path2);
                syslog(LOG_INFO, "Cannot open %s.\nTerminated.\n", path2);
                exit(EXIT_FAILURE);
            }

            // Valuto se il path1 rappresenta una directory..
            stat(completePath1, &stat1);
            is_dir1 = ((stat1.st_mode & S_IFMT) == S_IFDIR);

            // Scorro tutta la seconda directory e controllo se il file è presente
            while ((dirent2 = readdir(dir2)) != NULL) {

                // Verifico che non siano le entries di default delle directories..
                if (strcmp(dirent2->d_name, ".") != 0 && strcmp(dirent2->d_name, "..") != 0) {

                    // Se i files o le directories hanno lo stesso nome..

                    //printf("dirent1 = %s , dirent2 = %s\n", dirent1->d_name, dirent2->d_name);

                    if(strcmp(dirent1->d_name, dirent2->d_name) == 0) {

                        // Setto il flag di found a 1 così riconosco che due files o directories hanno lo stesso nome..
                        found = 1;

                        // Sto visualizzando il file percorsi la prima volta..
                        if(symb == '+') {

                            // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
                            // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
                            size_t length2 = strlen(path2) + strlen(dirent2->d_name) + 2;
                            char * completePath2 = (char *) malloc(sizeof(char) * length2);
                            snprintf(completePath2, length2, "%s/%s", path2, dirent2->d_name);

                            stat(completePath2, &stat2);
                            is_dir2 = ((stat2.st_mode & S_IFMT) == S_IFDIR);

                            //printf("analizzando %s con %s \n", completePath1, completePath2 );

                            if (is_dir1 && is_dir2) {

                                if( are_equals_directories(completePath1, completePath2) != 1) {

                                    // Cartelle con contenuto diverso..
                                    for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf("  "); }
                                    printf ("Differences between folders: %s - %s\n", completePath1, completePath2);

                                    // Procedo in maniera ricorsiva sulle sub-directory aventi lo stesso nome in entrambi i path..
                                    dirwalk(completePath1, completePath2, indent_limit + 1, '+');
                                    dirwalk(completePath2, completePath1, indent_limit + 1, '-');
                                    printf("------------------------------\n");
                                }
                                
                            } else {

                                if(are_equals(completePath1, completePath2) != 1) {

                                    // File con contenuto diverso..
                                    for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf("  "); }
                                    printf ("Differences between files: %s - %s\n", completePath1, completePath2);
                                        //printf("Differences in %s\n", dirent1->d_name);
                                        //printf("------------------------------\n");
                                    diffBetweenFiles(completePath1, completePath2, indent_limit + 1);
                                    //diffBetweenFiles(file2, file1, indent_limit + 1);
                                    printf("------------------------------\n");
                                }
                            }
                            free(completePath2);    
                        }
                    }
                }
            }

            // A seconda che il file sia presente nel path2 stampo una + o -
            if(!found) {
                for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf("  "); }
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

    // Apro il file di log..
    openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);
    syslog(LOG_INFO, "Utility started: %s", argv[0]);
    syslog(LOG_INFO, "\nFirst path passed: %s", argv[1]);
    syslog(LOG_INFO, "\nSecond path passsed: %s", argv[2]);

    // Dichiaro due strutture di tipo stat che mi serviranno per estrarre informazioni relative ai due percorsi dei files..
    struct stat stbuf1;
    struct stat stbuf2;

    // Gestisce la tabulazione per una stampa a console ordinata..
    int indent = 1;

    // Valuto se i due percorsi hanno particolari restrizioni di accesso..
    // In caso di errore esco e aggiungo un nuovo messaggio al file di log..
    int access_err1 = (stat(argv[1], &stbuf1) == -1);
    int access_err2 = (stat(argv[2], &stbuf2) == -1);
    if ( access_err1 ) {
        fprintf(stderr, "Error: can't access %s\n", argv[1]);
        syslog(LOG_ERR, "Error: can't access %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    if ( access_err2) {
        fprintf(stderr, "Error: can't access %s\n", argv[2]);
        syslog(LOG_ERR, "Error: can't access %s", argv[2]);
        exit(EXIT_FAILURE);
    }

    // Valuto se i due percorsi sono delle directories..
    int is_dir_1 = ((stbuf1.st_mode & S_IFMT) == S_IFDIR);
    int is_dir_2 = ((stbuf2.st_mode & S_IFMT) == S_IFDIR);
    if(is_dir_1 && is_dir_2) {

        // I due percorsi rappresentano entrambi due directories..
        syslog(LOG_INFO, "Both paths are directories.");

        // Differences from path1 to path2..
        dirwalk(argv[1], argv[2], indent, '+');

        // Differences from path2 to path1..
        dirwalk(argv[2], argv[1], indent, '-');

        // Separator..
        printf("\n--------------------------------------------------------------------------------------------------\n");
    }
    else if(!is_dir_1 && !is_dir_2) {

        // I due percorsi rappresentano entrambi due files..
        syslog(LOG_INFO, "Both paths are files.");
        diffBetweenFiles(argv[1], argv[2], indent);
    }
    else {

        // I due percorsi non possono essere comparati..
        syslog(LOG_INFO, "Paths cannot be compared. Terminated.");
        printf("You cannot compare two different kind of files (one directory one file)!");
        exit(EXIT_FAILURE);
    }

    // Termino con successo..
    syslog(LOG_INFO, "Exit success. Terminated.");
    exit(EXIT_SUCCESS);
}