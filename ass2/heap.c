#include <stdio.h>
#include <stdlib.h>

typedef struct heapPair {
    double first;
    char* pid;
}heapPair;

typedef struct MinHeap {
    struct heapPair* array;
    int capacity;
    int size;
}MinHeap;

void swap(struct heapPair* a, struct heapPair* b) {
    struct heapPair temp = *a;
    *a = *b;
    *b = temp;
}

struct MinHeap* createMinHeap(int capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->array = (struct heapPair*)malloc(sizeof(struct heapPair) * capacity);
    minHeap->capacity = capacity;
    minHeap->size = 0;
    return minHeap;
}

void heapify(struct MinHeap* minHeap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < minHeap->size && minHeap->array[left].first < minHeap->array[smallest].first)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right].first < minHeap->array[smallest].first)
        smallest = right;

    if (smallest != i) {
        swap(&minHeap->array[i], &minHeap->array[smallest]);
        heapify(minHeap, smallest);
    }
}

void insert(struct MinHeap* minHeap, struct heapPair second) {
    if (minHeap->size == minHeap->capacity) {
        printf("Heap is full, cannot insert more elements.\n");
        return;
    }

    int i = minHeap->size;
    minHeap->array[i] = second;
    minHeap->size++;

    while (i != 0 && minHeap->array[i].first < minHeap->array[(i - 1) / 2].first) {
        swap(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

struct heapPair extractMin(struct MinHeap* minHeap) {
    char str[] = "Hello, world!";
    struct heapPair minheapPair = { -1, str }; // Default second for empty heap

    if (minHeap->size <= 0)
        return minheapPair;

    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    minheapPair = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    heapify(minHeap, 0);

    return minheapPair;
}

int empty(struct MinHeap* minHeap){
    if(minHeap->size==0){
        return 1;
    }
    return 0;
}

// void display(struct MinHeap* minHeap) {
//     printf("Min Heap:\n");
//     for (int i = 0; i < minHeap->size; i++) {
//         printf("(%d, %d) ", minHeap->array[i].first, minHeap->array[i].second);
//     }
//     printf("\n");
// }

// int main() {
//     printf("hello");
//     struct MinHeap* minHeap = createMinHeap(10);

//     struct heapPair p1 = { 5, 50 };
//     struct heapPair p2 = { 3, 30 };
//     struct heapPair p3 = { 8, 80 };
//     struct heapPair p4 = { 1, 90 };

//     insert(minHeap, p1);
//     insert(minHeap, p2);
//     insert(minHeap, p3);
//     insert(minHeap, p4);

//     display(minHeap);

//     struct heapPair extractedheapPair = extractMin(minHeap);
//     printf("Extracted minimum pair: (%d, %d)\n", extractedheapPair.first, extractedheapPair.second);

//     display(minHeap);

//     free(minHeap->array);
//     free(minHeap);

//     return 0;
// }