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

void manage();
void printHelp();
void makeBackup(char* path);
void showBackupContent(char* archive);

#endif