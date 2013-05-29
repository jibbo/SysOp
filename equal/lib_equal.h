
#ifndef EQUAL_H

#define EQUAL_H
#define LOG_PATH  "/var/log/utility/equal.log"

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

FILE* log_file;

void dirwalk(char *, int);
void diffBetweenFiles(str_file *,  str_file *);

#endif