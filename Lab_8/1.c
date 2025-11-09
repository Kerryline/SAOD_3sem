#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define N 100

typedef struct Node {
    int key;
    int weight;
    struct Node *left, *right;
} Node;

Node* create_node(int key, int weight) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        printf("Ошибка выделения памяти для узла!\n");
        exit(1);
    }
    node->key = key;
    node->weight = weight;
    node->left = node->right = NULL;
    return node;
}

Node* build_tree(int keys[], int weights[], int AR[][N+1], int i, int j) {
    if (i >= j) return NULL;
    int root_index = AR[i][j];
    Node* root = create_node(keys[root_index-1], weights[root_index-1]);
    root->left = build_tree(keys, weights, AR, i, root_index-1);
    root->right = build_tree(keys, weights, AR, root_index, j);
    return root;
}

void inorder_traversal(Node* root, int limit) {
    if (root && limit > 0) {
        inorder_traversal(root->left, limit);
        printf("%d ", root->key);
        inorder_traversal(root->right, limit - 1);
    }
}

int tree_size(Node* root) {
    if (!root) return 0;
    return 1 + tree_size(root->left) + tree_size(root->right);
}

void compute_tree_props(Node* root, int level, int* sum_weighted_height, int* total_weight, int* max_height, int* control_sum) {
    if (!root) return;
    *sum_weighted_height += root->weight * level;
    *total_weight += root->weight;
    *control_sum += root->weight;
    if (level > *max_height) *max_height = level;
    compute_tree_props(root->left, level + 1, sum_weighted_height, total_weight, max_height, control_sum);
    compute_tree_props(root->right, level + 1, sum_weighted_height, total_weight, max_height, control_sum);
}

void free_tree(Node* root) {
    if (root) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void print_matrix_partial(int matrix[][N+1], const char* name, int max_rows, int max_cols) {
    printf("\n%s (первые 10x10 столбцов):\n", name, max_rows, max_cols);
    for (int i = 0; i < max_rows && i <= N; i++) {
        for (int j = 0; j < max_cols && j <= N; j++) {
            printf("%4d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    srand(42);
    
    int keys[N], weights[N];
    int AW[N+1][N+1] = {0}, AP[N+1][N+1] = {0}, AR[N+1][N+1] = {0};

    printf("Генерация %d вершин со случайными весами...\n", N);
    int total_weight = 0;
    for (int i = 0; i < N; i++) {
        keys[i] = i + 1;
        weights[i] = rand() % 100 + 1;
        total_weight += weights[i];
    }
    printf("Общий вес всех вершин: %d\n\n", total_weight);

    for (int i = 0; i <= N; i++) {
        AW[i][i] = 0;
        AP[i][i] = 0;
        AR[i][i] = 0;
    }

    printf("Построение матриц AW, AP, AR...\n");
    
    for (int length = 1; length <= N; length++) {
        for (int i = 0; i <= N - length; i++) {
            int j = i + length;
            
            AW[i][j] = AW[i][j-1] + weights[j-1];
            
            AP[i][j] = INT_MAX;
            int min_k = i + 1;
            int max_k = j;
            
            if (i < j - 1) {
                min_k = AR[i][j-1];
                max_k = AR[i+1][j];
                if (min_k > max_k) min_k = max_k;
            }
            
            for (int k = min_k; k <= max_k; k++) {
                int cost = AP[i][k-1] + AP[k][j];
                if (cost < AP[i][j]) {
                    AP[i][j] = cost;
                    AR[i][j] = k;
                }
            }
            AP[i][j] += AW[i][j];
        }
    }

    print_matrix_partial(AW, "Матрица весов AW", 10, 10);
    print_matrix_partial(AP, "Матрица взвешенных высот AP", 10, 10);
    print_matrix_partial(AR, "Матрица корней поддеревьев AR", 10, 10);

    printf("\nПостроение дерева оптимального поиска...\n");
    Node* root = build_tree(keys, weights, AR, 0, N);

    printf("\nОбход дерева слева направо (первые 20 элементов): ");
    inorder_traversal(root, 20);
    printf("\n");

    printf("\nВычисление характеристик дерева...\n");
    int sum_weighted_height = 0;
    int tree_total_weight = 0;
    int max_height = 0;
    int control_sum = 0;
    
    compute_tree_props(root, 1, &sum_weighted_height, &tree_total_weight, &max_height, &control_sum);
    double avg_height = (double)sum_weighted_height / tree_total_weight;

    double calc_avg_height = (double)AP[0][N] / AW[0][N];
    printf("\nПроверка правильности алгоритма:\n");
    printf("AP[0,n]/AW[0,n] = %.6f\n", calc_avg_height);
    printf("Средневзвешенная высота дерева = %.6f\n", avg_height);
    printf("Расхождение: %.6f\n", calc_avg_height - avg_height);

    printf("\n+----------+--------+--------------+--------+---------------------+\n");
    printf("|   n=%d   | Размер | Контр. Сумма | Высота | Средневзвеш.высота |\n", N);
    printf("+----------+--------+--------------+--------+---------------------+\n");
    printf("|   ДОП    |  %4d  |    %8d   |  %4d  |       %8.4f      |\n", 
           tree_size(root), control_sum, max_height, avg_height);
    printf("+----------+--------+--------------+--------+---------------------+\n");

    return 0;
}