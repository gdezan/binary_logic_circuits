// Gabriel de Andrade Dezan
// 10525706

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -1

typedef struct node {
    char gate;
    int index;
    int bin;
    struct node* left;
    struct node* right;
} Node;

typedef Node* Tree;

Tree* CreateTree() {
    Tree* root = (Tree*)malloc(sizeof(Tree));
    if (root != NULL) {
        *root = NULL;
    }
    return root;
}

void FreeNode(Node* node) {
    if (node == NULL) return;
    FreeNode(node->left);
    FreeNode(node->right);
    free(node);
    node = NULL;
}

void FreeTree(Tree* root) {
    if (root == NULL) return;
    FreeNode(*root);
    free(root);
}

char* GetParent(char value[13]) {
    char* parent_string = malloc(4 * sizeof(char));
    for (int i = 0; i < 3; i++) parent_string[i] = value[i];
    parent_string[3] = '\0';
    return parent_string;
}

char* GetLeftChild(char value[13]) {
    char* child_string = malloc(4 * sizeof(char));
    for (int i = 4; i < 7; i++) child_string[i - 4] = value[i];
    child_string[4] = '\0';
    return child_string;
}

char* GetRightChild(char value[13]) {
    if (value[8] == '\0' || value[9] == '\0') return NULL;
    char* child_string = malloc(4 * sizeof(char));
    for (int i = 8; i < 11; i++) child_string[i - 8] = value[i];
    child_string[4] = '\0';
    return child_string;
}

int GetGateIndex(char gate[4]) {
    char index_string[3];
    index_string[0] = gate[1];
    index_string[1] = gate[2];
    index_string[2] = '\0';
    return atoi(index_string);
}

Node* CreateNode(char value[4]) {
    Node* new;
    new = (Node*)malloc(sizeof(Node));
    if (new == NULL) return 0;
    new->gate = value[0];
    new->index = GetGateIndex(value);
    new->right = NULL;
    new->bin = EMPTY;
    new->left = NULL;
    return new;
}

void SearchAndInsert(Tree* root, char value[13]) {
    char* parent = GetParent(value);
    if (root == NULL) return;
    if (*root != NULL) {
        if (parent[0] == (*root)->gate &&
            GetGateIndex(parent) == (*root)->index) {
            char* left_child = GetLeftChild(value);
            char* right_child = GetRightChild(value);
            (*root)->left = CreateNode(left_child);
            if (right_child != NULL) (*root)->right = CreateNode(right_child);
        }
        SearchAndInsert(&((*root)->left), value);
        SearchAndInsert(&((*root)->right), value);
    }
}

int InsertGates(Tree* root, char value[13]) {
    char* parent = GetParent(value);
    if (root == NULL) return 0;

    if (*root == NULL) {
        *root = CreateNode(parent);
    }
    SearchAndInsert(root, value);
    return 1;
}

void GateValue(Node* gate) {
    Node* current = gate;
    if (current == NULL) return;
    int lValue, rValue;
    if (current->left != NULL) lValue = current->left->bin;
    if (current->right != NULL) rValue = current->right->bin;
    if (lValue != EMPTY && rValue != EMPTY) {
        switch (gate->gate) {
            case 'A':
                current->bin = (lValue & rValue);
                break;
            case 'O':
                current->bin = (lValue | rValue);
                break;
            case 'D':
                current->bin = !(lValue & rValue);
                break;
            case 'R':
                current->bin = !(lValue | rValue);
                break;
            case 'X':
                current->bin = (lValue ^ rValue);
                break;
            case 'N':
                current->bin = !(lValue);
                break;
        }
    }
}

void EntryValues(Tree* root, char* values) {
    if (root == NULL) return;
    Node* current = *root;
    if (current != NULL) {
        if (current->gate == 'E') {
            int entry_value = values[(current->index) * 2] - '0';
            current->bin = entry_value;
        } else {
            current->bin = EMPTY;
        }
        EntryValues(&(current->left), values);
        EntryValues(&(current->right), values);
    }
}

void GetGateValues(Tree* root) {
    if (root == NULL) return;
    Node* current = *root;
    if (current != NULL) {
        GateValue(current);
        GetGateValues(&(current->left));
        GetGateValues(&(current->right));
    }
}

Node* RemoveCurrent(Node* current) {
    Node *node1, *node2;
    if (current->left == NULL) {
        node2 = current->right;
        free(current);
        return node2;
    }
    node1 = current;
    node2 = current->left;
    while (node2->right != NULL) {
        node1 = node2;
        node2 = node2->right;
    }
    // node2 é o nó anterior a r na ordem e-r-d
    // node1 é o pai de node2
    if (node1 != current) {
        node1->right = node2->left;
        node2->left = current->left;
    }
    node2->right = current->right;
    free(current);
    return node2;
}

int RemoveTree(Tree* root, char value[4]) {
    int new_index = GetGateIndex(value);
    if (root == NULL) return 0;
    Node* prev = NULL;
    Node* current = *root;
    while (current != NULL) {
        if (new_index == current->index) {
            if (current == *root)
                *root = RemoveCurrent(current);
            else {
                if (prev->right == current)
                    prev->right = RemoveCurrent(current);
                else
                    prev->left = RemoveCurrent(current);
            }
            return 1;
        }
        prev = current;
        if (new_index > current->index)
            current = current->right;
        else
            current = current->left;
    }
    return 0;
}

int IsTreeEmpty(Tree* root) {
    if (root == NULL) return 1;
    if (*root == NULL) return 1;
    return 0;
}

int TotalNodesTree(Tree* root) {
    if (root == NULL) return 0;
    if (*root == NULL) return 0;
    int alt_left = TotalNodesTree(&((*root)->left));
    int alt_right = TotalNodesTree(&((*root)->right));
    return (alt_left + alt_right + 1);
}

void InOrderTree(Tree* root) {
    if (root == NULL) return;
    if (*root != NULL) {
        InOrderTree(&((*root)->left));
        printf("%c", (*root)->gate);
        printf("%02d -> %d\n", (*root)->index, (*root)->bin);
        InOrderTree(&((*root)->right));
    }
}

int main() {
    Tree* root = CreateTree();
    char line_input[256];
    int type_of_input;
    scanf("%d", &type_of_input);
    if (type_of_input == 0) {
        int num_of_lines;
        scanf("%d", &num_of_lines);
        for (int i = 0; i < num_of_lines; i++) {
            fflush(stdin);
            fgets(line_input, 256, stdin);
            InsertGates(root, line_input);
        }
    } else {
        return 0;
    }
    int num_of_entries;
    scanf("%d", &num_of_entries);
    char entries[256];
    int* answers = (int*)malloc(num_of_entries * sizeof(int));
    for (int i = 0; i < num_of_entries; i++) {
        fflush(stdin);
        fgets(entries, 256, stdin);
        EntryValues(root, entries);
        while ((*root)->bin == EMPTY) GetGateValues(root);
        answers[i] = (*root)->bin;
    }
    for (int i = 0; i < num_of_entries; i++) printf("%d\n", answers[i]);
    printf("\n");

    // FreeTree(root);

    return 0;
}