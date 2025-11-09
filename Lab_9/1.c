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

int AW[N+1][N+1];
int AP[N+1][N+1];
int AR[N+1][N+1];

Node* create_node(int key, int weight) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        printf("Ошибка выделения памяти!\n");
        exit(1);
    }
    node->key = key;
    node->weight = weight;
    node->left = node->right = NULL;
    return node;
}

void free_tree(Node* root) {
    if (root) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void inorder_traversal(Node* root) {
    if (root) {
        inorder_traversal(root->left);
        printf("%d ", root->key);
        inorder_traversal(root->right);
    }
}

void compute_tree_props(Node* root, int level, int* sum_weighted_height, 
                       int* total_weight, int* max_height, int* control_sum) {
    if (!root) return;
    *sum_weighted_height += root->weight * level;
    *total_weight += root->weight;
    *control_sum += root->key;
    if (level > *max_height) *max_height = level;
    compute_tree_props(root->left, level + 1, sum_weighted_height, total_weight, max_height, control_sum);
    compute_tree_props(root->right, level + 1, sum_weighted_height, total_weight, max_height, control_sum);
}

void get_tree_stats(Node* root, int* height, double* avg_height, int* control_sum, int* total_weight) {
    int sum_weighted_height = 0;
    int total_w = 0;
    int max_height = 0;
    int cs = 0;
    
    compute_tree_props(root, 1, &sum_weighted_height, &total_w, &max_height, &cs);
    
    *height = max_height;
    *avg_height = total_w > 0 ? (double)sum_weighted_height / total_w : 0.0;
    *control_sum = cs;
    *total_weight = total_w;
}

Node* build_optimal_subtree(int keys[], int weights[], int i, int j) {
    if (i >= j) return NULL;
    int root_index = AR[i][j];
    Node* root = create_node(keys[root_index-1], weights[root_index-1]);
    root->left = build_optimal_subtree(keys, weights, i, root_index-1);
    root->right = build_optimal_subtree(keys, weights, root_index, j);
    return root;
}

void print_matrix(const char* name, int size) {
    printf("\n%s:\n", name);
    printf("    ");
    for (int j = 0; j <= size && j <= 10; j++) {
        printf("%4d ", j);
    }
    printf("\n");
    
    for (int i = 0; i <= size && i <= 10; i++) {
        printf("%2d: ", i);
        for (int j = 0; j <= size && j <= 10; j++) {
            if (strcmp(name, "Матрица взвешенных высот AP") == 0 && AP[i][j] == INT_MAX) {
                printf("  INF");
            } else if (strcmp(name, "Матрица весов AW") == 0) {
                printf("%4d ", AW[i][j]);
            } else if (strcmp(name, "Матрица взвешенных высот AP") == 0) {
                printf("%4d ", AP[i][j]);
            } else if (strcmp(name, "Матрица корней поддеревьев AR") == 0) {
                printf("%4d ", AR[i][j]);
            }
        }
        printf("\n");
    }
    printf("(показаны первые 10x10 элементов)\n");
}

Node* build_optimal_tree(int keys[], int weights[], int n, int print_matrices) {
    // Инициализация матриц
    for (int i = 0; i <= n; i++) {
        AW[i][i] = 0;
        AP[i][i] = 0;
        AR[i][i] = 0;
    }
    
    for (int length = 1; length <= n; length++) {
        for (int i = 0; i <= n - length; i++) {
            int j = i + length;
            AW[i][j] = AW[i][j-1] + weights[j-1];
            AP[i][j] = INT_MAX;
            
            int left = (i < j-1) ? AR[i][j-1] : i+1;
            int right = (i < j-1) ? AR[i+1][j] : j;
            
            for (int k = left; k <= right; k++) {
                int cost = AP[i][k-1] + AP[k][j];
                if (cost < AP[i][j]) {
                    AP[i][j] = cost;
                    AR[i][j] = k;
                }
            }
            AP[i][j] += AW[i][j];
        }
    }
    
    if (print_matrices) {
        print_matrix("Матрица весов AW", n);
        print_matrix("Матрица взвешенных высот AP", n);
        print_matrix("Матрица корней поддеревьев AR", n);
        
        // Проверка: AP[0,n]/AW[0,n] должно равняться средневзвешенной высоте
        printf("\nПроверка: AP[0,%d] = %d, AW[0,%d] = %d\n", n, AP[0][n], n, AW[0][n]);
        printf("AP[0,%d]/AW[0,%d] = %.4f\n", n, n, (double)AP[0][n] / AW[0][n]);
    }
    
    return build_optimal_subtree(keys, weights, 0, n);
}

Node* build_approx_A1(int keys[], int weights[], int start, int end) {
    if (start > end) return NULL;
    
    int max_weight = -1;
    int max_index = start;
    for (int i = start; i <= end; i++) {
        if (weights[i] > max_weight) {
            max_weight = weights[i];
            max_index = i;
        }
    }
    
    Node* root = create_node(keys[max_index], weights[max_index]);
    root->left = build_approx_A1(keys, weights, start, max_index - 1);
    root->right = build_approx_A1(keys, weights, max_index + 1, end);
    
    return root;
}

Node* build_approx_A2(int keys[], int weights[], int start, int end) {
    if (start > end) return NULL;
    
    int total_weight = 0;
    for (int i = start; i <= end; i++) {
        total_weight += weights[i];
    }
    
    int accumulated = 0;
    int split_index = start;
    for (int i = start; i <= end; i++) {
        accumulated += weights[i];
        if (accumulated * 2 >= total_weight) {
            split_index = i;
            break;
        }
    }
    
    Node* root = create_node(keys[split_index], weights[split_index]);
    root->left = build_approx_A2(keys, weights, start, split_index - 1);
    root->right = build_approx_A2(keys, weights, split_index + 1, end);
    
    return root;
}

int main() {
    srand(time(NULL));
    
    int keys[N], weights[N];
    
    printf("Генерация %d вершин со случайными весами...\n", N);
    int total_weights = 0;
    for (int i = 0; i < N; i++) {
        keys[i] = i + 1;
        weights[i] = rand() % 100 + 1;
        total_weights += weights[i];
    }
    printf("Общий вес всех вершин: %d\n\n", total_weights);
    
    printf("Построение дерева оптимального поиска (ДОП)...\n");
    Node* optimal_tree = build_optimal_tree(keys, weights, N, 1); // 1 - выводить матрицы
    
    printf("\nПостроение почти оптимального дерева (А1)...\n");
    Node* approx_A1_tree = build_approx_A1(keys, weights, 0, N-1);
    
    printf("Построение почти оптимального дерева (А2)...\n");
    Node* approx_A2_tree = build_approx_A2(keys, weights, 0, N-1);
    
    int height_opt, height_A1, height_A2;
    double avg_height_opt, avg_height_A1, avg_height_A2;
    int control_sum_opt, control_sum_A1, control_sum_A2;
    int total_weight_opt, total_weight_A1, total_weight_A2;
    
    get_tree_stats(optimal_tree, &height_opt, &avg_height_opt, &control_sum_opt, &total_weight_opt);
    get_tree_stats(approx_A1_tree, &height_A1, &avg_height_A1, &control_sum_A1, &total_weight_A1);
    get_tree_stats(approx_A2_tree, &height_A2, &avg_height_A2, &control_sum_A2, &total_weight_A2);
    
    printf("\nОбход дерева ДОП (слева направо, первые 30 элементов):\n");
    inorder_traversal(optimal_tree);
    printf("\n\n");
    
    printf("Обход дерева А1 (слева направо, первые 30 элементов):\n");
    inorder_traversal(approx_A1_tree);
    printf("\n\n");
    
    printf("Обход дерева А2 (слева направо, первые 30 элементов):\n");
    inorder_traversal(approx_A2_tree);
    printf("\n\n");
    
    printf("+----------+--------+--------------+--------+---------------------+\n");
    printf("|   n=%d   | Размер | Контр. Сумма | Высота | Средневзвеш.высота |\n", N);
    printf("+----------+--------+--------------+--------+---------------------+\n");
    printf("|   ДОП    |  %4d  |    %8d   |  %4d  |       %8.4f      |\n", 
           N, control_sum_opt, height_opt, avg_height_opt);
    printf("|   А1     |  %4d  |    %8d   |  %4d  |       %8.4f      |\n", 
           N, control_sum_A1, height_A1, avg_height_A1);
    printf("|   А2     |  %4d  |    %8d   |  %4d  |       %8.4f      |\n", 
           N, control_sum_A2, height_A2, avg_height_A2);
    printf("+----------+--------+--------------+--------+---------------------+\n");
    
    printf("\nПроверка контрольных сумм:\n");
    printf("ДОП: %d, А1: %d, А2: %d\n", control_sum_opt, control_sum_A1, control_sum_A2);
    printf("Проверка суммарных весов:\n");
    printf("ДОП: %d, А1: %d, А2: %d\n", total_weight_opt, total_weight_A1, total_weight_A2);
    
    printf("\nПроверка правильности точного алгоритма:\n");
    printf("AP[0,%d]/AW[0,%d] = %.4f\n", N, N, (double)AP[0][N] / AW[0][N]);
    printf("Средневзвешенная высота ДОП = %.4f\n", avg_height_opt);
    printf("Расхождение: %.6f\n", (double)AP[0][N] / AW[0][N] - avg_height_opt);
    
    printf("\nСравнение эффективности:\n");
    printf("А1 / ДОП по средневзвешенной высоте: %.2f%%\n", (avg_height_A1 / avg_height_opt - 1) * 100);
    printf("А2 / ДОП по средневзвешенной высоте: %.2f%%\n", (avg_height_A2 / avg_height_opt - 1) * 100);
    
    free_tree(optimal_tree);
    free_tree(approx_A1_tree);
    free_tree(approx_A2_tree);
    
    
    return 0;
}