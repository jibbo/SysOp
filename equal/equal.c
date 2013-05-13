#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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



/*
    free(path1);
    free(path2);

    printf("\n\np1: %s\n", path1);
    printf("\n\np2: %s\n", path2);
*/

    // File test for both filepath
    /*
    char* test1 = (char*) malloc(sizeof("test -d ") + sizeof(path1));
    test1 = strcat(test1, "test -d ");
    test1 = strcat(test1, path1);

    char* test2 = (char*) malloc(sizeof("test -d ") + sizeof(path2));
    test2 = strcat(test2, "test -d ");
    test2 = strcat(test2, path2);

    printf("\n\ntest1: %s\n", test1);
    printf("\n\ntest2: %s\n", test2);
    */

/*
    // Start with project
    // .....................................................................................

    // true is both path are files or directories!
    // false if one path rappresent a file and the other rappresent a directory!
    bool comparable = !system(test1) && !system(test2);
    if(comparable) {
        printf("Both paths are directories!\n");

        // List all files directory 1
        // ---------------------
        // List all files directory 2
    }

    // Directory test for both filepath
    test1 = (char*) malloc(sizeof("test -d ") + sizeof(argv[1]));
    test1 = strcpy(test1, "test -d ");
    test1 = strcat(test1, path1);
    test2 = (char*) malloc(sizeof("test -d ") + sizeof(argv[2]));
    test2 = strcpy(test2, "test -d ");
    test2 = strcat(test2, path2);
*/

    int comparable = 1;
    //comparable = !system(test1) && !system(test2);
    if(comparable) {
        printf("Both paths are files!\n");

        struct str_file * file1;
        struct str_file * file2;
        file1 = (struct str_file *) malloc(sizeof(struct str_file));
        file2 = (struct str_file *) malloc(sizeof(struct str_file));



        // Copy argv parameters into path1 and path2..
        file1->path = (char*) malloc(sizeof(argv[1]));
        file1->path = strcpy(file1->path, argv[1]);
        file2->path = (char*) malloc(sizeof(argv[2]));
        file2->path = strcpy(file2->path, argv[2]);

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

        // Print file 1
        // ---------------------
        // Print file 2

    }
    else {
        printf("You cannot compare two different kind of file!\n");
    }

    return 0;
}