#include <stdio.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 10000
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
    struct client *next;
} Client;




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

void myStrCopy(char *dest, const char *src, size_t maxLen) {
    size_t i;
    for (i = 0; i < maxLen - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0'; // Ujistěte se, že je řetězec správně ukončen
}

void printTable(Client *hashTable[]) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        printf("%d: ", i); // Vypíše index hash tabulky
        Client *current = hashTable[i];
        if (current == NULL) {
            printf("NULL"); // Pokud na indexu není žádný klient
        } else {
            // Prochází seznam klientů a vypisuje jejich informace
            while (current != NULL) {
                printf("Clovek (Jmeno: %s, Prijmeni: %s, Datum narozeni: %s, Zůstatek: %.2f) -> ",
                       current->firstname, current->lastname, current->dateOfBirth, current->balance);
                current = current->next;
            }
            printf("NULL"); // Na konci seznamu
        }
        printf("\n"); // Nový řádek po každém indexu
    }
}


// Vložení klienta do hash tabulky
void insertClient(Client *hashTable[], char* firstname, char* lastname, char* dateOfBirth, double balance) {
    int index = hashFunction(firstname, lastname, dateOfBirth);
    Client *newClient = (Client *)malloc(sizeof(Client));
    if (!newClient) {
        perror("Failed to alloc. memory for new client");
        return;
    }

    myStrCopy(newClient->firstname, firstname, FIRSTNAME_SIZE - 1);
    newClient->firstname[FIRSTNAME_SIZE - 1] = '\0';
    myStrCopy(newClient->lastname, lastname, LASTNAME_SIZE - 1);
    newClient->lastname[LASTNAME_SIZE - 1] = '\0';
    myStrCopy(newClient->dateOfBirth, dateOfBirth, DATE_OF_BIRTH - 1);
    newClient->dateOfBirth[DATE_OF_BIRTH - 1] = '\0';
    newClient->balance = balance;
    newClient->next = hashTable[index]; // Vloží na začátek seznamu
    hashTable[index] = newClient;
}

// Vyhledání klienta
Client* searchClient(Client *hashTable[], char* firstname, char* lastname, char* dateOfBirth) {
    int index = hashFunction(firstname, lastname, dateOfBirth);
    for (Client *curr = hashTable[index]; curr != NULL; curr = curr->next) {
        if (customStringCompare(curr->firstname, firstname) == 0 &&
            customStringCompare(curr->lastname, lastname) == 0 &&
            customStringCompare(curr->dateOfBirth, dateOfBirth) == 0) {
            return curr; // Klient nalezen
        }
    }
    return NULL; // Klient nenalezen
}

// Aktualizace zůstatku klienta
void updateBalance(Client *hashTable[], char* firstname, char* lastname, char* dateOfBirth, double newBalance) {
    Client *client = searchClient(hashTable, firstname, lastname, dateOfBirth);
    if (client) {
        client->balance = newBalance; // Aktualizuje zůstatek
    } else {
        printf("Client not found.\n");
    }
}

// Odstranění klienta
void deleteClient(Client *hashTable[], char* firstname, char* lastname, char* dateOfBirth) {
    int index = hashFunction(firstname, lastname, dateOfBirth);
    Client *curr = hashTable[index], *prev = NULL;
    while (curr != NULL) {
        if (customStringCompare(curr->firstname, firstname) == 0 &&
            customStringCompare(curr->lastname, lastname) == 0 &&
            customStringCompare(curr->dateOfBirth, dateOfBirth) == 0) {
            if (prev) {
                prev->next = curr->next; // Odstraní uzel ze seznamu
            } else {
                hashTable[index] = curr->next; // Odstraní první uzel seznamu
            }
            free(curr); // Uvolní paměť klienta
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Client not found.\n");
}


int main(void) {
    char line[LINE_LEN];
    char opCode;
    Client *hashTable[HASH_TABLE_SIZE] = {NULL};
    char firstname[FIRSTNAME_SIZE], lastname[LASTNAME_SIZE], dateOfBirth[DATE_OF_BIRTH], balanceValidation[20];
    double balance;

    while (fgets(line, sizeof(line), stdin)) {
        if (sscanf(line, "%c", &opCode) < 1) continue; // Přeskočit prázdné řádky

        if (opCode == 'q') {
            break; // Ukončit smyčku
        }

        switch (opCode) {
            case 'i':
                if (sscanf(line + 2, "%s %s %s %s", firstname, lastname, dateOfBirth, balanceValidation) == 4) {
                    balance = customAtolf(balanceValidation);
                    insertClient(hashTable, firstname, lastname, dateOfBirth, balance);
                    printf("Client inserted.\n");
                }
                break;
            case 's':
                if (sscanf(line + 2, "%s %s %s", firstname, lastname, dateOfBirth) == 3) {
                    Client *found = searchClient(hashTable, firstname, lastname, dateOfBirth);
                    if (found) {
                        printf("%.2f\n", found->firstname, found->lastname, found->dateOfBirth, found->balance);
                    } else {
                        printf("Client not found.\n");
                    }
                }
                break;
            case 'u':
                if (sscanf(line + 2, "%s %s %s %s", firstname, lastname, dateOfBirth, balanceValidation) == 4) {
                    balance = customAtolf(balanceValidation);
                    updateBalance(hashTable, firstname, lastname, dateOfBirth, balance);
                    printf("Client updated.\n");
                }
                break;
            case 'd':
                if (sscanf(line + 2, "%s %s %s", firstname, lastname, dateOfBirth) == 3) {
                    deleteClient(hashTable, firstname, lastname, dateOfBirth);
                    printf("Client deleted.\n");
                }
                break;
            default:
                //printf("Unknown command.\n");
                break;
        }
    }

    
    
    //printf("Exiting...\n");

    return 0;
}