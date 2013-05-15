#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <sys/stat.h>
#include <sys/types.h>

// header -----------------------------------------


#define NAME_MAX 14   /* longest filename component; */
                      /* system-dependent */

#define MAX_PATH 1024

typedef struct {              /* portable directory entry */
   long ino;                  /* inode number */
   char name[NAME_MAX+1];     /* name + '\0' terminator */
} Dirent;

typedef struct {              /* minimal DIR: no buffering, etc. */
   int fd;                    /* file descriptor for the directory */
   Dirent d;                  /* the directory entry */
} DIR;

DIR *opendir(char *dirname);
Dirent *readdir(DIR *dfd);
void closedir(DIR *dfd);
void dirwalk(char *dir);

// ------------------------------------------------

struct str_file {

    char * path;
    FILE * file;
    char * line;
    size_t size;
    ssize_t read;

};

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


    struct str_file * file1;
    struct str_file * file2;
    file1 = (struct str_file *) malloc(sizeof(struct str_file));
    file2 = (struct str_file *) malloc(sizeof(struct str_file));

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


    if(is_dir_1 ^ is_dir_2) {

        // Same types (both paths are files or directories) ..
        if(is_dir_1 && is_dir_2) {
            // Directories case..
            dirwalk(file1->path);
            dirwalk(file2->path);
        }
        else {
            // Files case..
            printf("Both paths are files!\n");

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
    }
    else {
        printf("You cannot compare two different kind of file (one directory one file)!\n");
    }

/*
    int len;
    struct dirent *pDirent;
    DIR *pDir;

    if (c < 2) {
        printf ("Usage: testprog <dirname>\n");
        return 1;
    }
    pDir = opendir (v[1]);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", v[1]);
        return 1;
    }

    while ((pDirent = readdir(pDir)) != NULL) {
        printf ("[%s]\n", pDirent->d_name);
    }
    closedir (pDir);
    */

    return 0;
}

void dirwalk(char *dir)
{
    char name[MAX_PATH];
    Dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "dirwalk: can't open %s\n", dir);
        return;
    }
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->name, ".") == 0 || strcmp(dp->name, ".."))
            continue;    /* skip self and parent */
        if (strlen(dir)+strlen(dp->name)+2 > sizeof(name))
            fprintf(stderr, "dirwalk: name %s %s too long\n", dir, dp->name);
        else {
            sprintf(name, "%s/%s", dir, dp->name);
            dirwalk(dp->name);
        }
    }
    closedir(dfd);
}