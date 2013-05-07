#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

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

    // Copy argv parameters into path1 and path2..
    char* path1;
    path1 = (char*) malloc(sizeof(argv[1]));
    path1 = strcpy(path1, argv[1]);
    char* path2;
    path2 = (char*) malloc(sizeof(argv[2]));
    path2 = strcpy(path2, argv[2]);

    // File test for both filepath
    char* test1 = (char*) malloc(sizeof("test -d ") + sizeof(argv[1]));
    test1 = strcpy(test1, "test -d ");
    strcat(test1, path1);

    char* test2 = (char*) malloc(sizeof("test -d ") + sizeof(argv[2]));
    test2 = strcpy(test2, "test -d ");
    test2 = strcat(test2, path2);

    printf("\n\ntest1: %s\n", test1);
    printf("\n\ntest2: %s\n", test2);

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

    comparable = !system(test1) && !system(test2);
    if(comparable) {
        printf("Both paths are files!\n");

        FILE *file1;    // First file
        FILE *file2;    // Second file

        if ((file1 = fopen(path1,"r")) == NULL)  {
            // Impossibile aprire path1
            perror(path1);
            exit(1);
        }
        else if ((file2 = fopen(path2,"r")) == NULL)  {
            // Impossibile aprire path1
            perror(path2);
            exit(1);
        }
        else {
            fclose(file1);
            fclose(file2);
        }
        exit(0);


        // Print file 1
        // ---------------------
        // Print file 2

    }
    else {
        printf("You cannot compare two different kind of file!\n");
    }
*/
    return 0;
}