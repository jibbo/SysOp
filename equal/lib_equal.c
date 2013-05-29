
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

void dirwalk(char * path, int indent)
{
    struct dirent* direntry;
    DIR* dir;   // file descriptor usato per la gestione del directory stream
    struct stat stbuf;
    int is_dir;
    int access_err;

    //printf("tento di aprire %s\n", path);

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

        if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0)
            continue;    /* skip self and parent */

        size_t length = strlen(path) + strlen(direntry->d_name) + 2;
        char * concat = malloc(sizeof(char) * length);
        snprintf(concat, length, "%s/%s", path, direntry->d_name);
        stat(concat, &stbuf);

        // indentation
        int i = 0;
        while(i++ < indent) { 
            printf("  ");
        }

        is_dir = ((stbuf.st_mode & S_IFMT) == S_IFDIR);
        if (is_dir) {
            printf ("%s/\n" , direntry->d_name );
            dirwalk(concat, indent + 1);
        } else {
            printf ("%s\n" , direntry->d_name);
        }
        free(concat);
    }

    // Chiude il directory stream.
    // La funzione restituisce 0 in caso di successo e -1 altrimenti, 
    closedir(dir);
    return;
}