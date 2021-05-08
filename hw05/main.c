#include <unistd.h>
#include <pwd.h>
#include "options.h"


int main(int argc, char *argv[]) {
 
    char *path = NULL;
    char *bucket = NULL;
    char *optString = ":n:s:t:m:u:f:a0h";
    int opt = 0;
    struct atributs atributs = { 
        .sort = NULL,
        .mask = NULL,
        .attr = NULL,
        .userId = 0,
        .max = 0,
        .min = 0,
    };
    int options[8] = {0};
    int count = 1;
    while(count < argc) {
        count++;
        opt = getopt(argc, argv, optString);
        if (opt == -1 && optind != argc) {
            if (path != NULL) {
                free(path);
            }
            bucket = path;
            path = malloc(strlen(argv[optind]) + 1);
            if (path == NULL) {
                fprintf(stderr, "Memory fall");
                freeAll(path, NULL, &atributs, NULL);
                free(bucket);
                return EXIT_FAILURE;
            }
            strcpy(path, argv[optind]);
            optind++;
        }
        if (opt == '?') {
            fprintf(stderr, "Invalid option: %c\n", optopt);
            freeAll(path, NULL, &atributs, NULL);
            return EXIT_FAILURE;
        }
        if (opt == ':') {
            fprintf(stderr, "Missing argument to option %c\n", optopt);
            freeAll(path, NULL, &atributs, NULL);
            return EXIT_FAILURE;
        }
        switch (opt) {
            case 'a':
                options[0] = 1;
                break;
            case 'n':
                options[1] = 1;
                
                if (atributs.attr != NULL) {
                    free(atributs.attr);
                }
                
                atributs.attr = atributCopy(optarg, optopt);
                if (!atributs.attr) {
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }
                break;
            case 'm':
                options[2] = 1;
                
                if (atributs.mask != NULL) {
                    free(atributs.mask);
                }
                
                atributs.mask = atributCopy(optarg, optopt);
                if (atributs.mask == NULL || strlen(atributs.mask) > 3 || atributs.mask[0] >= '8' || atributs.mask[1] >= '8' || atributs.mask[2] >= '8') {
                    fprintf(stderr, "invalid mask: %s\n", optarg);
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }
                break;
            case 'u':
                options[3] = 1;
                struct passwd *user = getpwnam(optarg);
                if (user == NULL) {
                    fprintf(stderr, "No such user: %s\n", optarg);
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }
                atributs.userId = user->pw_uid;
                break;
            case 't':                
                options[4] = 1; 
                char *pEndf = NULL;
                atributs.max = strtol(optarg, &pEndf, 10);
                if (pEndf[0] !='\0' || strcmp(pEndf, optarg) == 0 || atributs.max < 0) {
                    fprintf(stderr, "Invalid depth: %s\n", optarg);
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }
                break;
            case 'f':
                options[5] = 1; 
                char *pEnd = NULL;
                atributs.min = strtol(optarg, &pEnd, 10);
                if (pEnd[0] !='\0' || strcmp(pEnd, optarg) == 0 || atributs.min < 0) {
                    fprintf(stderr, "Invalid depth: %s\n", optarg);
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                options[6] = 1;
                
                if (atributs.sort != NULL) {
                    free(atributs.sort);
                }
            
                atributs.sort = atributCopy(optarg, optopt);
                if (atributs.sort == NULL || strlen(atributs.sort) != 1 || (atributs.sort[0] != 's' && atributs.sort[0] != 'f')) {
                    fprintf(stderr, "invalid atribut: %s\n", optarg);
                    freeAll(path, NULL, &atributs, NULL);
                    return EXIT_FAILURE;
                }

                break;
            case '0':
                options[7] = 1;
                break;
            case 'h':
                help();
                freeAll(path, NULL, &atributs, NULL);
                return EXIT_SUCCESS;
        }
    }
    if (path == NULL) {
        path = malloc(2);
        if (path == NULL){
            fprintf(stderr, "Memory fall");
            freeAll(path, NULL, &atributs, NULL);
            return EXIT_FAILURE;
        }
        strcpy(path, ".");
    }
        struct stat st;
    if (stat(path, &st)) {
        perror(path);
        free(path);
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(st.st_mode)) {
        printf("File %s is not directory\n", path);
        free(path);
        return EXIT_FAILURE;
    }
    DIR *fd = NULL;
    if ((fd = opendir(path)) == NULL) {
        perror(path); 
        free(path);
        return EXIT_FAILURE;
    }
    struct resultList *list = malloc(sizeof(struct resultList)); 
    if (list == NULL){
        fprintf(stderr, "Memory fall");
        freeAll(path, fd, &atributs, list);
        return EXIT_FAILURE;
    }
    list->allocSize = 0;
    list->possition = 0; 
    if (readFiles(path, options, list, 1, atributs) != EXIT_SUCCESS) {
        freeAll(path, fd, &atributs, list);
    }
    if (list->possition == 0) {
        freeAll(path, fd, &atributs, NULL);
        free(list);
        return EXIT_SUCCESS;
    }
    if (options[6] == 1 && atributs.sort[0] == 'f') {
        qsort(list->list, list->possition, sizeof(char *), compF);
    } else if (options[6] == 1 && atributs.sort[0] == 's') {
        qsort(list->list, list->possition, sizeof(char *), compS);
    } else {
        qsort(list->list, list->possition, sizeof(char *), comp);
    }

    if (options[7] == 1) {
        printLines(list->list, list->possition, '\0');
    } else {
        printLines(list->list, list->possition, '\n');
    }
    freeAll(path, fd, &atributs, list);
    
    return EXIT_SUCCESS;
}