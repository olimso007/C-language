#include <stddef.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

struct atributs {
    char *sort;
    char *mask;
    char *attr;
    uid_t userId;
    int max;
    int min;
};

struct resultList {
    char **list;
    size_t allocSize;
    size_t possition;
};


void help(void);

int checkFile(struct stat st, char *name, int *options, struct atributs atr, int depth);

int readFiles(char *path, int *options, struct resultList *list,  int depth, struct atributs atr);

int comp(const void *i, const void *j);

char *lastSlash(char *str);

int compF(const void *i, const void *j);

int compS(const void *i, const void *j);

void printLines(char **list, size_t size, char end);

char *atributCopy(char *atribut, char optopt);

int path_copy(char *path, struct resultList *list);

void freeAll(char *path, DIR *dr, struct atributs *atr, struct resultList *list);

