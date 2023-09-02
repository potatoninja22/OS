//.......................................................................................................................................
//........................................................................................................................................
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "heap.c"
#include "queue.c"

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct Process {
    char* pid;
    double startTime;
    double completionTime;
}Process;

typedef struct {
    Process* schedule;
    double avgTurnaroundTime;
    double avgResponseTime;
    int size;
} Output;

int startTimeComparator(const void *a, const void *b) {
    const struct Process *processA = (const struct Process *)a;
    const struct Process *processB = (const struct Process *)b;

    if (processA->startTime < processB->startTime) {
        return -1;
    } else if (processA->startTime > processB->startTime) {
        return 1;
    } else{
        if(processA->completionTime < processB->completionTime) {
            return -1;
        }
        else if(processA->completionTime > processB->completionTime){
            return 1;
        }
    }
    return 0;
}


double durationComparator(const void *a, const void *b) {
    struct Process *p1 = *(struct Process **)a;
    struct Process *p2 = *(struct Process **)b;
    return p1->completionTime - p2->completionTime;
}

//........................................................................................................................................


Output fcfs(struct Process *vp, int n) {
    Output output;

    output.schedule = NULL;

    qsort(vp, n, sizeof(struct Process), startTimeComparator);

    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    double currTime = 0.0;
    output.size = 0;

    for (int i = 0; i < n; i++) {
        //currTime = (vp[i].startTime > currTime) ? vp[i].startTime : currTime;
        //printf("%d\n", i);
        output.size++;
        output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));

        currTime = max(currTime, vp[i].startTime);
        double endTime = currTime + vp[i].completionTime;

        //output->schedule[i].pid = vp[i].pid;
        output.schedule[i].pid = vp[i].pid;
        output.schedule[i].startTime = currTime;
        output.schedule[i].completionTime = endTime;

        //printf("%.6f %.6f\n", output.schedule[i].startTime, output.schedule[i].completionTime);
        
        totalTurnaroundTime += endTime - vp[i].startTime;
        totalResponseTime += currTime - vp[i].startTime;
        
        currTime = endTime;
    }
    
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    
    return output;
}


Output sjf(struct Process *vp, int n) {
    Output output;

    output.schedule = NULL;

    qsort(vp, n, sizeof(struct Process), startTimeComparator);

    // double totalTurnaroundTime = 0.0;
    // double totalResponseTime = 0.0;
    int i=0;
    double currTime = 0.0;
    output.size = 0;
    struct MinHeap* minHeap = createMinHeap(n);
    while(i<n || minHeap->size > 0){
        if(minHeap->size == 0) currTime = max(currTime, vp[i].startTime);
        for(;i<n;i++){
            if(vp[i].startTime <= currTime){
                heapPair temp = { vp[i].completionTime, vp[i].pid };
                insert(minHeap, temp);
            }
            else break;
        }

        heapPair currProc = extractMin(minHeap);
        output.size++;
        
        output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
        output.schedule[output.size-1].pid = currProc.pid;
        output.schedule[output.size-1].startTime = currTime;
        output.schedule[output.size-1].completionTime = currTime + currProc.first;

        currTime += currProc.first;

    }
    // output.avgTurnaroundTime = totalTurnaroundTime / n;
    // output.avgResponseTime = totalResponseTime / n;
    
    return output;
}

Output srtf(struct Process *vp, int n) {
    Output output;
    output.schedule = NULL;
    output.size = 0;

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
            output.size++;
            output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));

            double remaining = currProc.first;
            double prevTime = currTime;
            if(nextTime - currTime < remaining){
                currProc.first = remaining - nextTime + currTime;
                currTime = nextTime;
                insert(minHeap, currProc);
            }
            else{
                currTime += remaining;
                currProc.first = 0;
            }

            output.schedule[output.size-1].pid = currProc.pid;
            output.schedule[output.size-1].startTime = prevTime;
            output.schedule[output.size-1].completionTime = currTime;
            
        }
    }
    // output.avgTurnaroundTime = totalTurnaroundTime / n;
    // output.avgResponseTime = totalResponseTime / n;
    
    return output;
}

Output roundrobin(struct Process *vp, int n, double slice) {
    Output output;
    output.schedule = NULL;
    output.size = 0;

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
                currTime += remaining;
                completed++;
            }
            output.size++;
            output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
            output.schedule[output.size-1].pid = temp.pid;
            output.schedule[output.size-1].startTime = prevTime;
            output.schedule[output.size-1].completionTime = currTime;
        }
        else{
            currTime = vp[i].startTime;
        }
    }
    // output.avgTurnaroundTime = totalTurnaroundTime / n;
    // output.avgResponseTime = totalResponseTime / n;
    
    return output;
}


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

Output mlfq(struct Process *vp, int n, double slice1, double slice2, double slice3, double boost) {
    Output output;
    output.schedule = NULL;
    output.size = 0;

    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    int i=0;
    double currTime = 0.0, nextTime = 0, prevTime = 0;
    double boost_var = 0;
    double* boostTime = &boost_var;
    *boostTime = boost;

    struct Queue* q1 = createQueue();
    struct Queue* q2 = createQueue();
    struct Queue* q3 = createQueue();
    while(i<n || q1->size > 0 || q2->size > 0 || q3->size > 0){
        if(q1->size == 0 && q2->size == 0 && q3->size == 0){
            currTime = max(currTime, vp[i].startTime);
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
            int rem = qp.first;
            prevTime = currTime;   // this is what our time is right now. 
            if(rem > slice1){       
                nextTime = prevTime + slice1; 
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
                qp.first = (rem-slice1);
                enqueue(q2, qp);
            }
            else{
                nextTime = prevTime + rem;
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
            }
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);

        }

        if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);

        while (q2->size > 0 && q1->size == 0) {
            queuePair qp = dequeue(q2);
            int rem = qp.first;
            prevTime = currTime;   // this is what our time is right now. 
            if(rem > slice2){       
                nextTime = prevTime + slice2; 
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
                qp.first = (rem-slice2);
                enqueue(q3, qp);
            }
            else{
                nextTime = prevTime + rem;
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
            }
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);
        }

        while (q3->size > 0 && q2->size == 0 && q1->size == 0) {
            queuePair qp = dequeue(q3);
            int rem = qp.first;
            prevTime = currTime;   // this is what our time is right now. 
            if(rem > slice3){       
                nextTime = prevTime + slice3; 
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
                qp.first = (rem-slice3);
                enqueue(q3, qp);
            }
            else{
                nextTime = prevTime + rem;
                for(;i<n && vp[i].startTime <= nextTime;i++){
                    queuePair temp = {vp[i].completionTime, vp[i].pid};
                    enqueue(q1, temp);
                } 
                currTime = nextTime;                
                output.size++;
                output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
                output.schedule[output.size-1].pid = qp.pid;
                output.schedule[output.size-1].startTime = prevTime;
                output.schedule[output.size-1].completionTime = currTime;
            }
            if(currTime >= *boostTime) Qboost(q1, q2, q3, boostTime, boost);
        }

        
    }
    // output.avgTurnaroundTime = totalTurnaroundTime / n;
    // output.avgResponseTime = totalResponseTime / n;
    return output;
}


//.......................................................................................................................................

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

    printf("%d\n",numEntries);
    for(int i=0;i<numEntries;i++){
        printf("%s %0.6f %0.f\n",processes[i].pid,processes[i].startTime,processes[i].completionTime);
    }
    printf("hello1");
    Output outfcfs = mlfq(processes, numEntries, 5, 3, 2, 8);
    int n = outfcfs.size;
    printf("hello2");
    for(int i=0;i<n;i++){
        printf("%s %.6f %.6f\n", outfcfs.schedule[i].pid, outfcfs.schedule[i].startTime, outfcfs.schedule[i].completionTime);
    }

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error opening output file.\n");
        free(processes);
        return 1;
    }
    fclose(inputFile);
    fclose(outputFile);

    free(processes);

    printf("Process data and process array written to %s\n", outputFileName);

    return 0;
}
//........................................................................................................................................