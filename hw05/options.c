#include "options.h"

void help(void) 
{
    fprintf(stderr, "HELP\n");
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "-h %-20s Print help\n", "");
    fprintf(stderr, "-n NAME %-15s Filter by substring NAME in file name\n", "");
    fprintf(stderr, "-m MASK %15s Filter by  permissions\n", "");
    fprintf(stderr, "-u USER %15s Filter by user name\n", "");
    fprintf(stderr, "-f NUM %16s Filter by minimal depth\n", "");
    fprintf(stderr, "-t NUM %16s Filter by minimal depth\n", "");
    fprintf(stderr, "-a %20s Go throught hidden dirs\n", "");
    fprintf(stderr, "-0 %20s Use null byte as line separator\n", "");
    fprintf(stderr, "-s SORT %15s Sorting by:\n", "");
    fprintf(stderr, "(SORT = s) %12s Sort by file size\n", "");
    fprintf(stderr, "(SORT = f) %12s Sort by file path\n", "");
}

int readFiles(char *path, int *options, struct resultList *list,  int depth, struct atributs atr)
{
    struct dirent *current= NULL;
	DIR *head_dir = NULL;
    head_dir = opendir(path);
	if (head_dir == NULL) {
		perror(path);
		return EXIT_FAILURE;
	}

	size_t buf_len = strlen(path) + 2;
    char *buffer = malloc(buf_len);
    char *bucket = NULL;
    if (buffer == NULL){
        fprintf(stderr, "Memory fault");
        return EXIT_FAILURE;
    }
	strcpy(buffer, path);
    strcat(buffer, "/");

    while ((current = readdir(head_dir)) != NULL) {
        if (strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..") == 0) {
            continue;
        }

        buffer[buf_len - 1] = '\0';
        bucket = buffer;
        buffer = realloc(buffer, buf_len + strlen(current->d_name));
        if (buffer == NULL) {
            fprintf(stderr, "Memory fault");
            free(bucket);
            return EXIT_FAILURE;
        }
        strcat(buffer, current->d_name);
        struct stat statt;

        if (stat(buffer, &statt) != 0) {
            perror(path);
            continue;
        }
        if (S_ISDIR(statt.st_mode)) {
            if ((current->d_name)[0] != '.' || options[0] == 1) {
                readFiles(buffer, options, list, depth + 1, atr);                
            }

            continue;
        }
        if (S_ISREG(statt.st_mode)) {
            if (checkFile(statt, current->d_name, options, atr, depth) != 0) {
                if (path_copy(buffer, list) != 0) {
                    fprintf(stderr, "Memory fault");
                    return EXIT_FAILURE;
                }
            }
            continue;
        }
    }
    free(buffer);
    closedir(head_dir);
    return EXIT_SUCCESS;
}

int checkFile(struct stat st, char *name, int *options, struct atributs atr, int depth)
{

    if (options[0] != 1 && name[0] == '.') {
        return 0; // -a did not use and file is hidden
    }
    if (options[1] == 1 && (strstr(name, atr.attr) == NULL)) {
        return 0; // -n file`s name does not include substring 
    }
    if (options[2] == 1) {
        char userMask[7];
        sprintf(userMask, "%o", st.st_mode);
        if (strcmp(userMask + 3, atr.mask) != 0) {
            return 0; //-m file`s permission does not meet the condition
        }   
    }
    if (options[3] == 1 && atr.userId != st.st_uid) {
        return 0; // -u user id of file does not meet the condition
    }
    if (options[4] == 1 && depth > atr.max) {
        return 0; // -t
    }
    if (options[5] == 1 && depth < atr.min) {
        return 0; // -f
    }
    return 1;
}


int compF(const void *i, const void *j)
{
    char *first = *((char **) i);
    char *second = *((char **) j);
    return strcmp(first, second);
}

char *lastSlash(char *str)
{
    char *current = strchr(str, '/');
    int last = current - str;
    while (current != NULL) {
        last = current - str;
        current = strchr(str + last + 1, '/');
    }
    return str + last + 1;
}

void toLowCase(char *str) 
{
    for (size_t i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}

int comp(const void *i, const void *j)
{
    char *first = lastSlash(*((char **) i));
    char *second = lastSlash(*((char **) j));
    size_t lenght = 0;
    if (strlen(first) > strlen(second)) {
        lenght = strlen(second);
    } else {
        lenght = strlen(first);
    }
    for (size_t i = 0; i < lenght; i++) {
        if (tolower(first[i]) != tolower(second[i])) {
            return tolower(first[i]) - tolower(second[i]);
        }
    }
    if (strlen(first) > strlen(second)) {
        return 1;
    }
    if (strlen(first) < strlen(second)) {
        return -1;
    }
    return strcmp(*((char **) i), *((char **) j));
}

int compS(const void *i, const void *j)
{
    struct stat st1; 
    struct stat st2; 
    stat (*((char **) i), &st1);
    stat (*((char **) j), &st2);
    if (st1.st_size - st2.st_size == 0) {
        return comp(i, j);
    } 
    if (st2.st_size - st1.st_size > 0) {
        return 1;
    }
    return -1;
}

void printLines(char **list, size_t size, char end)
{
    for (size_t i = 0; i < size; i++) {

        printf("%s%c", list[i], end);
    }
}


char *atributCopy(char *atribut, char optopt) 
{
    size_t len = strlen(atribut);
    if (len >= 255) {
       fprintf(stderr, "Argument of option %c too long", optopt);
        return NULL;
        }
    char *out = (char *)malloc(sizeof(char) * (len + 1));
    if (out == NULL) {
        fprintf(stderr, "Memory fall");
        return NULL;
    }
    strncpy(out, atribut, len);
    out[len] = '\0';
    return out;
}


int path_copy(char *path, struct resultList *list)
{   

    if (list->possition == 0)
    {
        list->allocSize += 10 * sizeof(char *);
        list->list = (char **) malloc(list->allocSize);
        if (list->list == NULL) {
            return EXIT_FAILURE;
        }
    }
    if (list->possition + 1 > list->allocSize / sizeof(char *))
    {
        list->allocSize += 10 * sizeof(char *);
        char **bucket = list->list; 
        list->list = (char **) realloc(list->list, list->allocSize);
        if (list->list == NULL) {
            free(bucket);
            return EXIT_FAILURE;
        }
    } 
    size_t possition = list->possition;
    list->list[possition] = (char *)malloc(sizeof(char) * (strlen(path) + 2));
    if (list->list[possition] == NULL) {
        return EXIT_FAILURE;
    }
    strcpy(list->list[possition], path);
    list->possition++;
    return EXIT_SUCCESS;
}

void freeAll(char *path, DIR *dr, struct atributs *atr, struct resultList *list)
{
    if (path != NULL) {
        free(path);
    }
    if (dr != NULL) {
        closedir(dr);
    }
    
    if (atr->sort != NULL) {
        free(atr->sort);
    }
    if (atr->mask != NULL) {
        free(atr->mask);
    }
    if (atr->attr != NULL) {
        free(atr->attr);
    }
    
    if (list != NULL) {
        for (size_t i = 0; i < list->possition; i++) {
        free(list->list[i]);
        }
        free(list->list);
        free(list);
    }
}
