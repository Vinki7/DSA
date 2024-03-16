#include <stdio.h>
#include <stdlib.h>

typedef struct node{ 
    long long id;
    char firstname[22];
    char lastname[22];
    char birthday[16];
    int height;

    struct node *rightChild;   
    struct node *leftChild;    
}Node;

int height(Node* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

int updateHeight(Node* node) {
    if (node == NULL) {
        return 0;
    }
    int leftChildHeight = height(node->leftChild);
    int rightChildHeight = height(node->rightChild);
    node->height = (leftChildHeight > rightChildHeight ? leftChildHeight : rightChildHeight) + 1;
    return node->height;
}

Node* rightRotation(Node* subtreeParent){
    Node* subLeftChild = subtreeParent->leftChild;
    Node* subSubRightChild = subLeftChild->rightChild;

    subLeftChild->rightChild = subtreeParent;
    subtreeParent->leftChild = subSubRightChild;
    updateHeight(subtreeParent);
    updateHeight(subLeftChild);


    return subLeftChild;
}

Node* leftRotation(Node* subtreeParent){
    Node* subRightChild = subtreeParent->rightChild;
    Node* subSubLeftChild = subRightChild->leftChild;


    subRightChild->leftChild = subtreeParent;
    subtreeParent->rightChild = subSubLeftChild;


    updateHeight(subtreeParent);
    updateHeight(subRightChild);

    return subRightChild;
}

int getBalanceCoeficient(Node* currentNode){
    if (currentNode == NULL)
    {
        return 0;
    }
    return updateHeight(currentNode->leftChild) - updateHeight(currentNode->rightChild);
}

Node* insert(Node* root, long long inputId, char* inputFirstName, char* inputLastName, char* inputBirthday){
    if (root == NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->id = inputId;

        snprintf(newNode->lastname, sizeof(newNode->lastname), "%s", inputLastName);
        snprintf(newNode->firstname, sizeof(newNode->firstname), "%s", inputFirstName);
        snprintf(newNode->birthday, sizeof(newNode->birthday), "%s", inputBirthday);
        newNode->height = 1;
        newNode->leftChild = NULL;
        newNode->rightChild = NULL;
        return newNode;
    }

    if (inputId < root->id) {
        root->leftChild = insert(root->leftChild, inputId, inputFirstName, inputLastName, inputBirthday);
    } else if (inputId > root->id) {
        root->rightChild = insert(root->rightChild, inputId, inputFirstName, inputLastName, inputBirthday);
    } else {
        //printf("Duplicite ID: %lld\n", inputId);
        return root;
    }
    
    updateHeight(root);

    int balance = getBalanceCoeficient(root);

    if (balance > 1 && inputId < root->leftChild->id){
        return rightRotation(root);
    }
    
    if (balance < -1 && inputId > root->rightChild->id){
        return leftRotation(root);
    }

    if (balance > 1 && inputId > root->leftChild->id){
        root->leftChild = leftRotation(root->leftChild);
        return rightRotation(root);
    }

    if (balance < -1 && inputId < root->rightChild->id){
        root->rightChild = rightRotation(root->rightChild);
        return leftRotation(root);
    }

    return root;
}

void freeTree(Node* root){
    if(root == NULL){
        return;
    }
    freeTree(root->leftChild);
    freeTree(root->rightChild);
    free(root);
}

void searchNode(Node* root, long long leftIntervalId, long long rightIntervalId, int* firstOutputFlag) {
    if (root == NULL) {
        return;
    }

    if (root->id > leftIntervalId) { // Traverse left subtree if the node's ID is greater than the left interval ID
        searchNode(root->leftChild, leftIntervalId, rightIntervalId, firstOutputFlag);
    }

    if (root->id >= leftIntervalId && root->id <= rightIntervalId) { // Output node if its ID falls within the interval
        if (*firstOutputFlag == 0) {
            *firstOutputFlag = 1;
            printf("%lld %s %s %s", root->id, root->firstname, root->lastname, root->birthday);
        } else {
            printf("\n%lld %s %s %s", root->id, root->firstname, root->lastname, root->birthday);
        }
    }

    if (root->id < rightIntervalId) { // Traverse right subtree if the node's ID is less than the right interval ID
        searchNode(root->rightChild, leftIntervalId, rightIntervalId, firstOutputFlag);
    }
}

Node* lowestValue(Node* node) {
    Node* current = node;
    // Loop down to find the leftmost leaf
    while (current && current->leftChild != NULL)
        current = current->leftChild;
    return current;
}

Node* nodeEvaporation(Node* root, long long id) {
    if (root == NULL)
        return root;

    // If the value to be deleted is smaller than the root's value,
    // then it lies in the left subtree
    if (id < root->id)
        root->leftChild = nodeEvaporation(root->leftChild, id);

    // If the value to be deleted is greater than the root's value,
    // then it lies in the right subtree
    else if (id > root->id)
        root->rightChild = nodeEvaporation(root->rightChild, id);

    // If the value to be deleted is the same as the root's value, then this is the node to be deleted
    else {
        // Node with only one child or no child
        if (root->leftChild == NULL) {
            Node* temp = root->rightChild;
            free(root);
            return temp;
        } else if (root->rightChild == NULL) {
            Node* temp = root->leftChild;
            free(root);
            return temp;
        }

        // Node with two children: Get the inorder successor (smallest in the right subtree)
        Node* temp = lowestValue(root->rightChild);

        // Copy the inorder successor's content to this node
        root->id = temp->id;
        snprintf(root->lastname, sizeof(root->lastname), "%s", temp->lastname);
        snprintf(root->firstname, sizeof(root->firstname), "%s", temp->firstname);
        snprintf(root->birthday, sizeof(root->birthday), "%s", temp->birthday);

        // Delete the inorder successor
        root->rightChild = nodeEvaporation(root->rightChild, temp->id);
    }
    // Update height
    root->height = updateHeight(root);

    // Balance the tree
    int balance = getBalanceCoeficient(root);

    // Left Left Case
    if (balance > 1 && getBalanceCoeficient(root->leftChild) >= 0){
        return rightRotation(root);
    }
    
    // Left Right Case
    if (balance > 1 && getBalanceCoeficient(root->leftChild) < 0) {
        root->leftChild = leftRotation(root->leftChild);
        return rightRotation(root);
    }

    // Right Right Case
    if (balance < -1 && getBalanceCoeficient(root->rightChild) <= 0){
        return leftRotation(root);
    }

    // Right Left Case
    if (balance < -1 && getBalanceCoeficient(root->rightChild) > 0) {
        root->rightChild = rightRotation(root->rightChild);
        return leftRotation(root);
    }

    return root;
}

int main(void){
    Node* avlTree = NULL;
    long long inputId;
    int firstOutputFlag = 0;
    char line[1024], inputFirstname[24], inputLastName[24], inputBirthDay[16], operation;
    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        if (sscanf(line, "%c", &operation) < 1){
            continue;
        }

        switch (operation)
        {
        case 'i':
            sscanf(line+2, "%lld %23s %23s %15s", &inputId, inputFirstname, inputLastName, inputBirthDay);
            avlTree = insert(avlTree, inputId, inputFirstname, inputLastName, inputBirthDay);
            break;
        
        case 's':
            long long leftIntervalId, rightIntervalId;
            int numItemsRead = sscanf(line + 2, "%lld %lld", &leftIntervalId, &rightIntervalId);
            if (numItemsRead < 1) {
                printf("Invalid input for search operation.\n");
                break;
            }
            if (numItemsRead == 1) {
                rightIntervalId = leftIntervalId; // If only one ID is provided, set it to both left and right IDs
            }
            if (rightIntervalId < leftIntervalId) {
                long long temp = leftIntervalId;
                leftIntervalId = rightIntervalId;
                rightIntervalId = temp;
            }
            searchNode(avlTree, leftIntervalId, rightIntervalId, &firstOutputFlag);
            break;
        
        case 'd':
            long long deleteId;
            sscanf(line + 2, "%lld", &deleteId);
            avlTree = nodeEvaporation(avlTree, deleteId);
            break;
        default:
            break;
        }
    }
    freeTree(avlTree);
    return 0;
}