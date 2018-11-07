#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node* left;
    struct node* right;
} Node;

typedef Node* Tree;

Tree* create_tree() {
    Tree* root = (Tree*)malloc(sizeof(Tree));
    if (root != NULL) {
        *root = NULL;
    }
    return root;
}

void free_node(Node* node) {
    if (node == NULL) return;
    free_node(node->left);
    free_node(node->right);
    free(node);
    node = NULL;
}

void free_tree(Tree* root) {
    if (root == NULL) return;
    free_node(*root);
    free(root);
}

int insert_tree(Tree* root, int value) {
    if (root == NULL) return 0;
    Node* new;
    new = (Node*)malloc(sizeof(Node));
    if (new == NULL) return 0;
    new->data = value;
    new->right = NULL;
    new->left = NULL;

    if (*root == NULL)
        *root = new;
    else {
        Node* current = *root;
        Node* prev = NULL;
        while (current != NULL) {
            prev = current;
            if (value == current->data) {
                free(new);
                return 0;  // elemento já existe
            }

            if (value > current->data)
                current = current->right;
            else
                current = current->left;
        }
        if (value > prev->data)
            prev->right = new;
        else
            prev->left = new;
    }
    return 1;
}

Node* remove_current(Node* current) {
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

int remove_tree(Tree* root, int value) {
    if (root == NULL) return 0;
    Node* prev = NULL;
    Node* current = *root;
    while (current != NULL) {
        if (value == current->data) {
            if (current == *root)
                *root = remove_current(current);
            else {
                if (prev->right == current)
                    prev->right = remove_current(current);
                else
                    prev->left = remove_current(current);
            }
            return 1;
        }
        prev = current;
        if (value > current->data)
            current = current->right;
        else
            current = current->left;
    }
    return 0;
}

int is_tree_empty(Tree* root) {
    if (root == NULL) return 1;
    if (*root == NULL) return 1;
    return 0;
}

int total_nodes_tree(Tree* root) {
    if (root == NULL) return 0;
    if (*root == NULL) return 0;
    int alt_left = total_nodes_tree(&((*root)->left));
    int alt_right = total_nodes_tree(&((*root)->right));
    return (alt_left + alt_right + 1);
}

int height_tree(Tree* root) {
    if (root == NULL) return 0;
    if (*root == NULL) return 0;
    int height_left = height_tree(&((*root)->left));
    int height_right = height_tree(&((*root)->right));
    if (height_left > height_right)
        return (height_left + 1);
    else
        return (height_right + 1);
}

int consult_tree(Tree* root, int value) {
    if (root == NULL) return 0;
    Node* current = *root;
    while (current != NULL) {
        if (value == current->data) {
            return 1;
        }
        if (value > current->data)
            current = current->right;
        else
            current = current->left;
    }
    return 0;
}

void preOrdem_tree(Tree* root) {
    if (root == NULL) return;
    if (*root != NULL) {
        printf("%d\n", (*root)->data);
        preOrdem_tree(&((*root)->left));
        preOrdem_tree(&((*root)->right));
    }
}

void emOrdem_tree(Tree* root) {
    if (root == NULL) return;
    if (*root != NULL) {
        emOrdem_tree(&((*root)->left));
        printf("%d\n", (*root)->data);
        emOrdem_tree(&((*root)->right));
    }
}

void posOrdem_tree(Tree* root) {
    if (root == NULL) return;
    if (*root != NULL) {
        posOrdem_tree(&((*root)->left));
        posOrdem_tree(&((*root)->right));
        printf("%d\n", (*root)->data);
    }
}

// Funcoes Adicionais de Arvore

void Procura_preOrdem_tree(Tree* root, int value, int* achou) {
    if (root == NULL) return;
    if (*achou) return;
    if (*root != NULL) {
        if (value == (*root)->data) {
            printf("Achou: %d! \n", (*root)->data);
            *achou = 1;
        }
        Procura_preOrdem_tree(&((*root)->left), value, achou);
        Procura_preOrdem_tree(&((*root)->right), value, achou);
    }
}

void Exibe_emOrdem_tree(Tree* root) {
    if (root == NULL) return;

    if (*root != NULL) {
        printf("Atual: %d - Vai para Esquerda \n", (*root)->data);
        Exibe_emOrdem_tree(&((*root)->left));
        printf("Dado : %d \n", (*root)->data);
        printf("Atual: %d - Vai para Direita \n", (*root)->data);
        Exibe_emOrdem_tree(&((*root)->right));
        printf("Feito(%d) \n", (*root)->data);
    } else
        printf("NULL\n");
}

int main() {
    int N = 8, dados[8] = {50, 100, 30, 20, 40, 45, 35, 37};

    Tree* raiz = create_tree();

    int i;
    for (i = 0; i < N; i++) insert_tree(raiz, dados[i]);

    printf("Dados:\n");
    emOrdem_tree(raiz);

    printf("\n");
    printf("Percurso:\n");
    Exibe_emOrdem_tree(raiz);

    // system("Pause");
    printf("\n");

    i = 0;
    Procura_preOrdem_tree(raiz, 45, &i);
    if (i == 0) printf("Nao Achou! \n");

    free_tree(raiz);

    return 0;
}