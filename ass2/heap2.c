#include <stdio.h>
#include <stdlib.h>

struct Pair {
    int key;
    int value;
};

struct MinHeap {
    struct Pair* array;
    int capacity;
    int size;
};

void swap(struct Pair* a, struct Pair* b) {
    struct Pair temp = *a;
    *a = *b;
    *b = temp;
}

struct MinHeap* createMinHeap(int capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->array = (struct Pair*)malloc(sizeof(struct Pair) * capacity);
    minHeap->capacity = capacity;
    minHeap->size = 0;
    return minHeap;
}

void heapify(struct MinHeap* minHeap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < minHeap->size && minHeap->array[left].key < minHeap->array[smallest].key)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right].key < minHeap->array[smallest].key)
        smallest = right;

    if (smallest != i) {
        swap(&minHeap->array[i], &minHeap->array[smallest]);
        heapify(minHeap, smallest);
    }
}

void insert(struct MinHeap* minHeap, struct Pair value) {
    if (minHeap->size == minHeap->capacity) {
        printf("Heap is full, cannot insert more elements.\n");
        return;
    }

    int i = minHeap->size;
    minHeap->array[i] = value;
    minHeap->size++;

    while (i != 0 && minHeap->array[i].key < minHeap->array[(i - 1) / 2].key) {
        swap(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

struct Pair extractMin(struct MinHeap* minHeap) {
    struct Pair minPair = { -1, -1 }; // Default value for empty heap

    if (minHeap->size <= 0)
        return minPair;

    if (minHeap->size == 1) {
        minHeap->size--;
        return minHeap->array[0];
    }

    minPair = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    heapify(minHeap, 0);

    return minPair;
}

int empty(struct MinHeap* minHeap){
    if(minHeap->size==0){
        return 1;
    }
    return 0;
}

void display(struct MinHeap* minHeap) {
    printf("Min Heap:\n");
    for (int i = 0; i < minHeap->size; i++) {
        printf("(%d, %d) ", minHeap->array[i].key, minHeap->array[i].value);
    }
    printf("\n");
}

int main() {
    struct MinHeap* minHeap = createMinHeap(10);

    struct Pair p1 = { 5, 50 };
    struct Pair p2 = { 3, 30 };
    struct Pair p3 = { 8, 80 };
    struct Pair p4 = { 1, 90 };

    insert(minHeap, p1);
    insert(minHeap, p2);
    insert(minHeap, p3);
    insert(minHeap, p4);

    display(minHeap);

    struct Pair extractedPair = extractMin(minHeap);
    printf("Extracted minimum pair: (%d, %d)\n", extractedPair.key, extractedPair.value);

    display(minHeap);

    free(minHeap->array);
    free(minHeap);

    return 0;
}