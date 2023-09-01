//.......................................................................................................................................
//........................................................................................................................................
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "heap.c"

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct Process {
    char* pid;
    double startTime;
    double completionTime;
}Process;

// //return type
// typedef struct{
//     double startTime;
//     double endTime;
// } Pair; 

// typedef struct {
//     char* pid;
//     Pair second;
// }PairPair;

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
    // for(int i=0;i<numEntries;i++){
    //     printf("%s %0.6f %0.f\n",processes[i].pid,processes[i].startTime,processes[i].completionTime);
    // }

    Output outfcfs = fcfs(processes, numEntries);
    int n = outfcfs.size;

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