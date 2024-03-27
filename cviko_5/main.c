#include <stdio.h>
#include <stdlib.h>

typedef struct node{    // node handling
    long long id;
    char firstname[24];
    char lastname[24];
    char birthday[16];
    int height;
    //struct node *parrent;  // parent pointer
    struct node *rightChild;   // higher child
    struct node *leftChild;    // lower child
}Node;

int height(Node* node) {// get the height of the node
    if (node == NULL) {// if the node is NULL, return 0
        return 0;
    }
    return node->height;// return the height of the actuall node
}

int updateHeight(Node* node) { // update the height of the node
    if (node == NULL) {
        return 0;
    }
    int leftChildHeight = height(node->leftChild);
    int rightChildHeight = height(node->rightChild);
    node->height = (leftChildHeight > rightChildHeight ? leftChildHeight : rightChildHeight) + 1;
    return node->height;
}

Node* rightRotation(Node* subtreeParent){
    Node* subLeftChild = subtreeParent->leftChild;// these are just pointers!!! you are not copying the whole subtree, just the pointers to the nodes
    Node* subSubRightChild = subLeftChild->rightChild;

    // Perform the rotation
    subLeftChild->rightChild = subtreeParent;
    /*
              A
            /
          B     -->    B
        /            /  \\
      C            C      A
        \
    */
    subtreeParent->leftChild = subSubRightChild;
    /*
        B
       / \
      C   A
    */
    // Update the heights
    updateHeight(subtreeParent);
    updateHeight(subLeftChild);

    // Return the new root of the subtree (pointer)
    return subLeftChild;
}

Node* leftRotation(Node* subtreeParent){
    Node* subRightChild = subtreeParent->rightChild;
    Node* subSubLeftChild = subRightChild->leftChild;

    // Perform the rotation
    subRightChild->leftChild = subtreeParent;
    subtreeParent->rightChild = subSubLeftChild;

    // Update the heights
    updateHeight(subtreeParent);
    updateHeight(subRightChild);

    return subRightChild;
}

int getBalanceCoeficient(Node* currentNode){// balancing the tree
    if (currentNode == NULL)
    {
        return 0;
    }
    return updateHeight(currentNode->leftChild) - updateHeight(currentNode->rightChild);
}

Node* insert(Node* root, long long inputId, char* inputFirstName, char* inputLastName, char* inputBirthday){// The function works well, fluently, without any issues
    /* Thanks to the recursive approach, we can handle the insertion of the node in the AVL tree in a very simple way. When pointer which is passed
     * to the function is NULL, we can simply allocate the memory for the new node and return it. it will blow up the stack and return the pointer of 
     * the new node to the previous call of the function. This way we can easily handle the insertion of the node to the AVL tree.
    */
    if (root == NULL) { // Base case
        Node* newNode = (Node*)malloc(sizeof(Node));// dynamically allocate the node (root)
        if (newNode == NULL) {
            printf("Memory allocation failure\n");
            exit(EXIT_FAILURE);
        }
        newNode->id = inputId;
        // Copy strings using snprintf, then change the height and children pointers
        snprintf(newNode->lastname, sizeof(newNode->lastname), "%s", inputLastName);
        snprintf(newNode->firstname, sizeof(newNode->firstname), "%s", inputFirstName);
        snprintf(newNode->birthday, sizeof(newNode->birthday), "%s", inputBirthday);
        newNode->height = 1;// the height of the new node
        newNode->leftChild = NULL;
        newNode->rightChild = NULL;
        return newNode;
    }

    // Insert node recursively to the avlTree ==> readability is improved by recursive approach, simulates the real life algorithm behavior
    if (inputId < root->id) {
        root->leftChild = insert(root->leftChild, inputId, inputFirstName, inputLastName, inputBirthday);// try to insert to the left child
    } else if (inputId > root->id) {
        root->rightChild = insert(root->rightChild, inputId, inputFirstName, inputLastName, inputBirthday);// try to insert to the right child
    } else {
        // Duplicate IDs are not allowed
        printf("Duplicate ID: %lld\n", inputId);
        return root;
    }// up to this point, the code should be right, the issue is somewhere else
    
    updateHeight(root);// update the height of the current node

    int balance = getBalanceCoeficient(root);// get the balance coeficient of the current node

    if (balance > 1 && inputId < root->leftChild->id){// left left case
        return rightRotation(root);
    }
    
    if (balance < -1 && inputId > root->rightChild->id){// right right case
        return leftRotation(root);
    }

    if (balance > 1 && inputId > root->leftChild->id){// left right case
        root->leftChild = leftRotation(root->leftChild);
        return rightRotation(root);
    }

    if (balance < -1 && inputId < root->rightChild->id){// right left case
        root->rightChild = rightRotation(root->rightChild);
        return leftRotation(root);
    }

    return root;// return the node pointer of the unchanged node
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

int main(void){
    Node* avlTree = NULL;// pointer to the root of the AVL tree
    long long inputId;
    int firstOutputFlag = 0;
    char line[1024], inputFirstname[24], inputLastName[24], inputBirthDay[16], operation;
    while (fgets(line, sizeof(line), stdin) != NULL)// read the input line by line
    {
        if (sscanf(line, "%c", &operation) < 1){
            continue;
        }// if there was nothing at the input, skip current iteration

        switch (operation)
        {
        case 'i':
            sscanf(line+2, "%lld %23s %23s %15s", &inputId, inputFirstname, inputLastName, inputBirthDay);// parse for insertion
            avlTree = insert(avlTree, inputId, inputFirstname, inputLastName, inputBirthDay);
            break;
        
        case 's':
            long long leftIntervalId, rightIntervalId;
            int number_of_read = sscanf(line+2, "%lld", &leftIntervalId); 
            
            int num_items_read = sscanf(line + number_of_read + 3, "%lld", &rightIntervalId);
            if (num_items_read <= 1) {//
                rightIntervalId = leftIntervalId; // If no right interval ID provided, set it to left interval ID
                searchNode(avlTree, leftIntervalId, rightIntervalId, &firstOutputFlag);
            } else if (rightIntervalId < leftIntervalId) {
                searchNode(avlTree, rightIntervalId, leftIntervalId, &firstOutputFlag);
            } else {
                searchNode(avlTree, leftIntervalId, rightIntervalId, &firstOutputFlag);
            }
            break;
        
        case 'd':
            /* code */
            break;

        default:
            break;
        }
        

    }
    freeTree(avlTree);
    return 0;
}