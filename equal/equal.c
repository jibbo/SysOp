#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

    char* path1;
    strcpy(path1,argv[0]);     // Path of first file
    char* path2;
    strcpy(path2,argv[1]);     // Path of second file

    // true is both path are files or directories!
    // false if one path rappresent a file and the other rappresent a directory!
    bool comparable = !system("test -d " + path1) && !system("test -d " + path2);
    if(comparable) {
        printf("Both paths are directories!\n");

        // List all files directory 1
        // ---------------------
        // List all files directory 2

    }
    comparable = !system("test -f " + path1) && !system("test -f " + path2);
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

    return 0;
}