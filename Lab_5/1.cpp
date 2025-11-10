#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Vertex {
    int Data;
    int Bal;
    Vertex* Left;
    Vertex* Right;
};
Vertex* Root_AVL;


int max(int a, int b) {
    return (a > b) ? a : b;
}

Vertex* rotateLL(Vertex* p) {
    Vertex* q = p->Left;
    p->Bal = 0;
    q->Bal = 0;
    p->Left = q->Right;
    q->Right = p;
    return q;
}

Vertex* rotateRR(Vertex* p) {
    Vertex* q = p->Right;
    p->Bal = 0;
    q->Bal = 0;
    p->Right = q->Left;
    q->Left = p;
    return q;
}

Vertex* rotateLR(Vertex* p) {
    Vertex* q = p->Left;
    Vertex* r = q->Right;
    
    if (r->Bal < 0)
        p->Bal = 1;
    else
        p->Bal = 0;
    
    if (r->Bal > 0)
        q->Bal = -1;
    else
        q->Bal = 0;
    
    r->Bal = 0;
    q->Right = r->Left;
    p->Left = r->Right;
    r->Left = q;
    r->Right = p;
    return r;
}

Vertex* rotateRL(Vertex* p) {
    Vertex* q = p->Right;
    Vertex* r = q->Left;
    
    if (r->Bal > 0)
        p->Bal = -1;
    else
        p->Bal = 0;
    
    if (r->Bal < 0)
        q->Bal = 1;
    else
        q->Bal = 0;
    
    r->Bal = 0;
    q->Left = r->Right;
    p->Right = r->Left;
    r->Right = q;
    r->Left = p;
    return r;
}

Vertex* insertAVL(Vertex* p, int D, int* Rost) {
    if (p == NULL) {
        p = (Vertex*)malloc(sizeof(Vertex));
        p->Data = D;
        p->Left = NULL;
        p->Right = NULL;
        p->Bal = 0;
        *Rost = 1;
        return p;
    }
    else if (p->Data > D) {
        p->Left = insertAVL(p->Left, D, Rost);
        
        if (*Rost == 1) {
            if (p->Bal > 0) {
                p->Bal = 0;
                *Rost = 0;
            }
            else if (p->Bal == 0) {
                p->Bal = -1;
                *Rost = 1;
            }
            else if (p->Bal > 0) {
                p->Bal = 0;
                *Rost = 1;
            }
            else { // p->Bal < 0
                if (p->Left->Bal < 0) {
                    p = rotateLL(p);
                    *Rost = 0;
                }
                else {
                    p = rotateLR(p);
                    *Rost = 0;
                }
            }
        }
    }
    else if (p->Data < D) {
        p->Right = insertAVL(p->Right, D, Rost);
        
        if (*Rost == 1) {
            if (p->Bal < 0) {
                p->Bal = 0;
                *Rost = 0;
            }
            else if (p->Bal == 0) {
                p->Bal = 1;
                *Rost = 1;
            }
            else { // p->Bal > 0
                if (p->Right->Bal > 0) {
                    p = rotateRR(p);
                    *Rost = 0;
                }
                else {
                    p = rotateRL(p);
                    *Rost = 0;
                }
            }
        }
    }
    else {
        *Rost = 0;
    }
    
    return p;
}

Vertex* ISDP(int L, int R, int A[]) {
    if (L > R) return NULL;
    
    int m = (L + R) / 2;
    Vertex* p = (Vertex*)malloc(sizeof(Vertex));
    p->Data = A[m];
    p->Left = ISDP(L, m - 1, A);
    p->Right = ISDP(m + 1, R, A);
    return p;
}

void Obhod(Vertex* p) {
    if (p) {
        Obhod(p->Left);
        printf("%d ", p->Data);
        Obhod(p->Right);
    }
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

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    srand(time(NULL));
    const int n = 100;
    int A[n];

    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }

    Vertex* Root_ISDP = ISDP(0, n - 1, A);


   for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&A[i], &A[j]);
    }

    for (int i = 0; i < n; i++) {
        int rost = 0;
        Root_AVL = insertAVL(Root_AVL, A[i], &rost);
    }

    printf("Обход АВЛ-дерева:\n");
    Obhod(Root_AVL);
    printf("\n\n");

    printf("|-------------------------------------------------------|\n");
    printf("| n=100 | Размер | Контр. Сумма | Высота | Средн.высота |\n");
    printf("|-------|--------|--------------|--------|--------------|\n");
    printf("| ИСДП  | %-6d | %-12d | %-6d | %-12.2f |\n", 
           Size(Root_ISDP), Checksum(Root_ISDP), Height(Root_ISDP), (float)SumPathLengths(Root_ISDP, 1) /Size(Root_ISDP));
    printf("| АВЛ   | %-6d | %-12d | %-6d | %-12.2f |\n", 
           Size(Root_AVL), Checksum(Root_AVL), Height(Root_AVL), (float)SumPathLengths(Root_AVL, 1) / Size(Root_AVL));
    printf("|-------------------------------------------------------|\n");

    return 0;
}
