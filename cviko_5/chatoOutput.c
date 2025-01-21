#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    long long id;
    char firstname[24];
    char lastname[24];
    char birthDay[16];
    int height;
    struct node *rightChild;
    struct node *leftChild;
} Node;

// Helper function to calculate the height of a node
int getHeight(Node* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// Helper function to update the height of a node
void updateHeight(Node* node) {
    if (node == NULL) {
        return;
    }
    int leftHeight = getHeight(node->leftChild);
    int rightHeight = getHeight(node->rightChild);
    node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}


// Helper function to calculate the balance factor of a node
int getBalanceFactor(Node* node) {
    if (node == NULL) {
        return 0;
    }
    return getHeight(node->leftChild) - getHeight(node->rightChild);
}

// Helper function to perform a right rotation
Node* rotateRight(Node* y) {
    Node* x = y->leftChild;
    Node* T = x->rightChild;

    // Perform rotation
    x->rightChild = y;
    y->leftChild = T;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    return x;
}

// Helper function to perform a left rotation
Node* rotateLeft(Node* x) {
    Node* y = x->rightChild;
    Node* T = y->leftChild;

    // Perform rotation
    y->leftChild = x;
    x->rightChild = T;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    return y;
}

// Function to insert a new node into the AVL tree
Node* insert(Node* root, long long inputId, char* inputFirstName, char* inputLastName, char* inputBirthday) {
    // Base case: if the tree is empty, create a new node and return it
    if (root == NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        newNode->id = inputId;
        strcpy(newNode->firstname, inputFirstName);
        strcpy(newNode->lastname, inputLastName);
        strcpy(newNode->birthDay, inputBirthday);
        newNode->height = 1;
        newNode->leftChild = NULL;
        newNode->rightChild = NULL;
        return newNode;
    }

    // Recursive insertion
    if (inputId < root->id) {
        root->leftChild = insert(root->leftChild, inputId, inputFirstName, inputLastName, inputBirthday);
    } else if (inputId > root->id) {
        root->rightChild = insert(root->rightChild, inputId, inputFirstName, inputLastName, inputBirthday);
    } else {
        // Duplicate IDs are not allowed
        printf("Duplicate ID: %lld\n", inputId);
        return root;
    }

    // Update the height of the current node
    updateHeight(root);

    // Get the balance factor of the current node
    int balance = getBalanceFactor(root);

    // Left Left Case
    if (balance > 1 && inputId < root->leftChild->id) {
        return rotateRight(root);
    }

    // Right Right Case
    if (balance < -1 && inputId > root->rightChild->id) {
        return rotateLeft(root);
    }

    // Left Right Case
    if (balance > 1 && inputId > root->leftChild->id) {
        root->leftChild = rotateLeft(root->leftChild);
        return rotateRight(root);
    }

    // Right Left Case
    if (balance < -1 && inputId < root->rightChild->id) {
        root->rightChild = rotateRight(root->rightChild);
        return rotateLeft(root);
    }

    // Return the unchanged node pointer
    return root;
}

// Function to free the memory allocated for the AVL tree
void freeAVLTree(Node* root) {
    if (root == NULL) {
        return;
    }
    freeAVLTree(root->leftChild);
    freeAVLTree(root->rightChild);
    free(root);
}

int main(void) {
    // Your main function here
    return 0;
}
