#include <stdlib.h>
#include <stdio.h>

#define SIZE_OF_TABLE 10000// Size of the hash table -> maybe it would be better to handle the size of the table dynamically for better resource handling

// Client
typedef struct client{// Data structure that holds the data of the client - storage of the data
    char firstName[20];// Represents the first part of the key
    char lastName[20];// Represents the second part of the key
    char dateOfBirth[11];
    float ballance;
}Client;

// Hash Table
typedef struct hashTable{// Works as a key-value pair | key is the identificator (based on full name and last name) and value is the client (data)
    Client value;
    struct hashTable *next;
}HashTable;

// Custom string length function to get the length of the string
int customStrLen(char *str){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

// Custom string copy function to copy the string from source to destination
void customStrCpy(char *result, const char *source) {
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



// Hash function returns the index of the new key
int hashFunction(char *firstName, char *lastname){// Hash function that returns the index of the key
    int sum = 0;
    for(int i = 0; i < customStrLen(firstName); i++){
        sum += firstName[i];
    }
    for (int i = 0; i < customStrLen(lastname); i++)
    {
        sum += lastname[i];
    }
    
    return sum % SIZE_OF_TABLE;
}


void insert(HashTable *table, Client inputClient, int *numberOfElements) {
    // Compute hash position
    int hashPosition = hashFunction(inputClient.firstName, inputClient.lastName);
    
    // Create a new hash table entry
    HashTable *newClient = (HashTable*)malloc(sizeof(HashTable));
    if (newClient == NULL) {
        printf("insertion failed");
        return;
    }
    
    // Copy client data into the new node
    customStrCpy(newClient->value.firstName, inputClient.firstName);
    customStrCpy(newClient->value.lastName, inputClient.lastName);
    customStrCpy(newClient->value.dateOfBirth, inputClient.dateOfBirth);
    newClient->value.ballance = inputClient.ballance;
    newClient->next = NULL;

    // Insert the entry into the hash table
    HashTable *currentElement = &table[hashPosition];
    if (currentElement->value.firstName[0] == '\0' && currentElement->value.lastName[0] == '\0')
    {
        customStrCpy(currentElement->value.firstName, inputClient.firstName);
        customStrCpy(currentElement->value.lastName, inputClient.lastName);
        customStrCpy(currentElement->value.dateOfBirth, inputClient.dateOfBirth);
        currentElement->value.ballance = inputClient.ballance;
    }else {
        // Create a new hash table entry
        HashTable *newClient = (HashTable*)malloc(sizeof(HashTable));
        if (newClient == NULL) {
            printf("insertion failed");
            return;
        }
        
        // Copy client data into the new node
        customStrCpy(newClient->value.firstName, inputClient.firstName);
        customStrCpy(newClient->value.lastName, inputClient.lastName);
        customStrCpy(newClient->value.dateOfBirth, inputClient.dateOfBirth);
        newClient->value.ballance = inputClient.ballance;
        newClient->next = NULL;

        if (currentElement->next == NULL) {
        currentElement->next = newClient;
        } else {
            // Handle collision: Traverse the linked list until the last node and append the new node
            while (currentElement->next != NULL) {
                currentElement = currentElement->next;
            }
            currentElement->next = newClient;
        }
    }
    
    // Increment the number of elements
    (*numberOfElements)++;
}



// Search function - searches for the client in the hash table
Client* searchForClient(HashTable *table, Client *client) {
    int clusterAddress = hashFunction(client->firstName, client->lastName);
    HashTable *currentClientPtr = &table[clusterAddress];
    int match = 0;
    do {
        if (customStringCompare(currentClientPtr->value.firstName, client->firstName) == 0 &&
            customStringCompare(currentClientPtr->value.lastName, client->lastName) == 0 &&
            customStringCompare(currentClientPtr->value.dateOfBirth, client->dateOfBirth) == 0) {
            // Free the client argument before returning
            free(client);
            return &currentClientPtr->value;
        }
        currentClientPtr = currentClientPtr->next;
    }while (currentClientPtr->next != NULL);
    // Free the client argument since it's not found
    free(client);
    return NULL;
}


int main(){

    char command;
    char line[100];
    int numberOfElements = 0;
    HashTable hashTable[SIZE_OF_TABLE];
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
            // insert
            Client *insertClient = (Client*)malloc(sizeof(Client));
            long int wholePart;
            int decimalPart;
            //i Thomas Sanford 1.1.2001 2017,81
            sscanf(line+2, "%s %s %11s %ld,%d", insertClient->firstName, insertClient->lastName, insertClient->dateOfBirth, &wholePart, &decimalPart);// parse for insertion
            insertClient->ballance = wholePart + (float)decimalPart/100;// or maybe it would be better to handle the ballance in cents not in euros
            insert(hashTable, *insertClient, &numberOfElements);// here it calls incompatible pointer type
            free(insertClient);
            break;
        case 's':
            // search
            Client *potentialClient = (Client*)malloc(sizeof(Client));
            sscanf(line+2, "%s %s %s", potentialClient->firstName, potentialClient->lastName, potentialClient->dateOfBirth);
            potentialClient = searchForClient(hashTable, potentialClient);
            if (potentialClient != NULL)
            {
                if (numberOfOutputs == 0)
                {
                    printf("%.2f", potentialClient->ballance);
                    numberOfOutputs++;
                }else{
                    printf("\n%.2f", potentialClient->ballance);
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
    
}
