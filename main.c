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

// Função que chama SearchAndInsert e preenche os portões
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

/*
 * Função que retorna um portão de uma entrada do tipo '1', dado o número do portão
 * Ex.: GetGateFromLine(2, "A00 N00 R00 E01") retorna "R00"
 *                          [0] [1] [2] [3]
 */
char* GetGateFromLine(int index, char line[1024]) {
    char* gate = (char*)malloc(4 * sizeof(char));
    for (int i = 0; i < 3; i++) {
        gate[i] = line[(index * 4) + i];
    }
    gate[3] = '\0';
    return gate;
}

// Insere a entrada do tipo '1' recursivamente
void LineInsert(Tree* root, char line[1024], int* count) {
    char* gate_str = GetGateFromLine(*count, line);
    Node* gate = CreateNode(gate_str);
    free(gate_str);
    if (gate == NULL) return;
    (*root) = gate;
    (*count)++; // Vai aumentando para poder pegar o portão seguinte da string
    if (gate->gate == 'E') return; // Termina de inserir num ramo, quando o portão é uma entrada
    LineInsert(&((*root)->left), line, count);
    if (gate->gate != 'N') LineInsert(&((*root)->right), line, count); // Apesar se o portão não for do tipo NOT
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

/*
 * A partir do nó, esse método pega o tipo de portão e, a partir
 * dos seus filhos, preenche o estado lógico do portão
 */
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

/*
 * Vai iterando pela árvore, preenchendo todos os estados
 * lógicos dos portões, com o auxílio do método GateValue
 */
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

int main(int argc, char const* argv[]) {
    Tree* root = CreateTree();

    char line_input[1024]; // String que vai receber a maioria das entradas
    char string_of_input[8]; 

    fgets(string_of_input, 3, stdin);               // Recebe o tipo de entrada
    int type_of_input = atoi(string_of_input);      // e converte para int
    
    int num_of_lines;

    if (type_of_input == 0) {
        fgets(string_of_input, 8, stdin);           // Recebe a quantidade de linhas de entrada
        num_of_lines = atoi(string_of_input);
        for (int i = 0; i < num_of_lines; i++) {
            fgets(line_input, 1024, stdin);         // A partir da quantidade de linhas de entrada,
            InsertGates(root, line_input);          // vai montando a árvore
        }
    } else if (type_of_input == 1) {
        fgets(line_input, 1024, stdin);
        int i = 0;                                  // Monta a árvore a partida da entrada do
        LineInsert(root, line_input, &i);           // tipo 1, recursivamente
    } else {
        return 0;
    }

    int num_of_entries;
    fgets(string_of_input, 8, stdin);               // Recebe a quantidade de 'testes' serão feitos
    num_of_entries = atoi(string_of_input);

    char entries[1024];

    // As respostas serão postas em um vetor de inteiros, para serem exibidas após
    // todos os testes serem rodados
    int* answers = (int*)malloc(num_of_entries * sizeof(int));

    for (int i = 1; i <= num_of_entries; i++) {
        fgets(entries, 1024, stdin);

        // Primeiro preenche o valor dos portões de entrada e "esvazia" os outros portões
        // que foram preenchidos em testes anteriores
        EntryValues(root, entries);

        // Em seguida, vai preenchendo o resto dos estados lógicos, até que o nó cabeça da
        // árvore não esteja mais vazio
        while ((*root)->bin == EMPTY) {
            GetGateValues(root);
        }

        // Pega a resposta e põe no vetor
        answers[i] = (*root)->bin;
    }

    for (int i = 1; i <= num_of_entries; i++) {
        printf("%d\n", answers[i]);
    }
    FreeTree(root);

    return 0;
}