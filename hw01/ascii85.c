#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printelka(int resultList[4], int counter)
{
    unsigned int result = resultList[0];
    int sings[5];
    for (int i = 1; i <= counter; i++) {
        result = result << 8;
        result = result | resultList[i];
    }
    for (int i = counter; i < 3; i++) {
        result = result << 8;
    }
    for (int i = 0; i <= 4; i++) {
        sings[i] = result % 85 + 33;
        result /= 85;
    }
    for (int i = 4; i >= 0; i--) {
        printf("%c", sings[i]);
    }
    return 0;
}

int encode(void)
{
    int counter = 0;
    int current;
    int resultList[4];
    current = getchar();
    //printf("beru znak %d\n", counter);
    while (current != EOF) {
        if (counter == 3) {
            resultList[counter] = current;
            printelka(resultList, 3);
            counter = 0;
            current = getchar();
      } else {
          resultList[counter] = current;
          current = getchar();
          counter++;
      }
    }
    if (counter != 0) {
        printelka(resultList, counter - 1);
    }
    printf("\n");
    return 0;
}


int printelkaDec(int current, int counter) {
    int resultList[4];
    int result = 255;
    for (int i = counter; i >= 0; i--) {
        result = 255;
        result = result & current;
        resultList[i] = result;
        current = current >> 8;
    }
    for (int i = 0; i <= counter; i++) {
        //printf("cuka %d \n", i);
        printf("%c", resultList[i]);
    }
    return 0;
}
int decode(void)
{
       // TODO: implement Ascii85 decoding
    int counter = 0;
    int current = getchar();
    while(isspace(current) && current != EOF) {
        current = getchar();
    }
    int result = 0;
    while (current != EOF) {
        while(isspace(current) && current != EOF) {
            current = getchar();
        }
        if ((current > 117 || current < 33) && current != EOF) {
            counter++;
            return 7;
        }
        if (counter == 4) {
            result = result * 85 + current - 33;
            printelkaDec(result, 3);
            counter = 0;
            current = getchar();
            result = 0;

        } else {
            if (current != EOF) {
               result = result * 85 + current - 33;
               current = getchar();
               counter++;
               //printf("ff\n");
            }
        }
    }
    //printf("\n%d \n ", counter);
    if (counter != 0) {
      //  printf("\n %d! \n", counter);
        return 17;
        printelkaDec(result, counter - 1);
    } else {
           //   printf("\n %d!! \n", counter);
        return 0;
    }
}



// ================================
// DO NOT MODIFY THE FOLLOWING CODE
// ================================
int main(int argc, char *argv[])
{
    int retcode = 1;

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-e") == 0)) {
        retcode = encode();
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        retcode = decode();
    } else {
        fprintf(stderr, "usage: %s [-e|-d]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (retcode != 0) {
        fprintf(stderr, "an error occured\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
