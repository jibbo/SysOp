/*
    Anno accademico     2012/2013
    Corso studio        Informatica
    Progetto            #1 MKBKB
    Componenti:
        Zen Roberto             151737
        Giovanni De Francesco   152080
        Perantoni Nicola        151709
*/

#include "mkbkp.h"

#define PATH_MAX_LENGTH 256

typedef struct {
  char path[PATH_MAX_LENGTH];
  FILE* file;
  char* line;
  size_t size;
  ssize_t read;
} str_file;

FILE* backup;

char cwd[PATH_MAX_LENGTH];
char subpath[PATH_MAX_LENGTH];
char filevalue[PATH_MAX_LENGTH];
char dirvalue[PATH_MAX_LENGTH];

char *f_value = NULL;
char *t_value = NULL;

int f_flag = 0;
int c_flag = 0;
int x_flag = 0;
int t_flag = 0;

int main(int argc, char **argv) {
	int c;
	opterr = 0;

	openlog(argv[0], LOG_CONS || LOG_PID, LOG_LOCAL0);

	while ((c = getopt (argc, argv, "f:xct:")) != -1) {
    switch (c) {
      case 'f':
        f_flag = 1;
        f_value = optarg;
        break;
      case 'c':
        c_flag = 1;
        break;
      case 'x':
        x_flag = 1;
        break;
      case 't':
        t_flag = 1;
        t_value = optarg;
        break;
      default:
        printf("Argument not available\n");
        printHelp();
        exit(EXIT_FAILURE);
    }
    checkInput(optind, argc, argv);
  }
  return 0;
}

void checkInput(int opt_index, int argc, char** targets) {

	FILE *tempfile;

	// 
	// Visualizzazione del contenuto di un archivio
	// 
	if(t_flag == 1) {
		if(c_flag == 1 || x_flag == 1 || f_flag == 1) {
			printf("Errore nell'utilizzo dei parametri\n");
		} else if (t_value == NULL) {
			printf("Non è stato specificato un archivio\n");
		} else {
			printf("Visualizzo il contenuto archivio: %s\n", optarg);
			if(tempfile = fopen(f_value, "r")) {
				extractBackup(tempfile);
			}
		}
	}

	// 
	// Estrazione di un archivio
	// 
	if(x_flag == 1) {
		if(f_flag == 0) {
			printf("Errore nell'utilizzo dei parametri\n");
		} else if (f_value == NULL) {
			printf("Non è stato specificato un archivio\n");
		} else if (c_flag == 1){
			printf("\n");
		} else {
			printf("Estrazione archivio\n");
			if(tempfile = fopen(f_value, "r")) {
				extractBackup(tempfile);
			} else {
				printf("File non trovato\n");
			}	
		}
	}

	// 
	// Creazione di un nuovo archivio
	// 
	if(c_flag == 1) {
		if(f_flag == 0){
			printf("Errore nell'utilizzo dei parametri\n");
		} else if (f_value == NULL) {
			printf("Non è stato specificato un archivio\n");
		} else {
			printf("Compressione archivio\n");
			createBackup();
		}
	}

	if(f_flag == 1) {
		printf("Non è stato specificato un archivio\n");
	} else {
		printHelp();
	}
}

void printHelp() {
	printf("Usage: mkbkp [-c] [-x] [-t] [-f]\n");
	printf("\t -f to create or extract an archive");
	printf("\n\t -c to create a new archive");
	printf("\n\t -x to extract an archive in the current directory");
	printf("\n\t -t to display the content of an archive\n");
}

void createBackup(char* path) {
	struct dirent* direntry;
	DIR* dir;
	struct stat stbuf;
	int is_dir;
	int access_err;

	FILE *openedfile;
	char* buffer = NULL;
	int flength = 0;

	// La funzione opendir apre un directory stream.
	// Restituisce un puntatore ad un oggetto di tipo DIR in caso di successo e NULL in caso di errore.
	// Inoltre posiziona lo stream sulla prima voce contenuta nella directory.
	if ((dir = opendir(path)) == NULL) { 
	  printf("Non è stato possibile aprire la cartella %s\n", path);
	  return;
	}

	// La funzione readdir legge la voce corrente nella directory, posizionandosi sulla voce successiva.
	// Restituisce un puntatore al directory stream in caso di successo e NULL in caso di errore.
	// Loop on directory entries
	while ((direntry = readdir(dir)) != NULL) {
		if (strcmp(direntry -> d_name, ".") == 0 || strcmp(direntry -> d_name, "..") == 0) {
		  continue;
		}

		size_t length = strlen(path) + strlen(direntry -> d_name) + 2;
		char * concat = (char *) malloc(sizeof(char) * length);
		snprintf(concat, length, "%s/%s", path, direntry -> d_name);
		stat(concat, &stbuf);

		// Legge la cartella corrente
		getcwd(cwd, PATH_MAX_LENGTH);
		strcat(cwd, "/");
		strcat(cwd, filevalue);

		// Apro il file di backup in Append
		backup = fopen(cwd, "a+");
		if(backup == NULL) {
		  perror(cwd);
		  exit(EXIT_FAILURE);
		}

		// Bool che indica se è una directory
		is_dir = ((stbuf.st_mode & S_IFMT) == S_IFDIR);

		// se è una directory
		if (is_dir) {
			strcpy(subpath, path);
			strcat(subpath, "/");
			strcat(subpath, direntry -> d_name);

			fprintf(backup, "\nDIR=");
			fprintf(backup, "%s", subpath);
			fprintf(backup, "\n");

			createBackup(subpath);
		} else {
			// allora è un file
			str_file* temp;

			temp = (str_file* ) malloc(sizeof(str_file));
			strcpy(temp -> path, path);
			strcat(temp -> path, "/");
			strcat(temp -> path, direntry -> d_name);

			temp -> file = fopen(temp -> path, "rb");
			if(temp -> file == NULL) {
				perror(temp -> path);
				exit(EXIT_FAILURE);
			}

			fseek(temp -> file, 0, SEEK_END);
			temp -> size = ftell(temp -> file);
			fseek(temp -> file, 0, SEEK_SET);

			temp -> line = (char* ) malloc(temp -> size);
			temp -> read = fread(temp -> line, temp -> size, 1, temp -> file);

			// Scrive il delimitatore del file all'interno dell'archivio
			fprintf(backup, "FILE=");
			fprintf(backup, "%s", temp -> path);
			fprintf(backup, "\n");

			// Scrive il file appena letto all'interno dell'archivio
			fwrite(temp->line, 1, temp -> size, backup);
			fclose(temp -> file);

			free(temp -> line);
			free(temp);
		}
		fclose(backup);
	}
	closedir(dir);
}

void extractBackup(FILE* bkp)  {
	
}

void showBackupContent(FILE* bkp) {
	
}