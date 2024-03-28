#include <stdlib.h>
#include <stdio.h>
#define HASH_TABLE_SIZE 10

typedef struct client{// Data structure that holds the data of the client - storage of the data
    char firstName[20];// Represents the first part of the key
    char lastName[20];// Represents the second part of the key
    char dateOfBirth[11];
    float ballance;
}Client;

typedef struct hashNode{
    Client value;
    struct hashNode *next;
    int key;
}HashNode;


int customStrLen(char *str){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

void customStringCopy(char *result, const char *source) {
    while (*source) {
        *result = *source;
        result++;
        source++;
    }
    *result = '\0'; // Add null terminator to the end of the destination string
}

int customStringCompare(char *firstString,char *secondString) {
    while (*firstString && *secondString) {
        if (*firstString != *secondString) {
            return *firstString - *secondString;
        }
        firstString++;
        secondString++;
    }
    // Strings are equal so far, check if one string is longer than the other
    if (*firstString == '\0' && *secondString == '\0') {
        return 0; // Both strings are equal
    } else if (*firstString == '\0') {
        return -1; // firstString is shorter than secondString
    } else {
        return 1; // secondString is shorter than firstString
    }
}

void freeTable(HashNode *hashTable[]){
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *current = hashTable[i];
        while (current != NULL) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
}

// Hash function returns the index of the new key, encapsulates the logic of the hash function
int hashFunction(char *firstname, char *lastname){
    int sumOfCharValues = 0;
    for (int i = 0; i < customStrLen(firstname); i++) {
        sumOfCharValues += firstname[i];
    }
    for (int i = 0; i < customStrLen(lastname); i++) {
        sumOfCharValues += lastname[i];
    }
    return sumOfCharValues % HASH_TABLE_SIZE;
}

void insertHashNode(HashNode **hashTable, char *firstname, char *lastname, char *dateOfBirth, float ballance, int key){
    
    if (hashTable[key] == NULL)
    {
        // Alloc. the first node of the cluster
        hashTable[key] = (HashNode *)malloc(sizeof(HashNode));
        // Define next node address
        hashTable[key]->next = NULL;
        // Store key
        hashTable[key]->key = key;
        // Value part
        customStringCopy(hashTable[key]->value.firstName, firstname);
        customStringCopy(hashTable[key]->value.lastName, lastname);
        customStringCopy(hashTable[key]->value.dateOfBirth, dateOfBirth);
        hashTable[key]->value.ballance = ballance;
    }else{
        HashNode *currentNodePtr = hashTable[key];
        while (currentNodePtr->next != NULL)
        {
            currentNodePtr = currentNodePtr->next;
        }
        currentNodePtr->next = (HashNode *)malloc(sizeof(HashNode));
        currentNodePtr = currentNodePtr->next;
        // Set pointer to the next element to NULL 
        currentNodePtr->next = NULL;       
        // Store key
        currentNodePtr->key = key;
        // Value part
        customStringCopy(currentNodePtr->value.firstName, firstname);
        customStringCopy(currentNodePtr->value.lastName, lastname);
        customStringCopy(currentNodePtr->value.dateOfBirth, dateOfBirth);
        currentNodePtr->value.ballance = ballance;
    }
}


HashNode* searchNode(HashNode **hashTable, char *firstname, char* lastname, char *dateOfBirth, int key){

    HashNode *searchedNode = NULL;
    HashNode *currentNodePtr = hashTable[key];
    while (currentNodePtr != NULL)
    {
        if (customStringCompare(currentNodePtr->value.firstName, firstname) == 0 && customStringCompare(currentNodePtr->value.lastName, lastname) == 0 && customStringCompare(currentNodePtr->value.dateOfBirth, dateOfBirth) == 0)
        {
            searchedNode = currentNodePtr;
            return searchedNode;
        }
        currentNodePtr = currentNodePtr->next;
    }
    return searchedNode;
}


int main(void){
    
    char line[100];
    char command;
    HashNode *hashTable[HASH_TABLE_SIZE] = {NULL};
    int numberOfOutputs = 0;

    while (fgets(line, sizeof(line), stdin) != NULL)// read the input line by line
    {
        if (sscanf(line, "%c", &command) < 1)// nothing at the input -> skip current iteration
        {
            continue;
        }
        
        switch (command)
        {
        case 'i':
            char firstname[20];
            char lastname[20];
            char dateOfBirth[11];
            int wholePart;
            float decimalPart;
            sscanf(line+2, "%s %s %s %d,%f", firstname, lastname, dateOfBirth, &wholePart, &decimalPart);
            float ballance = (float)wholePart + (decimalPart/100);
            int key = hashFunction(firstname, lastname);
            insertHashNode(hashTable , firstname, lastname, dateOfBirth, ballance, key);
//            printf("\n%.2f\n%d", (*hashTable)[key].value.ballance, (*hashTable)[key].key);

            break;
        case 's':
            char firstname[20];
            char lastname[20];
            char dateOfBirth[11];
            sscanf(line+2, "%s %s %s", firstname, lastname, dateOfBirth);
            int key = hashFunction(firstname, lastname);
            HashNode *searchedNode = searchNode(hashTable , firstname, lastname, dateOfBirth, key);
            if (searchedNode != NULL)
            {
                if (numberOfOutputs == 0)
                {
                    printf("%.2f", searchedNode->value.ballance);
                    numberOfOutputs++;
                }else{
                    printf("\n%.2f", searchedNode->value.ballance);
                }
            }else{
                if (numberOfOutputs == 0)
                {
                    printf("search failed");
                    numberOfOutputs++;
                }else{
                    printf("\nsearch failed");
                }
            }
            
            break;
        case 'u':
            // update
            break;
        case 'd':
            // delete
            break;
        default:
            break;
        }
    }

    freeTable(hashTable);

    return 0;
}