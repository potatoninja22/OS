#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct queuePair {
    double first;
    char* pid;
}queuePair;

struct Queue {
    queuePair items[MAX_SIZE];
    int front;
    int rear;
    int size;
};

struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = -1;
    queue->rear = -1;
    queue->size = 0;
    return queue;
}

int isEmpty(struct Queue* queue) {
    return queue->size == 0;
}

void enqueue(struct Queue* queue, queuePair qp) {
    printf("enqueue opration being done\n");
    if (queue->rear == MAX_SIZE - 1) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (isEmpty(queue)) {
        printf("queue was empty\n");
        queue->front = 0;
    }
    queue->rear++;
    queue->items[queue->rear] = qp;
    printf("%s\n", qp.pid);
    queue->size++;
}

queuePair dequeue(struct Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        queuePair* qp = malloc(sizeof(queuePair));
        qp->first = -1;
        return *qp;
    }
    queuePair qp = queue->items[queue->front];
    queue->front++;
    if (queue->front > queue->rear) {
        queue->front = queue->rear = -1;
    }
    queue->size--;
    return qp;
}

// void display(struct Queue* queue) {
//     if (isEmpty(queue)) {
//         printf("Queue is empty.\n");
//         return;
//     }
//     printf("Queue elements: ");
//     for (int i = queue->front; i <= queue->rear; i++) {
//         printf("%d ", queue->items[i]);
//     }
//     printf("\n");
// }

// int main() {
//     struct Queue* queue = createQueue();

//     enqueue(queue, 10);
//     enqueue(queue, 20);
//     enqueue(queue, 30);
    
//     display(queue);
    
//     printf("Dequeued: %d\n", dequeue(queue));
//     printf("Dequeued: %d\n", dequeue(queue));
    
//     display(queue);
    
//     return 0;
// }
