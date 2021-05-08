#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int readCards(char allCards[]);

int toNullSring(char string[]);

int toNullPointer(char *pointer);

int checkCard(char card[], char allCards[], int instance);

int cardToNumber(char cardValue);

int comp(const void *i, const void *j);

int straight(char gamerCards[], char combinationC[]);

int pair(char gamerCard[], char combination[]);

int twoPair(char gamerCard[], char combination[]);

int fourOfKind(char gamerCard[], char combination[]);

int threeOfKind(char gamerCard[], char combination[]);

int highCard(char gamerCard[], char combination[]);

int fullHouse(char gamerCard[], char combination[]);

int flush(char gamerCard[], char combination[]);

int checkCombination(char gamerCard[], char combination[]);

int main(void)
{
    int end = 1;
    while (end != EOF) {
        char allCards[20] = { '\0' };
        end = readCards(allCards);
        if (end != 0)
            return 0;
        char combination1[11] = { '\0' }; //first gamer combination
        char combination2[11] = { '\0' }; //second gamer combination
        char gamer1[15] = { '\0' };       // first gamer cards
        strcpy(gamer1, allCards + 4);
        for (int i = 0; i < 4; i++) {
            gamer1[i] = allCards[i];
        }
        char gamer2[15] = { '\0' }; // second gamer cards
        strcpy(gamer2, allCards + 4);
        qsort(gamer1, 7, 2, comp);
        qsort(gamer2, 7, 2, comp);
        int result1 = checkCombination(gamer1, combination1);
        int result2 = checkCombination(gamer2, combination2);
        int result = 0;
        if (result1 > result2) {
            printf("W\n");
            result = 1;
        }
        if (result1 < result2) {
            printf("L\n");
            result = 1;
        }

        for (int i = 0; i < 10; i += 2) {
            if (result)
                break;
            int higest1 = cardToNumber(combination1[i]);
            int higest2 = cardToNumber(combination2[i]);
            if (higest1 > higest2) {
                printf("W\n");
                result = 1;
            }
            if (higest1 < higest2) {
                printf("L\n");
                result = 1;
            }
        }
        if (!result)
            printf("D\n");
    }
    return 0;
}

int readCards(char allCards[])
{
    int counter = 0;
    char card[3] = { '\0' };
    char endOfStr = 0;
    int instance = 1;
    while (counter < 9) {
        endOfStr = scanf("%s", card);
        if (counter == 0 && (int) endOfStr == EOF)
            return EOF;
        if (checkCard(card, allCards, instance)) {
            strcat(allCards, card);
            counter++;
        } else {
            return 1;
        }
        if (counter == 2 || counter == 4 || counter == 9) {
            scanf("%c", &endOfStr);
            if (endOfStr != '\n') {
                fprintf(stderr, "vInstance %d have invalid format %d %s\n", instance, counter, card);
                return 1;
            }
            instance++;
        }
    }
    return 0;
}

int checkCard(char card[], char allCards[], int instance)
{
    if (strlen(card) != 2) {
        fprintf(stderr, "Instance %d: card %s is invalid\n", instance, card);
        return 0;
    }
    if (strstr(allCards, card) != NULL) {
        fprintf(stderr, "Instance %d: card %s has already been used\n", instance, card);
        return 0;
    }
    if (strchr("2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A", card[0]) == NULL) {
        fprintf(stderr, "Instance %d: card´s value %c is invalid\n", instance, card[0]);
        return 0;
    }
    if (strchr("h, d, s, c", card[1]) == NULL) {
        fprintf(stderr, "Instance %d: card´s suits %c is invalid\n", instance, card[1]);
        return 0;
    }
    return 1;
}


int comp(const void *i, const void *j)
{
    int ii = ((char *) i)[0];
    int jj = ((char *) j)[0];
    if (ii == jj)
        return 0;
    if ((int) ii >= 65 && (int) jj >= 65) {
        if (ii == 'A')
            return -1; //A
        if (jj == 'A')
            return 1;
        if (ii == 'K')
            return -1; //K
        if (jj == 'K')
            return 1;
        if (ii == 'Q')
            return -1; //Q
        if (jj == 'Q')
            return 1;
        if (ii == 'J')
            return -1; //J
        if (jj == 'J')
            return 1;
    }
    return jj - ii;
}

int cardToNumber(char cardValue)
{
    if (cardValue == 'T')
        return 10;
    if (cardValue == 'J')
        return 11;
    if (cardValue == 'Q')
        return 12;
    if (cardValue == 'K')
        return 13;
    if (cardValue == 'A')
        return 14;
    return cardValue - '0';
}

char numberToCard(int cardNumber)
{
    if (cardNumber == 10)
        return 'T';
    if (cardNumber == 11)
        return 'J';
    if (cardNumber == 12)
        return 'Q';
    if (cardNumber == 13)
        return 'K';
    if (cardNumber == 14)
        return 'A';
    return cardNumber + '0';
}

int isOneSuit(char combination[])
{
    for (size_t i = 3; i < strlen(combination); i += 2) {
        if (combination[1] != combination[i])
            return 0;
    }
    return 1;
}

int straight(char gamerCards[], char combinationC[])
{
    int current = 0;
    const char *pointers[10] = { "\0" };
    int result = 1;
    char combination[11] = { '\0' };
    int highestCard = 0;
    for (int i = 0; i < 8; i += 2) {
        current = cardToNumber(gamerCards[i]);
        if (current >= 5 && (current >= highestCard || result != 9)) {
            int oneSuit = 1; //check is straight flush
            int j = 0;
            char card[3] = { "\0" };
            card[1] = gamerCards[i + 1];
            for (; j < 4; j++) {
                if (oneSuit) {
                    if (current == 5 && j == 3) {
                        card[0] = 'A';
                        pointers[j] = strstr(gamerCards, card);
                    } else {
                        card[0] = numberToCard(current - j - 1);
                        pointers[j] = strstr(gamerCards + i, card);
                    }
                    if (pointers[j] == NULL)
                        oneSuit = 0;
                }
                if (!oneSuit) {
                    if (current == 5 && j == 3)
                        pointers[j] = strchr(gamerCards, 'A');
                    else
                        pointers[j] = strchr(gamerCards + i, numberToCard(current - j - 1));
                    if (pointers[j] == NULL)
                        break;
                }
            }
            if (j != 4)
                continue;
            combination[0] = gamerCards[i];
            combination[1] = gamerCards[i + 1];
            for (int j = 2; j < 10; j += 2) {
                combination[j] = *pointers[j / 2 - 1];
                combination[j + 1] = *(pointers[j / 2 - 1] + 1);
            }
            if (result == 1 || oneSuit ||
                    (result == 4 && cardToNumber(combination[2]) > cardToNumber(combinationC[2]))) {
                strcpy(combinationC, combination);
                if (oneSuit)
                    result = 9;
                else
                    result = 4;
            }
            highestCard = cardToNumber(combinationC[0]);
        }
    }
    return result;
}


int pair(char gamerCard[], char combination[])
{
    int length = strlen(combination);
    int up = -1;
    int down = length;
    if (length != 0) { // Three of a kind or other pair in combination
        down = strchr(gamerCard, combination[0]) - gamerCard;
        up = down + length;
    }
    for (size_t i = 0; i < strlen(gamerCard) - 3; i += 2) {
        if (down <= (int) i && (int) i < up)
            continue;
        char *result = strchr(gamerCard + i + 2, gamerCard[i]);
        if (result != NULL) {
            combination[length] = gamerCard[i];
            combination[length + 1] = gamerCard[i + 1];
            combination[length + 2] = result[0];
            combination[length + 3] = result[1];
            return 1;
        }
    }
    return 0;
}

int twoPair(char gamerCard[], char combination[])
{
    if (pair(gamerCard, combination) && (strlen(combination) == 8 || pair(gamerCard, combination))) {
        return 1;
    }
    return 0;
}

int threeOfKind(char gamerCard[], char combination[])
{
    const char *pointers[5] = { "\0" }; // pointers of card, which is needed
    for (size_t i = 0; i < strlen(gamerCard) - 4; i += 2) {
        int j = 0;
        int current = i;
        for (; j < 2; j++) {
            pointers[j] = strchr(gamerCard + current + 1, gamerCard[i]); // look for sign we need in list
            if (pointers[j] == NULL)
                break;
            current += 2; // list of gamer cards is sorted and same card is near
        }
        if (j != 2)
            continue;
        combination[0] = gamerCard[i];
        combination[1] = gamerCard[i + 1];
        for (int j = 2; j < 6; j += 2) {
            combination[j] = *pointers[j / 2 - 1];
            combination[j + 1] = *(pointers[j / 2 - 1] + 1);
        }
        return 1;
    }
    return 0;
}

int fourOfKind(char gamerCard[], char combination[])
{
    for (size_t i = 0; i < strlen(gamerCard) - 6; i += 2) {
        char currentCombination[15] = { '\0' };
        if (threeOfKind(gamerCard + i, currentCombination)) {
            char *pointer = strchr(gamerCard, currentCombination[0]) + 6;
            if (currentCombination[0] == *pointer) {
                currentCombination[6] = *pointer;
                currentCombination[7] = *(pointer + 1);
                strcpy(combination, currentCombination);
                return 1;
            }
        }
    }
    return 0;
}

int highCard(char gamerCard[], char combination[])
{
    // function add up combination to five cards
    // function ignor card, which is in combination
    int lenght = strlen(combination);
    int position = 0;
    while (lenght != 10) {
        for (int i = position; i < 14; i += 2) {
            char *pointer = strchr(combination, gamerCard[i]);
            if (pointer != NULL)
                continue;
            combination[lenght] = gamerCard[i];
            combination[lenght + 1] = gamerCard[i + 1];
            lenght += 2;
            position = i;
            break;
        }
    }
    return 1;
}

int fullHouse(char gamerCard[], char combination[])
{
    // threeOfKind change combination, for this reason pair work correct.
    char currentCombination[15] = { '\0' };
    if (threeOfKind(gamerCard, currentCombination) &&
            pair(gamerCard, currentCombination)) {
        strcpy(combination, currentCombination);
        return 1;
    }
    return 0;
}

int flush(char gamerCard[], char combination[])
{
    const char *pointers[5] = { "\0" }; // pointers of card, which is needed
    for (size_t i = 1; i < strlen(gamerCard) - 6; i += 2) {
        int j = 0;
        int current = i;
        for (; j < 4; j++) {
            if (current >= 13) // last card
                break;
            pointers[j] = strchr(gamerCard + current + 2, gamerCard[i]);
            if (pointers[j] == NULL)
                break;
            current = pointers[j] - gamerCard;
        }
        if (j != 4)
            continue;
        combination[0] = gamerCard[i - 1];
        combination[1] = gamerCard[i];
        for (int k = 2; k < 10; k += 2) {
            int sign = pointers[k / 2 - 1] - gamerCard - 1;
            combination[k] = gamerCard[sign];
            combination[k + 1] = combination[1];
        }
        return 1;
    }
    return 0;
}


int checkCombination(char gamerCard[], char combination[])
{
    // result is number of combination, where higest combination is straight flush = 9, lower - highCard = 1
    int result = straight(gamerCard, combination);
    if (result == 9)
        return 9; // Straight flush
    if (fourOfKind(gamerCard, combination))
        result = 8; // Four of a kind
    if (result != 8 && fullHouse(gamerCard, combination))
        return 7; // Full house
    if (result != 8 && flush(gamerCard, combination))
        return 6; //Flush
    if (result != 8 && strlen(combination) == 10)
        return 5; // Straight, combination is collected by straight flush
    if (result != 8 && threeOfKind(gamerCard, combination))
        result = 4; // Three of a kind
    if (result != 8 && twoPair(gamerCard, combination))
        result = 3; //Two pair
    if (strlen(combination) == 4)
        result = 2;                   //Pair, combination is collected by twoPair
    highCard(gamerCard, combination); //High card
    return result;
}
