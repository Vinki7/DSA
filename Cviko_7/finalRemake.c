#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 4096
#define FIRSTNAME_SIZE 20
#define LASTNAME_SIZE 20
#define DATE_OF_BIRTH 11
#define LINE_LEN 100

// Holds the data of an account
typedef struct client
{
    // Data
    char firstname[FIRSTNAME_SIZE];
    char lastname[LASTNAME_SIZE];
    char dateOfBirth[DATE_OF_BIRTH];
    double balance;
}Client;

typedef struct hashNode{
    Client value;
    int key;
    struct hashNode* next;
}HashNode;

int hashFunction(const char *firstname, const char *lastname, const char *dateOfBirth) {// djb2 hash function, which is a simple hash function created by Daniel J. Bernstein
    unsigned int hash = 5381; // Initial hash value

    // Hash the firstname
    for (int i = 0; firstname[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + firstname[i]; // Equivalent to: hash * 33 + firstname[i]
    }

    // Hash the lastname
    for (int i = 0; lastname[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + lastname[i]; // Equivalent to: hash * 33 + lastname[i]
    }

    // Hash the dateOfBirth
    for (int i = 0; dateOfBirth[i] != '\0'; i++) {
        if (dateOfBirth[i] != '.') {
            hash = ((hash << 5) + hash) + dateOfBirth[i]; // Equivalent to: hash * 33 + dateOfBirth[i]
        }
    }

    return hash % HASH_TABLE_SIZE; // Ensure the hash fits within the table size
}

/*
* String Formating part--------------------------------------------------------------------
*/

// Custom atof function
double customAtolf(char *ballanceValidation) {// Right version
    double result = 0;
    int i = 0;

    while (ballanceValidation[i] != '\0') {
        if (ballanceValidation[i] == ',') {// Point instead of comma -> "Propper formating" (:
            ballanceValidation[i] = '.';// Replace comma with point
            break;
        }
        i++;
    }
    sscanf(ballanceValidation, "%lf", &result);// Convert string to float
    return result;
}

char* doubleToString(double value) {
    char *result = (char *)malloc(20 * sizeof(char));
    sprintf(result, "%.2lf", value);
    int i = 0;

    while (result[i] != '\0') {
        if (result[i] == '.') {// Point instead of comma -> "Propper formating" (:
            result[i] = ',';// Replace comma with point
            break;
        }
        i++;
    }

    return result;
}

/*
* String operations ---------------------------------------------------------------------------
*/

// Return length of the provided string
int customStringLength(char *inputString) {// Unnecessary
    int len = 0;
    while (inputString[len] != '\0') {
        len++;
    }
    return len;
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

void freeTable(HashNode *hashTable[]){// MAybe unnecessary
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *current = hashTable[i];
        while (current != NULL) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
        hashTable[i] = NULL; // Set hash table entry to NULL after freeing nodes
    }
}


// Returns NULL when unsuccessful
HashNode* searchNode(HashNode **hashTable, char *firstname, char* lastname, char *dateOfBirth, int key){

    HashNode *searchedNode = NULL;
    HashNode *currentNodePtr = hashTable[key];
    while (currentNodePtr != NULL)
    {
        if (customStringCompare(currentNodePtr->value.firstname, firstname) == 0 && customStringCompare(currentNodePtr->value.lastname, lastname) == 0 && customStringCompare(currentNodePtr->value.dateOfBirth, dateOfBirth) == 0)
        {
            searchedNode = currentNodePtr;
            return searchedNode;
        }
        currentNodePtr = currentNodePtr->next;
    }
    return searchedNode;
}

int insertHashNode(HashNode *hashTable[], char* firstname, char* lastname, char* dateOfBirth, double balance, int key) {
    // New node
    HashNode *newNode = (HashNode*)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        
        return 1;
    }
    // Fill in the data
    snprintf(newNode->value.firstname, FIRSTNAME_SIZE, "%s", firstname);
    snprintf(newNode->value.lastname, LASTNAME_SIZE, "%s", lastname);
    snprintf(newNode->value.dateOfBirth, DATE_OF_BIRTH, "%s", dateOfBirth);
    newNode->value.balance = balance;
    newNode->key = key;
    newNode->next = NULL;

    // If the bucket is empty, insert as the first node
    if (hashTable[key] == NULL) {
        hashTable[key] = newNode;
        return 0;
    }

    // Otherwise, traverse the linked list and insert at the end
    HashNode *currentNodePtr = hashTable[key];
    while (currentNodePtr->next != NULL) {
        if (customStringCompare(currentNodePtr->value.firstname, firstname) == 0 &&
            customStringCompare(currentNodePtr->value.lastname, lastname) == 0 &&
            customStringCompare(currentNodePtr->value.dateOfBirth, dateOfBirth) == 0) {
            //free
            free(newNode);
            return 1;
        }
        currentNodePtr = currentNodePtr->next;
    }
    // Insert the new node at the end of the list
    currentNodePtr->next = newNode;
    return 0;
}


int updateBallance(HashNode **hashTable, char *firstname, char *lastname, char *dateOfBirth, double balance, int key){
    HashNode *searchedNode = searchNode(hashTable, firstname, lastname, dateOfBirth, key);
    if (searchedNode != NULL)
    {
        if ((searchedNode->value.balance + balance) >= 0)
        {
            searchedNode->value.balance += balance;
            return 0;
        }else{
            return 1;
        }
    }
    return 1;
}

// Delete a node from the hash table, returns 1 when unsuccessful
int removeHashNode(HashNode **hashTable, char *firstname, char* lastname, char *dateOfBirth, int key){
    HashNode *prevNodePtr = hashTable[key];
    HashNode *currentNodePtr = prevNodePtr ? prevNodePtr->next : NULL; // Check if prevNodePtr is NULL
    
    // Check if the first node matches the target data
    if (prevNodePtr != NULL && customStringCompare(prevNodePtr->value.firstname, firstname) == 0 &&
        customStringCompare(prevNodePtr->value.lastname, lastname) == 0 &&
        customStringCompare(prevNodePtr->value.dateOfBirth, dateOfBirth) == 0) {// Match!
        // Remove the first node
        HashNode *tempPtr = prevNodePtr;
        hashTable[key] = currentNodePtr;
        prevNodePtr = currentNodePtr;
        free(tempPtr);
        tempPtr = NULL;
        return 0; // successful

    } else {
        // Iterate through the linked list to find the node to delete
        while (prevNodePtr != NULL && currentNodePtr != NULL) {
            if (customStringCompare(currentNodePtr->value.firstname, firstname) == 0 &&
                customStringCompare(currentNodePtr->value.lastname, lastname) == 0 &&
                customStringCompare(currentNodePtr->value.dateOfBirth, dateOfBirth) == 0) {// Match!
                prevNodePtr->next = currentNodePtr->next;// Edit pointers
                free(currentNodePtr);
                currentNodePtr = NULL;// ref. to NULL
                return 0;
            }
            prevNodePtr = currentNodePtr;
            currentNodePtr = currentNodePtr->next;
        }
    }
    return 1; // Return NULL if the node is not found
}



int main(void){
    
    char line[LINE_LEN];
    char opCode;
    HashNode *hashTable[HASH_TABLE_SIZE] = {NULL};
    char firstname[FIRSTNAME_SIZE];
    char lastname[LASTNAME_SIZE];
    char dateOfBirth[DATE_OF_BIRTH];
    char ballanceValidation[20];
    double balance;
    int firstOutput = 1;

    while (fgets(line, sizeof(line), stdin) != NULL){
        sscanf(line, "%c", &opCode);// Get the operation code
        int wrongIput = 0;
        switch (opCode)
        {
        case 'i':
            sscanf(line+2, "%s %s %s %s", firstname, lastname, dateOfBirth, ballanceValidation);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0' && ballanceValidation[0] != '\0')
            {
                balance = customAtolf(ballanceValidation);
                wrongIput = insertHashNode(hashTable, firstname, lastname, dateOfBirth, balance, hashFunction(firstname, lastname, dateOfBirth));// insert client and get flag
            }else{
                wrongIput = 1;
            }

            if (wrongIput == 1)
            {
                printf("%sinsert failed", (firstOutput == 0)? "\n": "");// throw err message
                firstOutput = 0;
            }
            break;
        
        case 's':
            sscanf(line+2, "%s %s %s", firstname, lastname, dateOfBirth);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0')
            {
                HashNode *searchedNode = searchNode(hashTable, firstname, lastname, dateOfBirth, hashFunction(firstname, lastname, dateOfBirth));//get node
                if (searchedNode != NULL)// If the search was successful
                {
                    printf("%s%s", (firstOutput == 0)? "\n": "", doubleToString(searchedNode->value.balance));// print out result
                    firstOutput = 0;

                }else{
                    printf("%ssearch failed", (firstOutput == 0)? "\n": "");
                    firstOutput = 0;
                    
                }
            }else{
                printf("%ssearch failed", (firstOutput == 0)? "\n": "");
                firstOutput = 0;
            }
            break;

        case 'd':
            sscanf(line+2, "%s %s %s", firstname, lastname, dateOfBirth);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0')
            {
                wrongIput = removeHashNode(hashTable, firstname, lastname, dateOfBirth, hashFunction(firstname, lastname, dateOfBirth));// make delete
            }else{
                wrongIput = 1;
            }
            if (wrongIput == 1)
            {
                printf("%sdelete failed", (firstOutput == 0)? "\n": "");
                firstOutput = 0;

            }
            break;
        
        case 'u':
            sscanf(line+2, "%s %s %s %s", firstname, lastname, dateOfBirth, ballanceValidation);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0' && ballanceValidation[0] != '\0')
            {
                balance = customAtolf(ballanceValidation);
                wrongIput = updateBallance(hashTable, firstname, lastname, dateOfBirth, balance, hashFunction(firstname, lastname, dateOfBirth));
            }else{
                wrongIput = 1;
            }
            if (wrongIput == 1)
            {
                printf("%supdate failed", (firstOutput == 0)? "\n": "");
                firstOutput = 0;
            }
            break;
        
        default:
            break;
        }

    }

    freeTable(hashTable);

    return 0;
}