#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Vertex { //структура вершины
    int Data;
    Vertex* Left;
    Vertex* Right;
}; 
Vertex* Root;
Vertex* Root_r;
Vertex* Root_d;

void add_recursive_sdp(int D, Vertex** p) {
    if (*p == NULL) {
        *p = (Vertex*)malloc(sizeof(Vertex));
        (*p)->Data = D;
        (*p)->Left = NULL;
        (*p)->Right = NULL;
    } 
    
    else if (D < (*p)->Data) {
        add_recursive_sdp(D, &((*p)->Left));
    } 
    
    else if (D > (*p)->Data) {
        add_recursive_sdp(D, &((*p)->Right));
    }
}

void add_double_indirect(int D, Vertex** root) {
    Vertex** current = root;
    
    while (*current != NULL) {
        if (D < (*current)->Data) {
            current = &(*current)->Left;
        } else if (D > (*current)->Data) {
            current = &(*current)->Right;
        } else {
            return;
        }
    }
    
    *current = (Vertex*)malloc(sizeof(Vertex));
    (*current)->Data = D;
    (*current)->Left = NULL;
    (*current)->Right = NULL;
}

Vertex* ISDP(int L, int R, int A[]){ //создание исдп
    if (L>R){
        return NULL;
    }
    int m = (L + R) / 2;

    Vertex* p = (Vertex*)malloc(sizeof(Vertex));
    p->Data = A[m];
    p->Left = ISDP(L, m-1, A);
    p->Right = ISDP(m+1, R, A);

    return p;
}

void Obhod3 (Vertex *p){ //слева направо
    if (p!=NULL){
        Obhod3 (p->Left);
        printf ("%d ", p->Data);
        Obhod3 (p->Right);
    }
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int Size(Vertex *p){
    if (p==NULL){
        return 0;
    }
    else{ 
        return 1 + Size(p->Left) + Size(p->Right);
    }
}

int Checksum(Vertex *p){
    if (p==NULL){
        return 0;
    }
    else{
        return p->Data + Checksum(p->Left) + Checksum(p->Right);
    }
}

int Height(Vertex *p){
    if (p==NULL){
        return 0;
    }
    else{
        return 1 + max(Height(p->Left), Height(p->Right));
    }
}

int SumPathLengths(Vertex *p, int level) {
    if (p == NULL) {
        return 0;
    } else {
        return level + SumPathLengths(p->Left, level + 1) + SumPathLengths(p->Right, level + 1);
    }
}

// Функция для обмена элементов
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main(){
    srand(time(NULL)); 
    
    int n = 100;
    int A[n];

    for (int i = 0; i < n; i++){
        A[i] = i +1;
    }

    Root = ISDP(0, n-1, A);

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&A[i], &A[j]);
    }

    for (int i = 0; i < n; i++) {
        add_recursive_sdp(A[i], &Root_r);
        add_double_indirect(A[i], &Root_d);
    }

    printf("Обход рекурсивного дерева: ");
    Obhod3(Root_r);
    printf("\n\n");
    
    printf("Обход дерева с двойной косвенностью: ");
    Obhod3(Root_d);
    printf("\n\n");
    
    printf("Обход ИСДП: ");
    Obhod3(Root);
    printf("\n\n");

    printf("|---------------------------------------------------------------|\n");     
    printf("| n=100    | Размер    | Контр. сумма | Высота   | Средн.высота |\n");
    printf("|----------|-----------|--------------|----------|--------------|\n");
    printf("| ИСДП     | %-9d | %-12d | %-8d | %-12.2f |\n", Size(Root), Checksum(Root), Height(Root), (float)SumPathLengths(Root, 1) / Size(Root));
    printf("| СДП1     | %-9d | %-12d | %-8d | %-12.2f |\n", Size(Root_r), Checksum(Root_r), Height(Root_r), (float)SumPathLengths(Root_r, 1) /Size(Root_r));
    printf("| СДП2     | %-9d | %-12d | %-8d | %-12.2f |\n", Size(Root_d), Checksum(Root_d), Height(Root_d), (float)SumPathLengths(Root_d, 1) /Size(Root_d));
    printf("|---------------------------------------------------------------|");       
    
    return 0;
}