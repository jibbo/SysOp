#ifndef MKBKP_H
#define	MKBKP_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <limits.h>
#include <string.h>

void checkInput(int opt_index, int argc, char** targets);
void printHelp();
void createBackup(char* path);
void extractBackup(FILE* bkp);
void showBackupContent(FILE* bkp);

#endif