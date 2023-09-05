#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define MAX_SIZE 100000
//........................................................................................................................................

typedef struct queuePair {
    double first;
    char* pid;
}queuePair;

struct Queue {
    struct queuePair* data;
    int front;
    int rear;
    int size;
};

struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->data = (struct queuePair*)malloc(sizeof(struct queuePair) *100000);
    queue->front = -1;
    queue->rear = -1;
    queue->size = 0;
    return queue;
}

int isEmpty(struct Queue* queue) {
    return queue->size == 0;
}

void enqueue(struct Queue* queue, queuePair qp) {
    if ((queue->rear == MAX_SIZE - 1 && queue->front == 0) || (queue->rear == queue->front - 1)) {
        printf("Queue is full. Cannot enqueue.\n");
        return;
    }
    if (isEmpty(queue)) {
        queue->front = queue->rear = 0;
    }
    else if (queue->rear == MAX_SIZE - 1) {
        queue->rear = 0;
    }
    else {
        queue->rear++;
    }
    queue->data[queue->rear] = qp;
    queue->size++;
}

queuePair dequeue(struct Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue.\n");
        queuePair* qp = malloc(sizeof(queuePair));
        qp->first = -1;
        return *qp;
    }
    queuePair qp = queue->data[queue->front];
    queue->front++;
    if (queue->front > queue->rear) {
        queue->front = queue->rear = -1;
    }
    queue->size--;
    return qp;
}
//...............................................................................................................................

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

    if (left < minHeap->size && minHeap->array[left].first < minHeap->array[smallest].first)smallest = left;
    else if(left<minHeap->size && minHeap->array[left].first == minHeap->array[smallest].first){
        int result = strcmp(minHeap->array[left].pid,minHeap->array[right].pid);
        if(result<0)smallest = left;
    }
    if (right < minHeap->size && minHeap->array[right].first < minHeap->array[smallest].first)smallest = right;
    else if(right<minHeap->size && minHeap->array[right].first == minHeap->array[smallest].first){
        int result = strcmp(minHeap->array[right].pid,minHeap->array[right].pid);
        if(result<0)smallest = right;
    }

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

    while (i != 0 && (minHeap->array[i].first < minHeap->array[(i - 1) / 2].first||(minHeap->array[i].first == minHeap->array[(i - 1) / 2].first&&strcmp(minHeap->array[i].pid,minHeap->array[(i-1)/2].pid)<0)) ) {
        swap(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

struct heapPair extractMin(struct MinHeap* minHeap) {
    char str[] = "Hello, world!";
    struct heapPair minheapPair = { -1, str }; 

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
//........................................................................................................................................

typedef struct Process {
    char* pid;
    double startTime;
    double completionTime;
}Process;


int startTimeComparator(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;

    if (processA->startTime < processB->startTime) {
        return -1;
    } else if (processA->startTime > processB->startTime) {
        return 1;
    }
    else{
        int a = strcmp(processA->pid, processB->pid);
        return a < 0 ? -1 : 1;
    }
}

double durationComparator(const void *a, const void *b) {
    struct Process *p1 = *(struct Process **)a;
    struct Process *p2 = *(struct Process **)b;
    return p1->completionTime - p2->completionTime;
}

//........................................................................................................................................

double resptime(heapPair currproc , struct Process *vp, int n){
    for(int i=0;i<n;i++){
        if(vp[i].pid == currproc.pid){return vp[i].completionTime;}
    }
}
double resptimeq(queuePair currproc , struct Process *vp, int n){
    for(int i=0;i<n;i++){
        if(vp[i].pid == currproc.pid){return vp[i].completionTime;}
    }
}

// double generateExponential(double lambda) {
//     double u = (double)rand() / RAND_MAX; 
//     return -log(1 - u) / lambda;
// }

//........................................................................................................................................

void fcfs(FILE *outputFile, struct Process *vp, int n) {
    // fprintf(outputFile,"FCFS: \n");
    if (outputFile == NULL) {
        perror("Error opening the file");
        return;
    }
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    double currTime = 0.0;
    for (int i = 0; i < n; i++) {
        currTime = max(currTime, vp[i].startTime);
        double endTime = currTime + vp[i].completionTime;
        fprintf(outputFile,"%s %.3f %.3f ",vp[i].pid, currTime, endTime);
        totalTurnaroundTime += endTime - vp[i].startTime;
        totalResponseTime += currTime - vp[i].startTime;
        currTime = endTime;
    }
    totalTurnaroundTime = totalTurnaroundTime / n;
    totalResponseTime = totalResponseTime / n;
    fprintf(outputFile,"\n");
    fprintf(outputFile,"%.3f %.3f ",totalTurnaroundTime ,totalResponseTime);
    fprintf(outputFile,"\n");
    return;
}

//........................................................................................................................................

void sjf(FILE *outputFile, struct Process *vp, int n) {
    if (outputFile == NULL) {
        perror("Error opening the file");
        return;
    }
    // fprintf(outputFile,"SJF: \n");
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    int i=0;
    double currTime = 0.0;
    struct MinHeap* minHeap = createMinHeap(n);
    while(i<n || minHeap->size > 0){
        if(minHeap->size == 0) currTime = max(currTime, vp[i].startTime);
        for(;i<n;i++){
            if(vp[i].startTime <= currTime){
                totalTurnaroundTime-=vp[i].startTime;
                totalResponseTime-=vp[i].startTime;
                heapPair temp = { vp[i].completionTime, vp[i].pid };
                insert(minHeap, temp);
            }
            else break;
        }
        heapPair currProc = extractMin(minHeap);
        fprintf(outputFile,"%s %.3f %.3f ",currProc.pid, currTime, currTime + currProc.first);
        totalTurnaroundTime += currTime + currProc.first;
        totalResponseTime += currTime;
        currTime += currProc.first;
    }
    totalTurnaroundTime = totalTurnaroundTime / n;
    totalResponseTime = totalResponseTime / n;
    fprintf(outputFile,"\n");
    fprintf(outputFile,"%.3f %.3f ",totalTurnaroundTime ,totalResponseTime);
    fprintf(outputFile,"\n");
    free(minHeap->array); free(minHeap); 
    return;
}
//........................................................................................................................................


void srtf(FILE *outputFile, struct Process *vp, int n) {
    if (outputFile == NULL) {
        perror("Error opening the file");
        return;
    }
    // fprintf(outputFile,"SRTF: \n");
    Process* prevProc = (Process*)malloc(sizeof(Process));
    prevProc->pid = (char*)malloc(10*sizeof(char));
    prevProc->pid = "hello";
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    for(int i=0;i<n;i++){totalTurnaroundTime-=vp[i].startTime;totalResponseTime-=vp[i].startTime;}
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    
    int i=0;
    double currTime = 0.0;
    double nextTime = 0.0;

    struct MinHeap* minHeap = createMinHeap(n);
    
    while(i<n){
        currTime = vp[i].startTime;
        while(i<n && vp[i].startTime == currTime){
            heapPair temp = { vp[i].completionTime, vp[i].pid };
            insert(minHeap, temp);
            i++;
        }
        nextTime = (i == n)? INT_MAX: vp[i].startTime;
        while(minHeap->size > 0 && currTime < nextTime) {
            heapPair currProc = extractMin(minHeap);
            double tmp = resptime(currProc, vp, n);
            if(tmp==currProc.first){totalResponseTime+=currTime;}
            double remaining = currProc.first;
            double prevTime;
            if(nextTime - currTime < remaining){
                prevTime = currTime;
                currProc.first = remaining - nextTime + currTime;
                currTime = nextTime;
                insert(minHeap, currProc);
            }
            else{
                prevTime = currTime;
                currTime += remaining;
                currProc.first = 0;
                totalTurnaroundTime+=currTime;
            }
            if(prevProc->pid == "hello"){
                // setting the prevProc to currProc
                prevProc->pid = currProc.pid;
                prevProc->startTime = prevTime;
                prevProc->completionTime = currTime;
            }
            else if(currProc.pid == prevProc->pid){
                // updating the completionTime of prevProc
                prevProc->completionTime = currTime;
            }
            else{
                fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                prevProc->pid = currProc.pid;
                prevProc->startTime = prevTime;
                prevProc->completionTime = currTime;
            }
            if(currProc.first == 0){
                fprintf(outputFile,"%s %.3f %.3f ",prevProc->pid, prevProc->startTime, prevProc->completionTime);
                prevProc->pid = "hello";                 
            }
        }
    }
    totalTurnaroundTime = totalTurnaroundTime / n;
    totalResponseTime = totalResponseTime / n;
    fprintf(outputFile,"\n");
    fprintf(outputFile,"%.3f %.3f ",totalTurnaroundTime ,totalResponseTime);
    fprintf(outputFile,"\n");
    free(minHeap->array);free(minHeap); 
    free(prevProc->pid);free(prevProc);
    return;
}

//.......................................................................................................................................

void roundrobin(FILE *outputFile, struct Process *vp, int n, double slice) {
    if (outputFile == NULL) {
        perror("Error opening the file");
        return;
    }
    // fprintf(outputFile,"RR: \n");
    Process* prevProc = (Process* )malloc(sizeof(Process ));
    prevProc->pid = (char*)malloc(10*sizeof(char));
    prevProc->pid = "hello";
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    for(int i=0;i<n;i++){totalTurnaroundTime-=vp[i].startTime;totalResponseTime-=vp[i].startTime;}
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    int i=0;
    double currTime = 0.0;
    int completed = 0;
    struct Queue* q = createQueue();
    while(completed != n){
        while(i<n && vp[i].startTime <= currTime){
            queuePair temp = {vp[i].completionTime, vp[i].pid};
            enqueue(q, temp);
            i++;
        }
        if(q->size > 0){
            queuePair temp = dequeue(q);
            if(resptimeq(temp,vp,n)==temp.first){totalResponseTime+=currTime;}
            double remaining = temp.first;
            double prevTime = currTime;
            if(remaining > slice){
                temp.first = remaining-slice;
                currTime += slice;
                while(i<n && vp[i].startTime <= currTime){
                    queuePair nextProc = {vp[i].completionTime, vp[i].pid};
                    enqueue(q, nextProc);
                    i++;
                }
                enqueue(q, temp);
            }
            else{
                temp.first = 0;
                currTime += remaining;
                totalTurnaroundTime+=currTime;
                completed++;
            }
            if(prevProc->pid == "hello"){
                prevProc->pid = temp.pid;
                prevProc->startTime = prevTime;
                prevProc->completionTime = currTime;
            }
            else if(temp.pid == prevProc->pid){
                prevProc->completionTime = currTime;
            }
            else{
                fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                prevProc->pid = temp.pid;
                prevProc->startTime = prevTime;
                prevProc->completionTime = currTime;
            }
            if(temp.first == 0){     // if this process got completed, print this process as well and set prevProc to null for upcoming arrivals
                fprintf(outputFile,"%s %.3f %.3f ",prevProc->pid, prevProc->startTime, prevProc->completionTime);    
                prevProc->pid = "hello";             
            }
        }
        else{
            currTime = vp[i].startTime;
        }
    }
    totalTurnaroundTime = totalTurnaroundTime / n;
    totalResponseTime = totalResponseTime / n;
    fprintf(outputFile,"\n");
    fprintf(outputFile,"%.3f %.3f ",totalTurnaroundTime ,totalResponseTime);
    fprintf(outputFile,"\n");
    free(q->data);free(q);
    free(prevProc->pid);free(prevProc);
    return;
}
//........................................................................................................................................

void Qboost (struct Queue* q1, struct Queue* q2, struct Queue* q3, double *boostTime, double boost){
    while(q2->size > 0){
        queuePair temp = dequeue(q2);
        enqueue(q1, temp);
    }
    while(q3->size > 0){
        queuePair temp = dequeue(q3);
        enqueue(q1, temp);
    }
    *boostTime += boost;
    return;
}

void mlfq(FILE *outputFile, struct Process *vp, int n, double slice1, double slice2, double slice3, double boost) {
    if (outputFile == NULL) {
        perror("Error opening the file");
        return;
    }
    // fprintf(outputFile,"MLFQ: \n");
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    for(int i=0;i<n;i++){totalTurnaroundTime-=vp[i].startTime;totalResponseTime-=vp[i].startTime;}

    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    int i = 0;
    double currTime = 0.0, nextTime = 0, prevTime = 0, rem=0;
    double boost_var = 0;
    double* boostTime = &boost_var;
    *boostTime = boost;

    struct Queue* q1 = createQueue();
    struct Queue* q2 = createQueue();
    struct Queue* q3 = createQueue();

    Process* prevProc = (Process* )malloc(sizeof(Process ));
    prevProc->pid = (char*)malloc(10*sizeof(char));

    while(i<n || q1->size > 0 || q2->size > 0 || q3->size > 0){

        if(q1->size == 0 && q2->size == 0 && q3->size == 0){
            prevProc->pid = "hello";
            currTime = max(currTime, vp[i].startTime);
            *boostTime = boost*(int)((currTime+boost-1)/boost);    // setting the boostTime to a value slightly greater than currTime
            while(i<n && vp[i].startTime <= currTime){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
                i++;
            }
        }

        if(currTime >= *boostTime){
            Qboost(q1, q2, q3, boostTime, boost);
        }

        while(q1->size > 0){
            queuePair qp = dequeue(q1);
            printf("%s %.3f\n", qp.pid, qp.first);
            if(resptimeq(qp,vp,n)==qp.first){totalResponseTime+=currTime;}
            rem = qp.first;
            prevTime = currTime;
            int flag = 0;   
            if(rem > slice1){ 
                // printf("%s\n", qp.pid);      
                currTime = prevTime + slice1;  
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                qp.first = (rem-slice1);
                if(*boostTime >= prevTime && *boostTime <= currTime){
                    flag = 1;
                }
            }
            else{    // current Process has finished, so we need to print this as well
                // printf("For jobs early %s %.3f\n", qp.pid, qp.first);
                qp.first = 0;
                currTime = prevTime + rem;
                totalTurnaroundTime+=currTime;
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                if(qp.first == 0){
                    fprintf(outputFile,"%s %.3f %.3f ",prevProc->pid, prevProc->startTime, prevProc->completionTime); 
                    prevProc->pid = "hello";                
                }
            }
            for(;i<n && vp[i].startTime <= *boostTime && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }
            double temp = *boostTime;
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);
            
            for(;i<n && vp[i].startTime > temp && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }

            // if a boost has occured while the process (with duration greater than time slice) has been running, 
            // we enqueue it to q1 after its completion. else we enqueue it to q2 
            if(flag){
                enqueue(q1, qp);
            } else if(!flag && qp.first > 0){
                enqueue(q2, qp);
            }

        }

        if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);

        while (q2->size > 0 && q1->size == 0) {
            queuePair qp = dequeue(q2);
            if(resptimeq(qp,vp,n)==qp.first){totalResponseTime+=currTime;}
            rem = qp.first;
            prevTime = currTime;   // this is what our time is right now. 
            int flag = 0;
            if(rem > slice2){       
                nextTime = prevTime + slice2; 
                currTime = nextTime;                
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                qp.first = (rem-slice2);
                if(*boostTime >= prevTime && *boostTime <= currTime){
                    flag = 1;
                }
            }
            else{
                qp.first = 0;
                nextTime = prevTime + rem;
                totalTurnaroundTime+=nextTime;
                currTime = nextTime;                
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                if(qp.first == 0){
                    fprintf(outputFile,"%s %.3f %.3f ",prevProc->pid, prevProc->startTime, prevProc->completionTime); 
                    prevProc->pid = "hello";                
                }
            }
            // can the boostTime be lesser than prevTime 
            for(;i<n && vp[i].startTime <= *boostTime && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }
            double temp = *boostTime;
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);
            
            for(;i<n && vp[i].startTime > temp && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }

            // if a boost has occured while the process (with duration greater than time slice) has been running, 
            // we enqueue it to q1 after its completion. else we enqueue it to q2 
            if(flag){
                enqueue(q1, qp);
            } else if(!flag && qp.first > 0){
                enqueue(q3, qp);
            }
        }

        if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);

        while (q3->size > 0 && q2->size == 0 && q1->size == 0) {
            queuePair qp = dequeue(q3);
            if(resptimeq(qp,vp,n)==qp.first){totalResponseTime+=currTime;}
            rem = qp.first;
            prevTime = currTime;   // this is what our time is right now. 
            int flag = 0;
            if(rem > slice3){       
                nextTime = prevTime + slice3;  
                currTime = nextTime;                
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                qp.first = (rem-slice3);
                if(*boostTime >= prevTime && *boostTime <= currTime){
                    flag = 1;
                }
            }
            else{
                qp.first = 0;
                nextTime = prevTime + rem;
                totalTurnaroundTime+=nextTime;
                currTime = nextTime;                
                if(prevProc->pid == "hello"){
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                else if(qp.pid == prevProc->pid){
                    prevProc->completionTime = currTime;
                }
                else{
                    fprintf(outputFile,"%s %.3f %.3f ", prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = qp.pid;
                    prevProc->startTime = prevTime;
                    prevProc->completionTime = currTime;
                }
                if(qp.first == 0){
                    fprintf(outputFile,"%s %.3f %.3f ",prevProc->pid, prevProc->startTime, prevProc->completionTime);
                    prevProc->pid = "hello";                 
                }
            }

            for(;i<n && vp[i].startTime <= *boostTime && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }

            double temp = *boostTime;
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);
            
            for(;i<n && vp[i].startTime > temp && vp[i].startTime <= currTime; i++){
                queuePair temp = {vp[i].completionTime, vp[i].pid};
                enqueue(q1, temp);
            }

            // if a boost has occured while the process (with duration greater than time slice) has been running, 
            // we enqueue it to q1 after its completion. else we enqueue it to q2 
            if(flag){
                enqueue(q1, qp);
            } else if(!flag && qp.first > 0){
                enqueue(q3, qp);
            }
        }

    }
    totalTurnaroundTime /= n;
    totalResponseTime /= n;
    fprintf(outputFile,"\n");
    fprintf(outputFile,"%.3f %.3f ",totalTurnaroundTime ,totalResponseTime);
    fprintf(outputFile,"\n");
    free(q1->data);free(q1);
    free(q2->data);free(q2);
    free(q3->data);free(q3);
    free(prevProc->pid);free(prevProc);
    return;
}

//........................................................................................................................................


int main(int argc, char *argv[]) {
    if (argc < 8) {
        printf("Usage: %s input_file output_file num1 num2 num3 num4 num5\n", argv[0]);
        return 1;
    }

    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    double nums[5];

    for (int i = 0; i < 5; i++) {
        nums[i] = atof(argv[i + 3]);
    }

    FILE *inputFile = fopen(inputFileName, "r");
    if (!inputFile) {
        printf("Error opening files.\n");
        return 1;
    }
    struct Process *processes = NULL;
    int numEntries = 0;
    while (1) {
        processes = realloc(processes, (numEntries + 1) * sizeof(struct Process));
        if (!processes) {
            printf("Memory allocation error.\n");
            fclose(inputFile);
            return 1;
        }
        processes[numEntries].pid = (char*)malloc(10*sizeof(char));
        if (fscanf(inputFile, "%s %lf %lf", processes[numEntries].pid, &processes[numEntries].startTime, &processes[numEntries].completionTime) != 3) {
            break;
        }
        numEntries++;
    }
    // printf("%d",numEntries);
    FILE *outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        printf("Error opening output file.\n");
        free(processes);
        return 1;
    }
    fcfs(outputFile, processes, numEntries);
    roundrobin(outputFile, processes, numEntries, nums[0]);
    sjf(outputFile, processes, numEntries);
    srtf(outputFile, processes, numEntries);
    mlfq(outputFile, processes, numEntries, nums[1], nums[2], nums[3], nums[4]);
    fclose(inputFile);
    fclose(outputFile);
    for(int i=0;i<numEntries;i++){free(processes[i].pid);}
    free(processes);
    printf("Process data and process array written to %s\n", outputFileName);
    return 0;
}
//........................................................................................................................................