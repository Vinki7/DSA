#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 4096
#define FIRSTNAME_SIZE 20
#define LASTNAME_SIZE 20
#define DATE_OF_BIRTH 11
#define LINE_LEN 200

// Holds the data of an account
typedef struct client
{
    // Data
    char firstname[FIRSTNAME_SIZE];
    char lastname[LASTNAME_SIZE];
    char dateOfBirth[DATE_OF_BIRTH];
    double ballance;
}Client;

typedef struct hashNode{
    Client value;
    int key;
    struct hashNode* next;
}HashNode;
/*
* Elementary functions ----------------------------------------------------------------------------------
*/
// Return length of the provided string
int customStringLength(char *inputString) {
    int len = 0;
    while (inputString[len] != '\0') {
        len++;
    }
    return len;
}

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



// Custom atof function
double customAtolf(char *ballanceValidation) {
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

void customStringCopy(char *result, char *source) {// not used in my code actually, snprintf() works better
    while (*source) {
        *result = *source;
        *result++;
        source++;
    }
    *result = '\0'; // Add null  to the end of the destination string
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

/*
* functions used for hash table manipulation ------------------------------------------------------------
*/

void freeTable(HashNode *hashTable[]){
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

// Insert a new node into the hash table, returns 1 when unsuccessful
int insertHashNode(HashNode **hashTable, char *firstname, char *lastname, char *dateOfBirth, double ballance, int key){
    
    HashNode *validation = searchNode(hashTable, firstname, lastname, dateOfBirth, key);
    if (validation != NULL)
    {
        return 1;
    }else{
        if (hashTable[key] == NULL)
        {
            // Alloc. the first node of the cluster
            hashTable[key] = (HashNode *)malloc(sizeof(HashNode));
            if (hashTable[key] == NULL) {
                // Handle memory alloc. failure
                return 1;
            }
            // Define next node address
            hashTable[key]->next = NULL;
            // Store key
            hashTable[key]->key = key;
            // Value part
            snprintf(hashTable[key]->value.firstname, FIRSTNAME_SIZE, "%s", firstname);
            snprintf(hashTable[key]->value.lastname, LASTNAME_SIZE, "%s", lastname);
            snprintf(hashTable[key]->value.dateOfBirth, DATE_OF_BIRTH, "%s", dateOfBirth);
            hashTable[key]->value.ballance = ballance;

        }else{
            // Chaining
            
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
            snprintf(currentNodePtr->value.firstname, FIRSTNAME_SIZE, "%s", firstname);
            snprintf(currentNodePtr->value.lastname, LASTNAME_SIZE, "%s", lastname);
            snprintf(currentNodePtr->value.dateOfBirth, DATE_OF_BIRTH, "%s", dateOfBirth);
            currentNodePtr->value.ballance = ballance;
        }

    }
    return 0;
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
                customStringCompare(currentNodePtr->value.dateOfBirth, dateOfBirth) == 0) {
                prevNodePtr->next = currentNodePtr->next;
                free(currentNodePtr);
                currentNodePtr = NULL;
                return 0;
            }
            prevNodePtr = currentNodePtr;
            currentNodePtr = currentNodePtr->next;
        }
    }
    return 1; // Return NULL if the node is not found
}


char *doubleToString(double value) {// format handling
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


int updateBallance(HashNode **hashTable, char *firstname, char *lastname, char *dateOfBirth, double ballance, int key){
    HashNode *searchedNode = searchNode(hashTable, firstname, lastname, dateOfBirth, key);
    if (searchedNode != NULL)
    {
        if ((searchedNode->value.ballance + ballance) >= 0)// limit handling (balance >= 0)
        {
            searchedNode->value.ballance += ballance;
            return 0;
        }else{
            return 1;
        }
    }
    return 1;
}


int main(void){
    
    char line[LINE_LEN];
    char opCode;
    HashNode *hashTable[HASH_TABLE_SIZE] = {NULL};
    char firstname[FIRSTNAME_SIZE];
    char lastname[LASTNAME_SIZE];
    char dateOfBirth[DATE_OF_BIRTH];
    char ballanceValidation[40];
    double ballance;
    int firstOutput = 1;

    while (fgets(line, sizeof(line), stdin) != NULL){// read input with every iteration, break on end of input
        sscanf(line, "%c", &opCode);// Get the operation code
        int wrongIput = 0;
        switch (opCode)
        {
        case 'i':
            sscanf(line+2, "%s %s %s %s", firstname, lastname, dateOfBirth, ballanceValidation);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0' && ballanceValidation[0] != '\0')
            {
                ballance = customAtolf(ballanceValidation);// convert
                wrongIput = insertHashNode(hashTable, firstname, lastname, dateOfBirth, ballance, hashFunction(firstname, lastname, dateOfBirth));
            }else{
                wrongIput = 1;
            }
            // ↓↓↓ failure handled ↓↓↓
            if (wrongIput == 1)
            {
                if (firstOutput == 1)
                {
                    printf("insert failed");
                    firstOutput = 0;
                }else{
                    printf("\ninsert failed");
                }
            }

            break;
        
        case 's':
            sscanf(line+2, "%s %s %s", firstname, lastname, dateOfBirth);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0')
            {
                HashNode *searchedNode = searchNode(hashTable, firstname, lastname, dateOfBirth, hashFunction(firstname, lastname, dateOfBirth));
                if (searchedNode != NULL)// If the search was successful
                {
                    if (firstOutput == 1)
                    {
                        printf("%s", doubleToString(searchedNode->value.ballance));
                        firstOutput = 0;
                    }else{
                        printf("\n%s", doubleToString(searchedNode->value.ballance));
                    }
                // ↓↓↓ failure handled ↓↓↓
                }else{
                    if (firstOutput == 1)
                    {
                        printf("search failed");
                        firstOutput = 0;
                    }else{
                        printf("\nsearch failed");
                    }
                    
                }
            }else{
                if (firstOutput == 1)
                {
                    printf("search failed");
                    firstOutput = 0;
                }else{
                    printf("\nsearch failed");
                }
            }
            break;

        case 'd':
            sscanf(line+2, "%s %s %s", firstname, lastname, dateOfBirth);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0')
            {
                wrongIput = removeHashNode(hashTable, firstname, lastname, dateOfBirth, hashFunction(firstname, lastname, dateOfBirth));// success
            }else{
                wrongIput = 1;
            }
            if (wrongIput == 1)// failure
            {
                if (firstOutput == 1)
                {
                    printf("delete failed");
                    firstOutput = 0;
                }else{
                    printf("\ndelete failed");
                }
            }
            break;
        
        case 'u': //update
            sscanf(line+2, "%s %s %s %s", firstname, lastname, dateOfBirth, ballanceValidation);
            if (firstname[0] != '\0' && lastname[0] != '\0' && dateOfBirth[0] != '\0' && ballanceValidation[0] != '\0')//validation
            {
                ballance = customAtolf(ballanceValidation);
                wrongIput = updateBallance(hashTable, firstname, lastname, dateOfBirth, ballance, hashFunction(firstname, lastname, dateOfBirth));
            }else{
                wrongIput = 1;
            }
            if (wrongIput == 1)// failure
            {
                if (firstOutput == 1)
                {
                    printf("update failed");
                    firstOutput = 0;
                }else{
                    printf("\nupdate failed");
                }
            }
            break;
        
        default:
            break;
        }

    }

    freeTable(hashTable);

    return 0;
}