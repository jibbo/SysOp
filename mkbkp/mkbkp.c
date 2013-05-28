#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>

char pathname[MAXPATHLEN];

int main(int argc, char **argv) {
    int cflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "fbc:")) != -1)
        switch (c) {
            case 'f':
                cvalue = optarg;
                break;
            case 'b':
                bflag = 1;
                break;
            case 'c':
                cflag = 1;
                break;
            case '?':
                if (optopt == 'f')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
                return 1;
            default:
                abort();
        }

    printf("cvalue = %s, bflag = %d, aflag = %d\n", cvalue, bflag, cflag);

    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
    
    return 0;
}

void scanworkingpath() {
  struct direct **files;
  int file_select();

  if (getwd(pathname) == NULL ) { 
    printf("Error getting path\n");
    exit(0);
  }

  printf("Current Working Directory = %s\n",pathname);

  count = scandir(pathname, &files, file_select, alphasort);

  /* If no files found, make a non-selectable menu item */
  if (count <= 0) {
    printf("No files in this directory\n");
    exit(0);
  }

  for (i=1;i<count+1;++i) {
    printf("%s", files[i-1] -> d_name);
  }
  
  printf("\n");
}