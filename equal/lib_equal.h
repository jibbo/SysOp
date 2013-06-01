#ifndef EQUAL_H

#define EQUAL_H
#define PLUS_SYMB '+'
#define MINUS_SYMB '-'
#define TAB "  "
#define SLASH "/"
#define MAX_BUF_SIZE 128

#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <syslog.h>

// Struttura che rappresenta un file
typedef struct {
	char * path;    // Percorso assoluto del file
	FILE * file;    // Puntatore al file
	size_t size;    // Dimensione del file
} str_file;

// Naviga all'interno delle due directory specificate dai primi due parametri,
// per ogni file presente nel primo path, scorre la seconda directory fino a trovare un
// file omonimo oppure fino a che non ha completato la visita della directory.
// Una volta terminata la visita della seconda directory procede al file successivo contenuto
// nella prima directory in maniera iterativa.
// Nel caso trovi una sub-directory, la funzione viene richiamata sulla subdirectory.
void dirwalk(char *, char *, int, char);

// Stampa sulla console i contenuti che differiscono tra i path passati come primi 2 parametri.
// Il terzo parametro rappresenta una misura di tabulazione per una stampa ordinata.
void diffBetweenFiles(char *, char *, int);

// Verifica se i due paths passati come parametri rappresentano due files uguali.
// Restituisce 1 se i file sono uguali, 0 altrimenti.
int are_equals(char *, char *);

// Simile alla are_equals, verifica se i due paths passati come parametri rappresentano due cartelle uguali.
// Due cartelle sono uguali se e solo se sono uguali tutti i files e le sottocartelle proprie.
// Restituisce 1 se le cartelle sono uguali, 0 altrimenti.
int are_equals_directories(char *, char *);

// Verifica se il percorso passato per parametro rappresenta una cartella vuota.
// Una cartella è vuota se e solo se contiene solo i link . e ..
// Restituisce 1 se la cartella è vuota, 0 altrimenti.
int empty_directory(char *);

// Stampa la stringa passata come primo parametro secondo una tabulazione di grandezza definita dal secondo parametro.
// Produce una stampa sulla console.
void printIndented(char *, int);

// Stampa un messaggio di Help per l'utilizzo corretto della utility.
void printHelp();

#endif