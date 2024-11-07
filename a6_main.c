#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int label;
    int width;
    int height;
    char cutline;
    int x;
    int y;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct stackArray {
    Node** node;
    int top;
} stackArray;

Node* pop (stackArray* arr){
    return arr->node[arr->top--];
}

void push (stackArray* arr, Node *node){
    arr->node[++(arr->top)]=node;
}

void freeStack(stackArray* arr){
    if (arr == NULL) {
        return;
    }

    for (int i = 0; i <= arr->top; i++) {
        if (arr->node[i] != NULL) {
            free(arr->node[i]);
        }
    }

    free(arr->node);
    free(arr);
}

Node* createLeafNode(int label, int width, int height) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->label = label;
    node->width = width;
    node->height = height;
    node->cutline = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* createInternalNode(char cutline, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->label = -1;
    node->width = 0;
    node->height = 0;
    node->cutline = cutline;
    node->left = left;
    node->right = right;
    return node;
}

void calcPosition(Node* node) {
    if (node == NULL) {
        return;
    }

    if (node->cutline == '\0') {
        return;
    }

    calcPosition(node->left);
    calcPosition(node->right);

    if (node->cutline == 'H') {
        node->width = (node->left->width > node->right->width) ? node->left->width : node->right->width;
        node->height = node->left->height + node->right->height;
    } else if (node->cutline == 'V') {
        node->width = node->left->width + node->right->width;
        node->height = (node->left->height > node->right->height) ? node->left->height : node->right->height;
    }
}

void preOrderTraversal(Node* root, FILE* outFile) {
    if (root == NULL) {
        return;
    }
    if (root->label != -1)
        fprintf(outFile, "%d(%d,%d)\n", root->label, root->width, root->height);
    else
        fprintf(outFile, "%c\n", root->cutline);
    preOrderTraversal(root->left, outFile);
    preOrderTraversal(root->right, outFile);
}

void postOrderTraversal(Node* root, FILE* outputFile) {
    if (root == NULL) {
        return;
    }
    postOrderTraversal(root->left, outputFile);
    postOrderTraversal(root->right, outputFile);
    if (root->label != -1) {
        fprintf(outputFile, "%d(%d,%d)\n", root->label, root->width, root->height);
    } else {
        fprintf(outputFile, "%c(%d,%d)\n", root->cutline, root->width, root->height);
    }
}

void postOrderDimensions(Node* root, FILE* outputFile, int x, int y) {
    if (root == NULL) {
        return;
    }
    if (root->label == -1){
        if (root->cutline == 'H'){
            postOrderDimensions(root->left, outputFile, x, (y + (root->right->height)));
            postOrderDimensions(root->right, outputFile, x, y);
        } else {
            postOrderDimensions(root->left, outputFile, x, y);
            postOrderDimensions(root->right, outputFile, x + root->left->width, y);
        }
    } else {
        fprintf(outputFile, "%d((%d,%d)(%d,%d))\n", root->label, root->width, root->height, x, y);
    }
}

void freeTree(Node* node) {
    if (node == NULL) {
        return;
    }

    freeTree(node->left);
    freeTree(node->right);

    free(node);
}

Node* buildTree(FILE* inFile) {
    int size=0;
    char line[100];
    while(fgets(line, sizeof(line), inFile)!=NULL){
        size++;
    }
    rewind(inFile);
    stackArray* stack = malloc(sizeof(stackArray));
    stack->node = malloc(size * sizeof(Node*));
    stack->top = -1;
    // int top = -1;

    int label, width, height;
    while (fgets(line, sizeof(line), inFile)) {
        if (sscanf(line, "%d(%d,%d)", &label, &width, &height)==3) {
            Node* leaf = createLeafNode(label, width, height);
            push(stack, leaf);
        } else {
            // char cutline = line[0];
            Node* right = pop(stack);
            Node* left = pop(stack);
            Node* internal = createInternalNode(line[0], left, right);
            push(stack, internal);
        }
    }
    Node* root = pop(stack);
    freeStack(stack);
    return root;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {////////////////////////////////////////   !!!!!!!!!!!!!!!!!!!!!!!!!!!!   //////////////////////////////////////////////
        fprintf(stderr, "Usage: %s in_file out_file1 out_file2 out_file3\n", argv[0]);
        return EXIT_FAILURE;
    }

    ///////////////////////////////////////////////////////
    FILE* inFile = fopen(argv[1], "r");
    if (inFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    Node* root = buildTree(inFile);
    fclose(inFile);
    //////////////////////////////////////////////////////
    FILE* outFile1 = fopen(argv[2], "w");
    if (outFile1 == NULL) {
        perror("Error opening output file 1");
        return EXIT_FAILURE;
    }

    FILE* outFile2 = fopen(argv[3], "w");
    if (outFile2 == NULL) {
        perror("Error opening output file 2");
        fclose(outFile1);
        return EXIT_FAILURE;
    }

    FILE* outFile3 = fopen(argv[4], "w");
    if (outFile3 == NULL) {
        perror("Error opening output file 3");
        fclose(outFile1);
        fclose(outFile2);
        return EXIT_FAILURE;
    }
    //////////////////////////////////////////////////////

    preOrderTraversal(root, outFile1);
    fclose(outFile1);

    calcPosition(root);
    postOrderTraversal(root, outFile2);
    fclose(outFile2);


    postOrderDimensions(root, outFile3, 0, 0);
    fclose(outFile3);
    ///////////////////////////////////////////////////////////////////////////////////
    freeTree(root);

    return EXIT_SUCCESS;
}
