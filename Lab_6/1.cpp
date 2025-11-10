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

Vertex* LL1(Vertex* p, int* decreased) {
    Vertex* q = p->Left;
    if (q->Bal == 0) {
        q->Bal = 1;
        p->Bal = -1;
        *decreased = 0;
    } else {
        q->Bal = 0;
        p->Bal = 0;
    }
    p->Left = q->Right;
    q->Right = p;
    return q;
}

Vertex* RR1(Vertex* p, int* decreased) {
    Vertex* q = p->Right;
    if (q->Bal == 0) {
        q->Bal = -1;
        p->Bal = 1;
        *decreased = 0;
    } else {
        q->Bal = 0;
        p->Bal = 0;
    }
    p->Right = q->Left;
    q->Left = p;
    return q;
}

Vertex* BL(Vertex* p, int* decreased) {
    if (p->Bal == -1) {
        p->Bal = 0;
    } else if (p->Bal == 0) {
        p->Bal = 1;
        *decreased = 0;
    } else {
        if (p->Right != NULL) {
            if (p->Right->Bal >= 0) {
                p = RR1(p, decreased);
                printf("\nRR1 поворот!\n");
            } else {
                p = rotateRL(p);
                printf("\nRL поворот!\n");
            }
        }
    }
    return p;
}

Vertex* BR(Vertex* p, int* decreased) {
    if (p->Bal == 1) {
        p->Bal = 0;
    } else if (p->Bal == 0) {
        p->Bal = -1;
        *decreased = 0;
    } else {
        if (p->Left != NULL) {
            if (p->Left->Bal <= 0) {
                p = LL1(p, decreased);
                printf("\nLL1 поворот!\n");
            } else {
                p = rotateLR(p);
                printf("\nLR поворот!\n");
            }
        }
    }
    return p;
}

void del(Vertex** r, Vertex* q, int* decreased) {
    if ((*r)->Right != NULL) {
        del(&(*r)->Right, q, decreased);
        if (*decreased) {
            *r = BR(*r, decreased);
        }
    } else {
        q->Data = (*r)->Data;
        Vertex* temp = *r;
        *r = (*r)->Left;
        *decreased = 1;
        free(temp);
    }
}

Vertex* DELETE(int x, Vertex* p, int* decreased) {
    if (p == NULL) {
        *decreased = 0;
        return NULL;
    }
    
    if (x < p->Data) {
        p->Left = DELETE(x, p->Left, decreased);
        if (*decreased) {
            p = BL(p, decreased);
        }
    } else if (x > p->Data) {
        p->Right = DELETE(x, p->Right, decreased);
        if (*decreased) {
            p = BR(p, decreased);
        }
    } else {
        Vertex* q = p;
        if (q->Left == NULL) {
            p = q->Right;
            *decreased = 1;
            free(q);
        } else if (q->Right == NULL) {
            p = q->Left;
            *decreased = 1;
            free(q);
        } else {
            del(&q->Left, q, decreased);
            if (*decreased) {
                p = BL(p, decreased);
            }
        }
    }
    return p;
}
