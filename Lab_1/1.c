#include <stdio.h>
#include <stdlib.h>

struct Vertex {
    int Data;
    struct Vertex *Left;
    struct Vertex *Right;
};

struct Vertex *Root = NULL;

struct Vertex* createNode(int data) {
    struct Vertex* newNode = (struct Vertex*)malloc(sizeof(struct Vertex));
    newNode->Data = data;
    newNode->Left = NULL;
    newNode->Right = NULL;
    return newNode;
}

void buildTree() {
    Root = createNode(1);
    Root->Left = createNode(2);
    Root->Left->Right = createNode(4);
    Root->Left->Right->Left = createNode(5);
    Root->Right = createNode(3);
    Root->Right->Right = createNode(6);
}

#define MAX_QUEUE 100
struct Vertex* queue[MAX_QUEUE];
int front = 0, rear = 0;

void enqueue(struct Vertex* node) {
    if (rear < MAX_QUEUE) {
        queue[rear++] = node;
    }
}

struct Vertex* dequeue() {
    if (front < rear) {
        return queue[front++];
    }
    return NULL;
}

int isQueueEmpty() {
    return front >= rear;
}

void printTreeByLevels() {
    if (Root == NULL) {
        printf("Дерево пустое.\n");
        return;
    }

    front = rear = 0;
    enqueue(Root);
    int level = 1;

    printf("Дерево по уровням:\n");

    while (!isQueueEmpty()) {
        int levelSize = rear - front;
        if (levelSize == 0) break;

        printf("Уровень %d: ", level);
        for (int i = 0; i < levelSize; i++) {
            struct Vertex* node = dequeue();
            if (node != NULL) {
                printf("%d ", node->Data);
                enqueue(node->Left);
                enqueue(node->Right);
            } else {
                printf("N ");
            }
        }
        printf("\n");
        level++;
    }
}

void preorder(struct Vertex *p) {
    if (p != NULL) {
        printf("%d ", p->Data);
        preorder(p->Left);
        preorder(p->Right);
    }
}

void inorder(struct Vertex *p) {
    if (p != NULL) {
        inorder(p->Left);
        printf("%d ", p->Data);
        inorder(p->Right);
    }
}

void postorder(struct Vertex *p) {
    if (p != NULL) {
        postorder(p->Left);
        postorder(p->Right);
        printf("%d ", p->Data);
    }
}

int size(struct Vertex *p) {
    if (p == NULL) return 0;
    else return 1 + size(p->Left) + size(p->Right);
}

int sum(struct Vertex *p) {
    if (p == NULL) return 0;
    else return p->Data + sum(p->Left) + sum(p->Right);
}

int height(struct Vertex *p) {
    if (p == NULL) return 0;
    else {
        int leftH = height(p->Left);
        int rightH = height(p->Right);
        return 1 + (leftH > rightH ? leftH : rightH);
    }
}

int sumPaths(struct Vertex *p, int level) {
    if (p == NULL) return 0;
    else return level + sumPaths(p->Left, level + 1) + sumPaths(p->Right, level + 1);
}

void freeTree(struct Vertex *p) {
    if (p != NULL) {
        freeTree(p->Left);
        freeTree(p->Right);
        free(p);
    }
}

int main() {
    buildTree();

    printTreeByLevels();
    printf("\n");

    printf("Дерево (наглядно):\n");
    printf("        1\n");
    printf("       / \\\n");
    printf("      2   3\n");
    printf("       \\   \\\n");
    printf("        4   6\n");
    printf("       /\n");
    printf("      5\n\n");

    printf("Сверху вниз (preorder): ");
    preorder(Root);
    printf("\n");

    printf("Слева направо (inorder): ");
    inorder(Root);
    printf("\n");

    printf("Снизу вверх (postorder): ");
    postorder(Root);
    printf("\n");

    int treeSize = size(Root);
    printf("Размер: %d\n", treeSize);

    int treeSum = sum(Root);
    printf("Контрольная сумма: %d\n", treeSum);

    int treeHeight = height(Root);
    printf("Высота: %d\n", treeHeight);

    int totalPaths = sumPaths(Root, 1);
    double avgHeight = (double)totalPaths / treeSize;
    printf("Средняя высота: %.2f\n", avgHeight);

    freeTree(Root);
    Root = NULL;

    return 0;
}