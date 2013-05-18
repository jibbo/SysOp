#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

// header -----------------------------------------

#define MAX_PATH 1024

// ------------------------------------------------

typedef struct{
    char * path;
    FILE * file;
    char * line;
    size_t size;
    ssize_t read;
} str_file;

FILE* log_file;

void dirwalk(char *);
void diffBetweenFiles(str_file * file1,  str_file * file2);

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



/*
 % testopt
     aflag = 0, bflag = 0, cvalue = (null)
     
     % testopt -a -b
     aflag = 1, bflag = 1, cvalue = (null)
     
     % testopt -ab
     aflag = 1, bflag = 1, cvalue = (null)
     
     % testopt -c foo
     aflag = 0, bflag = 0, cvalue = foo
     
     % testopt -cfoo
     aflag = 0, bflag = 0, cvalue = foo
     
     % testopt arg1
     aflag = 0, bflag = 0, cvalue = (null)
     Non-option argument arg1
     
     % testopt -a arg1
     aflag = 1, bflag = 0, cvalue = (null)
     Non-option argument arg1
     
     % testopt -c foo arg1
     aflag = 0, bflag = 0, cvalue = foo
     Non-option argument arg1
     
     % testopt -a -- -b
     aflag = 1, bflag = 0, cvalue = (null)
     Non-option argument -b
     
     % testopt -a -
     aflag = 1, bflag = 0, cvalue = (null)
     Non-option argument -
     */

main (int argc, char **argv)
{
    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "abc:")) != -1)
    {
        switch (c)
        {
            case 'a':
                aflag = 1;
                break;
            case 'b':
                bflag = 1;
                break;
            case 'c':
                cvalue = optarg;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    }

    printf ("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);

    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    // Start with project
    // .....................................................................................

    str_file * file1;
    str_file * file2;
    file1 = (str_file *) malloc(sizeof(str_file));
    file2 = (str_file *) malloc(sizeof(str_file));

    // Copy argv parameters into path1 and path2..
    file1->path = (char*) malloc(sizeof(argv[1]));
    file1->path = strcpy(file1->path, argv[1]);
    file2->path = (char*) malloc(sizeof(argv[2]));
    file2->path = strcpy(file2->path, argv[2]);

    struct stat stbuf1;
    struct stat stbuf2;

    int access_err1 = (stat(file1->path, &stbuf1) == -1);
    int access_err2 = (stat(file2->path, &stbuf2) == -1);

    if ( access_err1 ) {
        fprintf(stderr, "Error: can't access %s\n", file1->path);
        return;
    }
    if ( access_err2) {
        fprintf(stderr, "Error: can't access %s\n", file2->path);
        return;
    }

    int is_dir_1 = ((stbuf1.st_mode & S_IFMT) == S_IFDIR);
    int is_dir_2 = ((stbuf2.st_mode & S_IFMT) == S_IFDIR);

    if(is_dir_1 && is_dir_2) {

        // Directories case..
        dirwalk(file1->path);
        dirwalk(file2->path);
    }
    else if(!is_dir_1 && !is_dir_2) {
        diffBetweenFiles(file1, file2);        
    }
    else {
        printf("You cannot compare two different kind of files (one directory one file)!\n");
    }
    return 0;
}

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
        file1->line = malloc(file1->size);
        fseek(file1->file, 0, SEEK_SET);  // Return to the top of the file

        // Obtain file2 dimension in bytes
        fseek(file2->file, 0, SEEK_END);
        file2->size = ftell(file2->file);
        file2->line = malloc(file2->size);
        fseek(file2->file, 0, SEEK_SET);  // Return to the top of the file

        printf("file1->path: %s\n", file1->path);
        printf("file1->size: %d bytes\n", file1->size);
        printf("file2->path: %s\n", file2->path);
        printf("file2->size: %d bytes\n", file2->size);
        printf("\n-----------------------------------\n");
        
        // Read the entire file1 and file2
        file1->read = fread(file1->line, file1->size, 1, file1->file);
        file2->read = fread(file2->line, file2->size, 1, file2->file);

        printf("file1:\n\nRead: \n%s\n----------------------------", file1->line);
        printf("file2:\n\nRead: \n%s\n----------------------------", file2->line);
        free(file1->line);
        free(file2->line);

        fclose(file1->file);
        fclose(file2->file);
    }
    exit(0);
}

void dirwalk(char * path)
{
    char name[MAX_PATH];
    struct dirent* direntry;
    DIR* dir;   // file descriptor usato per la gestione del directory stream

    printf("tento di aprire %s\n", path);

    // La funzione opendir apre un directory stream.
    // Restituisce un puntatore ad un oggetto di tipo DIR in caso di successo e NULL in caso di errore.
    // Inoltre posiziona lo stream sulla prima voce contenuta nella directory.
    if ((dir = opendir(path)) == NULL) { 
        printf("dirwalk: can't open %s\n", path);
        return;
    }

    // La funzione readdir legge la voce corrente nella directory, posizionandosi sulla voce successiva.
    // Restituisce un puntatore al directory stream in caso di successo e NULL in caso di errore.
    // Loop on directory entries
    while ((direntry = readdir(dir)) != NULL) {
        if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, ".."))
            continue;    /* skip self and parent */
        if (strlen(path)+strlen(direntry->d_name)+2 > sizeof(path))
            printf("dirwalk: name %s %s too long\n", path, direntry->d_name);
        else {

            struct stat data;
            stat ( direntry->d_name, & data ); /* get stat data */
            sprintf ("File : %s\t size : %d\n " , direntry->d_name, data.st_size );

            //printf("%s/%s", path, direntry->d_name);
            dirwalk(direntry->d_name);
        }
    }

    // Chiude il directory stream.
    // La funzione restituisce 0 in caso di successo e -1 altrimenti, 
    closedir(dir);
}