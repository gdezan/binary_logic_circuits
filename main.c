// Gabriel de Andrade Dezan
// 10525706

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -1

typedef struct node {
    char gate;  // Tipo de portão
    int index;  // Numeração do Portão
    int bin;    // Valor no portão (0, 1 ou EMPTY (-1))
    struct node* left;
    struct node* right;
} Node;

typedef Node* Tree;

// Função para a criação da árvore
Tree* CreateTree() {
    Tree* root = (Tree*)malloc(sizeof(Tree));
    if (root != NULL) {
        *root = NULL;
    }
    return root;
}

// Libera um nó específico da árvore
void FreeNode(Node* node) {
    if (node == NULL) return;
    FreeNode(node->left);
    FreeNode(node->right);
    free(node);
    node = NULL;
}

// Libera a árvore
void FreeTree(Tree* root) {
    if (root == NULL) return;
    FreeNode(*root);
    free(root);
}

/*
 * As três funções seguintes retornam a os portões
 * em forma de char* a partir da entrada do tipo:
 *          "A00 E00 E01".
 * Retornando respectivamente "A00", "E00" e "E01".
 */
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

/*
 * A partir de um string do tipo "A02", retorna
 * o índice do portão (2, nesse caso).
 */
int GetGateIndex(char gate[4]) {
    char index_string[3];
    index_string[0] = gate[1];
    index_string[1] = gate[2];
    index_string[2] = '\0';
    return atoi(index_string);
}

// Inicializa um nó a partir de string do tipo "A00"
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

/*
 * Função que insere os portões a partir das entradas do tipo
 *  "X00 Y00 Z00"
 * Ela primeiro procura retira o primeiro portão (X00), o procura
 * na árvore e, caso ache, insere os dois filhos (Y00 e Z00) em seguida
 */
void SearchAndInsert(Tree* root, char value[13]) {
    char* parent = GetParent(value);
    if (root == NULL) return;
    if (*root != NULL) {
        if (parent[0] == (*root)->gate &&  // Confirmando se o portão
            GetGateIndex(parent) == (*root)->index) {  // é o "pai"
            char* left_child = GetLeftChild(value);
            char* right_child = GetRightChild(value);
            (*root)->left = CreateNode(left_child);
            if (right_child != NULL) (*root)->right = CreateNode(right_child);
            free(parent);
            free(left_child);
            free(right_child);
        }
        SearchAndInsert(&((*root)->left), value);  // Continua procurando
        SearchAndInsert(&((*root)->right), value);
    }
}

int InsertGates(Tree* root, char value[13]) {
    if (root == NULL) return 0;
    if (*root == NULL) {
        char* parent = GetParent(value);
        *root = CreateNode(parent);
        free(parent);
    }
    SearchAndInsert(root, value);
    return 1;
}

char* GetGateFromLine(int index, char line[1024]) {
    char* gate = (char*)malloc(4 * sizeof(char));
    for (int i = 0; i < 3; i++) {
        gate[i] = line[(index * 4) + i];
    }
    gate[3] = '\0';
    return gate;
}

void LineInsert(Tree* root, char line[1024], int* count) {
    char* gate_str = GetGateFromLine(*count, line);
    Node* gate = CreateNode(gate_str);
    free(gate_str);
    if (gate == NULL) return;
    (*root) = gate;
    (*count)++;
    if (gate->gate == 'E') return;
    LineInsert(&((*root)->left), line, count);
    if (gate->gate != 'N') LineInsert(&((*root)->right), line, count);
}

/*
 * A partir de uma string do tipo "1 0 1 0", adiciona
 * esses valores em suas respectivas entradas.
 * (Nesse caso, E00 = 1, E01 = 0, E02 = 1, E03 = 0)
 */
void EntryValues(Tree* root, char* values) {
    if (root == NULL) return;
    Node* current = *root;
    if (current != NULL) {
        if (current->gate == 'E') {  // Se o portão for uma entrada
            int entry_value = values[(current->index) * 2] -
                              '0';       // Pega o respectivo valor na string
            current->bin = entry_value;  // Coloca no lugar
        } else {
            current->bin = EMPTY;  // Caso contrário, "limpa" o portão
        }
        EntryValues(&(current->left),
                    values);  // Continua procurando por outros
        EntryValues(&(current->right), values);  // portões
    }
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

void GetGateValues(Tree* root) {
    if (root == NULL) return;
    Node* current = *root;
    if (current != NULL) {
        if (current->left != NULL && current->left->bin != EMPTY &&
            current->bin == EMPTY)
            GateValue(current);
        GetGateValues(&(current->left));
        GetGateValues(&(current->right));
    }
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

int main(int argc, char const* argv[]) {
    Tree* root = CreateTree();

    char line_input[1024];
    char string_of_input[8];
    // FILE* fp = fopen(argv[1], "r");
    fgets(string_of_input, 3, stdin);
    int type_of_input = atoi(string_of_input);
    int num_of_lines;

    if (type_of_input == 0) {
        // if (1 == 0) {
        fgets(string_of_input, 8, stdin);
        num_of_lines = atoi(string_of_input);
        for (int i = 0; i < num_of_lines; i++) {
            // fflush(stdin);
            fgets(line_input, 1024, stdin);
            InsertGates(root, line_input);
        }
    } else {
        fgets(line_input, 1024, stdin);
        int i = 0;
        LineInsert(root, line_input, &i);
    }

    int num_of_entries;
    fgets(string_of_input, 8, stdin);
    num_of_entries = atoi(string_of_input);

    char entries[1024];
    int* answers = (int*)malloc(num_of_entries * sizeof(int));
    // fgets(entries, 1024, stdin);

    for (int i = 1; i <= num_of_entries; i++) {
        // fflush(stdin);
        fgets(entries, 1024, stdin);
        EntryValues(root, entries);
        while ((*root)->bin == EMPTY) {
            GetGateValues(root);
        }
        answers[i] = (*root)->bin;
    }

    for (int i = 1; i <= num_of_entries; i++) {
        printf("%d\n", answers[i]);
    }
    FreeTree(root);
    // fclose(fp);

    return 0;
}