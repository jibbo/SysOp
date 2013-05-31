
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib_equal.h"

void printIndented(char * str, int indent_limit) {
    int indent_tab;
    for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf("%s",TAB); }
        printf("%s\n", str);
}

void diffBetweenFiles(char * path1, char * path2, int indent_limit) {

    // Controllo prima se i file sono diversi e poi calcolo le differenze..
    if(!are_equals(path1, path2)) {

        str_file * file1;
        file1 = (str_file *) malloc(sizeof(str_file));

        // test
        printf("%s\n", path1);
        printf("%s\n", path2);

        // Apertura primo file..
        file1->file = fopen(path1,"r");
        if (file1->file == NULL) {
            // Impossibile aprire path1
            perror(path1);
            free(file1);
            exit(EXIT_FAILURE);
        }

        str_file * file2;
        file2 = (str_file *) malloc(sizeof(str_file));

        // Apertura secondo file..
        file2->file = fopen(path2,"r");
        if (file2->file == NULL)  {
            // Impossibile aprire path2
            perror(file2->path);
            free(file2);
            fclose(file1->file);
            exit(EXIT_FAILURE);
        }

        // Ottengo la dimensione del file1 in bytes e ritorno all'inizio del file..
        fseek(file1->file, 0, SEEK_END);
        file1->size = ftell(file1->file);
        fseek(file1->file, 0, SEEK_SET);

        // Ottengo la dimensione del file1 in bytes e ritorno all'inizio del file..
        fseek(file2->file, 0, SEEK_END);
        file2->size = ftell(file2->file);
        fseek(file2->file, 0, SEEK_SET);

        // Buffer per memorizzare una porzione del file1 e file2..
        char buf_file1[MAX_BUF_SIZE];
        char buf_file2[MAX_BUF_SIZE];

        // File con contenuto diverso..
        int indent_tab;
        for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf(TAB); }
        printf ("Differences between files: %s - %s\n", path1, path2);

        // Muovo i buffers utilizzati per il controllo in maniera parallela..
        while(fgets(buf_file1, MAX_BUF_SIZE, file1->file) != NULL && fgets(buf_file2, MAX_BUF_SIZE, file2->file)) {
            if(strcmp(buf_file1, buf_file2) != 0) {

                // E' stata riscontrata una differenza tra il contenuto letto nel primo file e quello letto nel secondo file.
                // Stampo le differenze.
                for(indent_tab = 0; indent_tab < indent_limit + 1; indent_tab++) { printf(TAB); }
                printf("%c %s", PLUS_SYMB, buf_file1);
                for(indent_tab = 0; indent_tab < indent_limit + 1; indent_tab++) { printf(TAB); }
                printf("%c %s", MINUS_SYMB, buf_file2);
            }
        }

        // Sarò arrivato alla fine dei uno dei due files..
        // Stampo la differenza solo di uno rispetto all'altro..

        // Leggo il contenuto RIMANENTE (se presente) del primo file e lo stampo..
        while(fgets(buf_file1, MAX_BUF_SIZE, file1->file) != NULL) {
            for(indent_tab = 0; indent_tab < indent_limit + 1; indent_tab++) { printf(TAB); }
            printf("%c %s", PLUS_SYMB, buf_file1);
        }

        // Controllo se la lettura è terminata perchè ho letto tutto il file oppure per errore.
        if(feof(file1->file)) {
            printf("finito file 1\n");
        }
        else if(ferror(file1->file)) {
            printf("error 1\n");
        }

        // Leggo il contenuto RIMANENTE (se presente) del secondo file e lo stampo..
        while(fgets(buf_file2, MAX_BUF_SIZE, file2->file) != NULL) {
            for(indent_tab = 0; indent_tab < indent_limit + 1; indent_tab++) { printf(TAB); }
            printf("%c %s", MINUS_SYMB, buf_file2);
        }

        // Controllo se la lettura è terminata perchè ho letto tutto il file oppure per errore.
        if(feof(file2->file)) {
            printf("finito file 2\n");
        }
        else if(ferror(file2->file)) {
            printf("error 2\n");
        }

        // Chiudo i due files..
        fclose(file1->file);
        fclose(file2->file);
    }
}

int empty_directory(char * path) {

    syslog(LOG_INFO, "Check if %s is an empty directory.", path);

    // Apro la cartella
    DIR *dir;
    dir = opendir(path);
    if (dir == NULL) {
        printf("Cannot open %s\n", path);
        syslog(LOG_ERR, "Cannot open %s. Terminated.", path);
        exit(EXIT_FAILURE);
    }

    // Leggo i contenuti della cartella
    struct dirent *dirent;
    dirent = readdir(dir);
    while (dirent != NULL) {
        if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0) {
            // Se sono presenti files o cartelle allora la cartella non è vuota..
            syslog(LOG_INFO, "%s is not an empty directory.", path);
            closedir(dir);
            return 0;
        }
    }

    // Chiudo la cartella e restituisco 1 perchè le cartelle sono vuote
    syslog(LOG_INFO, "%s is an empty directory.", path);
    closedir(dir);
    return 1;
}

int are_equals_directories(char path1[256] , char path2[256]) {


    printf("------------ SEMPRE are_equals_directories -----------------\n");


    // Verifico se entrambe le directories sono vuote..
    printf("Entrambe vuote? %d -> %s %s\n", empty_directory(path1) && empty_directory(path2), path1, path2);
    printf("Una vuota e una no? %d -> %s %s\n", empty_directory(path1) ^ empty_directory(path2), path1, path2);

    if (empty_directory(path1) && empty_directory(path2)) { return 1; }
    if (empty_directory(path1) ^ empty_directory(path2)) { return 0; }

    struct dirent *dirent1, *dirent2;
    DIR *dir1, *dir2;
    struct stat stat1, stat2;
    int is_dir1, is_dir2;
    int equal_dir = 1;

    char completePath1[256];    // Buffer che rappresenta il primo path
    char completePath2[256];    // Buffer che rappresenta il secondo path

    //strcpy(completePath1, path1);
    //strcpy(completePath2, path2);

    // Se si tratta di una directory aggiungo uno SLASH alla fine del path..
    // strcat(completePath1, SLASH);
    // strcat(completePath2, SLASH);

    printf("\nARRIVANO: %s e %s\n", path1, path2);

    dir1 = opendir(path1);
    if (dir1 == NULL) {
        printf("Cannot open %s\n", path1);
        syslog(LOG_ERR, "Cannot open %s. Terminated.", path1);
        exit(EXIT_FAILURE);
    }

    dirent1 = readdir(dir1);
    while (dirent1 != NULL) {

        if (strcmp(dirent1->d_name, ".") != 0 && strcmp(dirent1->d_name, "..") != 0) {
            
            strcpy(completePath1, path1);
            strcat(completePath1, SLASH);
            strcat(completePath1, dirent1->d_name);

            // Valuto se si tratta di una sub-directory..
            stat(completePath1, &stat1);
            is_dir1 = ((stat1.st_mode & S_IFMT) == S_IFDIR);

            // test

            printf("ARE_EQ dir1: %s is dir? %d\n", completePath1, is_dir1);
            /*
            printf("d_name: %s\n", dirent1->d_name);
            printf("def: %s%s\n", path1, dirent1->d_name);
            printf("\n");
            */

            printf("apro %s\n", path2);


            dir2 = opendir(path2);
            if (dir2 == NULL) { 
                printf("Cannot open %s\n", path2);
                syslog(LOG_ERR, "Cannot open %s. Terminated.", path2);
                closedir(dir1);
                exit(EXIT_FAILURE);
            }

            equal_dir = 1;

            // Scorro tutta la seconda directory e controllo se il file è presente
            dirent2 = readdir(dir2);
            while ( equal_dir && dirent2 != NULL) {

                equal_dir = 0;

                // Verifico che non siano le entries di default delle directories..
                if (strcmp(dirent2->d_name, ".") != 0 && strcmp(dirent2->d_name, "..") != 0) {


                    strcpy(completePath2, path2);
                    strcat(completePath2, SLASH);
                    strcat(completePath2, dirent2->d_name); /* test aggiungo d_name */

                    printf("confronto %s con %s \n", completePath1, completePath2 );

                    // Se i files o le directories hanno lo stesso nome..
                    if(strcmp(dirent1->d_name, dirent2->d_name) == 0) {

                        // Valuto se si tratta di una sub-directory..
                        stat(completePath2, &stat2);
                        is_dir2 = ((stat2.st_mode & S_IFMT) == S_IFDIR);

                        // test
                        printf("ARE_EQ dir2: %s is dir? %d\n", completePath2, is_dir2);
                        /*
                        printf("path2: %s\n", path2);
                        printf("d_name2: %s\n", dirent2->d_name);
                        printf("def2: %s%s\n", path2, dirent2->d_name);
                        printf("\n");
                        */

                        if (is_dir1 && is_dir2) {

                            printf("EQ_DIR FORSE %s e %s are directories\n", completePath1, completePath2);

                            printf("\nMANDO: %s e %s\n", completePath1, completePath2);

                            // Procedo in maniera ricorsiva sulle sub-directory aventi lo stesso nome in entrambi i path..
                            equal_dir = equal_dir && are_equals_directories(completePath1, completePath2);
                        }
                        else /* if (!is_dir1 && !is_dir2) */ {

                            printf("%s e %s are files\n", completePath1, completePath2);
                            printf("\nMANDO: %s e %s\n", completePath1, completePath2);

                            // Ritorna 1 se i files sono diversi..
                            equal_dir = equal_dir && are_equals(completePath1, completePath2);
                        } /*else {
                            //equal_dir = 0;
                            printf("qualcosa in piu\n");
                        }
                        */
                        printf("le cartelle finora sono %d\n", equal_dir );
                    }
                }
            }

            // Chiudo la seconda cartella aperta precedentemente
            closedir(dir2);

            // Ho trovato dei contenuti diversi, termino la procedura.
            if( equal_dir == 0 ) { 
                syslog(LOG_INFO, "%s and %s are not equal.", completePath1, completePath2);
                printf("NO EQUALS TRA %s e %s \n", completePath1, completePath2 );
                return equal_dir;
            }
        }
    }

    // Chiudo la prima cartella precedentemente aperta e restituisco se le cartelle sono uguali o differiscono per il contenuto..
    closedir(dir1);
    return equal_dir;
}

int are_equals(char * file_path1, char * file_path2) {

    syslog(LOG_INFO, "Compare %s with %s: are equal?", file_path1, file_path2);


    printf("------------ SEMPRE ARE_EQUAL -----------------\n");

    // Copio i due percorsi passati per parametro nel campo path delle rispettive strutture precedentemente dichiarate..
    str_file * file1;
    file1 = (str_file *) malloc(sizeof(str_file));
    if( file1 == NULL ) {
        syslog(LOG_ERR, "Malloc error: space unavailable to allocate memory. Terminated.");
        exit(EXIT_FAILURE);
    }

    // Apro il primo file
    file1->file = fopen(file_path1,"rb");
    if ( file1->file == NULL )  {
        // Impossibile aprire path1
        perror(file_path1);
        syslog(LOG_ERR, "Cannot open %s. Terminated.", file_path1);
        free(file1);
        exit(EXIT_FAILURE);
    }

    // Copio i due percorsi passati per parametro nel campo path delle rispettive strutture precedentemente dichiarate..
    str_file * file2;
    file2 = (str_file *) malloc(sizeof(str_file));
    if( file1 == NULL ) {
        syslog(LOG_ERR, "Malloc error: space unavailable to allocate memory. Terminated.");
        free(file1);
        exit(EXIT_FAILURE);
    }

    // Apro il secondo file..
    file2->file = fopen(file_path2,"rb");
    if ( file2->file == NULL )  {
        // Impossibile aprire path2
        perror(file_path2);
        syslog(LOG_ERR, "Cannot open %s. Terminated.", file_path2);
        free(file1);
        free(file2);
        exit(EXIT_FAILURE);
    }

    // Return value
    int are_equal = 1;

    // Ottengo la dimensione del file1 in bytes e ritorno all'inizio del file..
    fseek(file1->file, 0, SEEK_END);
    file1->size = ftell(file1->file);
    fseek(file1->file, 0, SEEK_SET);

    // Ottengo la dimensione del file2 in bytes e ritorno all'inizio del file..
    fseek(file2->file, 0, SEEK_END);
    file2->size = ftell(file2->file);
    fseek(file2->file, 0, SEEK_SET);

    char buf_file1[MAX_BUF_SIZE];   // Buffer nel quale memorizzo bytes che leggo dal primo file
    char buf_file2[MAX_BUF_SIZE];   // Buffer nel quale memorizzo bytes che leggo dal secondo file

    // Muovo i buffer utilizzati per il controllo in maniera parallela
    while(fgets(buf_file1, MAX_BUF_SIZE, file1->file) != NULL && fgets(buf_file2, MAX_BUF_SIZE, file2->file) != NULL && are_equal) {
        if(strcmp(buf_file1, buf_file2) != 0) {
            are_equal = 0;
            syslog(LOG_INFO, "Found differences between %s and %s. Files are not equal.", buf_file1, buf_file2);
        }
    }

    // Sarò arrivato alla fine dei uno dei due files..
    // Se un file ha ancora contenuto da leggere allora ci sono differenze tra i due files..
    while(fgets(buf_file1, MAX_BUF_SIZE, file1->file) != NULL && are_equal) {
        are_equal = 0;
        syslog(LOG_INFO, "Found differences. Files are not equal.");
    }
    while(fgets(buf_file2, MAX_BUF_SIZE, file2->file) != NULL && are_equal) {
        are_equal = 0;
        syslog(LOG_INFO, "Found differences. Files are not equal.");
    }

    // Stampo nel log che i files sono uguali se non sono state riscontrate differenze..
    if( are_equal ) { syslog(LOG_INFO, "Files are equal."); }

    fclose(file1->file);
    free(file1);
    fclose(file2->file);
    free(file2);

    // Ritorna il risultato del confronto: 1 se i files sono identici.
    return are_equal;
}

void dirwalk(char * path1, char * path2, int indent_limit, char symb)
{
    struct dirent *dirent1, *dirent2;
    DIR *dir1, *dir2;
    struct stat stat1, stat2;
    int is_dir1, is_dir2;

    // A differenza di path1 e path2 i completePath contengono i nomi dei files, mentre i paths solo la cartella di livello superiore.
    char completePath1[256];    // Buffer nel quale memorizzo il percorso relativo al primo file
    char completePath2[256];    // Buffer nel quale memorizzo il percorso relativo al secondo file

    int indent_tab = 0; // serve per dare una giusta indentazione al momento della stampa in console.
    int found = 0;      // serve per vedere se il file1 è stato trovato all'interno del path2.

    // La funzione opendir apre un directory stream.
    // Restituisce un puntatore ad un oggetto di tipo DIR in caso di successo e NULL in caso di errore.
    // Inoltre posiziona lo stream sulla prima voce contenuta nella directory.
    dir1 = opendir(path1);
    if (dir1 == NULL) { 
        printf("Cannot open %s\n", path1);
        syslog(LOG_ERR, "Cannot open %s. Terminated.", path1);
        exit(EXIT_FAILURE);
    }

    // Cicla tutti i files e directory presenti all'interno del path specificato.
    // La funzione readdir legge la voce corrente nella directory, posizionandosi sulla voce successiva.
    // Restituisce un puntatore al directory stream in caso di successo e NULL in caso di errore.
    dirent1 = readdir(dir1);
    while (dirent1 != NULL) {

        if (strcmp(dirent1->d_name, ".") != 0 && strcmp(dirent1->d_name, "..") != 0) {

            // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
            // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
            strcpy(completePath1, path1);
            strcat(completePath1, SLASH);
            strcat(completePath1, dirent1->d_name);

            // Setto il flag a zero per capire che finora non c'è alcun file uguale..
            found = 0;

            // Apro la directory rappresentata dal path2..
            dir2 = opendir(path2);
            if (dir2 == NULL) { 
                printf("Cannot open %s\n", path2);
                syslog(LOG_ERR, "Cannot open %s.\nTerminated.\n", path2);
                closedir(dir1);
                exit(EXIT_FAILURE);
            }

            // Valuto se il path1 rappresenta una directory..
            stat(completePath1, &stat1);
            is_dir1 = ((stat1.st_mode & S_IFMT) == S_IFDIR);

            // Scorro tutta la seconda directory e controllo se il file è presente
            dirent2 = readdir(dir2);
            while (dirent2 != NULL) {

                // Verifico che non siano le entries di default delle directories..
                if (strcmp(dirent2->d_name, ".") != 0 && strcmp(dirent2->d_name, "..") != 0) {

                    if(strcmp(dirent1->d_name, dirent2->d_name) == 0) {

                        // Concateno il nome del file/directory al path originario per lavorare ricorsivamente sul path completo..
                        // Serve per tenere traccia del path completo dove si trova il file o la directory che si sta analizzando.
                        strcpy(completePath2, path2);
                        strcat(completePath2, SLASH);
                        strcat(completePath2, dirent2->d_name); /* test aggiungo d_name */

                        // Setto il flag di found a 1 così riconosco che due files o directories hanno lo stesso nome..
                        found = 1;

                        // Sto visualizzando il file percorsi la prima volta..
                        if(symb == PLUS_SYMB) {

                            // test
                            printf("DIRWALK: compare %s with %s\n", completePath1, completePath2);

                            stat(completePath2, &stat2);
                            is_dir2 = ((stat2.st_mode & S_IFMT) == S_IFDIR);

                            if (is_dir1 && is_dir2) {

                                printf("%s e %s are directories\n", completePath1, completePath2);
                                printf("\nDIRWALK MANDO A are_equals: %s e %s\n", completePath1, completePath2);
                                if(!are_equals_directories(completePath1, completePath2)) {

                                    // Cartelle con contenuto diverso..
                                    for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf(TAB); }
                                    printf ("Differences between folders: %s - %s\n", completePath1, completePath2);

                                    printf("\nDIRWALK MANDO A dirwalk: %s e %s\n", completePath1, completePath2);

                                    // Procedo in maniera ricorsiva sulle sub-directory aventi lo stesso nome in entrambi i path..
                                    dirwalk(completePath1, completePath2, indent_limit + 1, PLUS_SYMB);
                                    dirwalk(completePath2, completePath1, indent_limit + 1, MINUS_SYMB);
                                }

                            } else /* if( !is_dir1 && !is_dir2 ) */ {
                                printf("diff between files %s e %s\n",completePath1, completePath2 );
                                diffBetweenFiles(completePath1, completePath2, indent_limit);
                            }
                        }
                    }
                }
            }

            // A seconda che il file sia presente nel path2 stampo una + o -
            if(!found) {
                for(indent_tab = 0; indent_tab < indent_limit; indent_tab++) { printf(TAB); }
                    printf ("%c %s\n", symb, dirent1->d_name);
            }

            // Chiude il directory stream della seconda cartella..
            closedir(dir2);
        }
    }

    // Chiude il directory stream della prima cartella..
    closedir(dir1);
}

int main (int argc, char **argv) {

    // Apro il file di log..
    openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);
    syslog(LOG_INFO, "Utility started: %s", argv[0]);
    syslog(LOG_INFO, "First path passed: %s", argv[1]);
    syslog(LOG_INFO, "Second path passsed: %s", argv[2]);

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
        printf("Error: can't access %s\n", argv[1]);
        syslog(LOG_ERR, "Error: can't access %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    if ( access_err2) {
        printf("Error: can't access %s\n", argv[2]);
        syslog(LOG_ERR, "Error: can't access %s", argv[2]);
        exit(EXIT_FAILURE);
    }

    // Valuto se i due percorsi sono delle directories..
    int is_dir1 = ((stbuf1.st_mode & S_IFMT) == S_IFDIR);
    int is_dir2 = ((stbuf2.st_mode & S_IFMT) == S_IFDIR);
    if(is_dir1 && is_dir2) {

        // I due percorsi rappresentano entrambi due directories..
        syslog(LOG_INFO, "Both paths (%s) and (%s) are directories.", argv[1], argv[2]);

        if(!are_equals_directories(argv[1], argv[2])) {

            // Calcolo le differenze tra path1 e path2 due volte:
            // Con la prima chiamata a dirwalk visualizzo i contenuti che il path1 ha in più rispetto al path2
            // Con la seconda chiamata visualizzo i contenuti che sono presenti in path2 ma che non sono presenti in path1

            // Differenze di path1 rispetto al path2..
            dirwalk(argv[1], argv[2], indent, PLUS_SYMB);

            printf("---------------------------------------------------\n");

            // Differenze di path2 rispetto al path1..
            dirwalk(argv[2], argv[1], indent, MINUS_SYMB);
        }
    }
    else if(!is_dir1 && !is_dir2) {

        // I due percorsi rappresentano entrambi due files..
        syslog(LOG_INFO, "Both paths %s and %s are files.", argv[1], argv[2]);
        diffBetweenFiles(argv[1], argv[2], indent);
    }
    else {

        // I due percorsi non possono essere comparati..
        syslog(LOG_ERR, "Paths cannot be compared. Terminated.");
        printf("You cannot compare two different kind of files (one directory one file)!");
        exit(EXIT_FAILURE);
    }

    // Termino con successo..
    syslog(LOG_INFO, "Exit success. Terminated.");
    exit(EXIT_SUCCESS);
}