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
//........................................................................................................................................

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
    }
    return 0;
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

double generateExponential(double lambda) {
    double u = (double)rand() / RAND_MAX; 
    return -log(1 - u) / lambda;
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
        output.size++;
        output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
        currTime = max(currTime, vp[i].startTime);
        double endTime = currTime + vp[i].completionTime;
        output.schedule[i].pid = vp[i].pid;
        output.schedule[i].startTime = currTime;
        output.schedule[i].completionTime = endTime;
        totalTurnaroundTime += endTime - vp[i].startTime;
        totalResponseTime += currTime - vp[i].startTime;
        currTime = endTime;
    }
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    return output;
}

//........................................................................................................................................

Output sjf(struct Process *vp, int n) {
    Output output;
    output.schedule = NULL;
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    int i=0;
    double currTime = 0.0;
    output.size = 0;
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
        output.size++;
        output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
        output.schedule[output.size-1].pid = currProc.pid;
        output.schedule[output.size-1].startTime = currTime;
        output.schedule[output.size-1].completionTime = currTime + currProc.first;
        totalTurnaroundTime += currTime + currProc.first;
        totalResponseTime += currTime;
        currTime += currProc.first;

    }
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    return output;
}
//........................................................................................................................................


Output srtf(struct Process *vp, int n) {
    Output output;
    output.schedule = NULL;
    output.size = 0;
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    int i=0;
    double currTime = 0.0;
    double nextTime = 0.0;
    struct MinHeap* minHeap = createMinHeap(n);
    while(i<n){
        currTime = vp[i].startTime;
        while(i<n && vp[i].startTime == currTime){
            totalTurnaroundTime-=vp[i].startTime;
            totalResponseTime-=vp[i].startTime;
            heapPair temp = { vp[i].completionTime, vp[i].pid };
            insert(minHeap, temp);
            i++;
        }
        nextTime = (i == n)? INT_MAX: vp[i].startTime;
        while(minHeap->size > 0 && currTime < nextTime) {
            heapPair currProc = extractMin(minHeap);
            double tmp = resptime(currProc, vp, n);
            if(tmp==currProc.first){totalResponseTime+=currTime;}
            output.size++;
            output.schedule = (Process* )realloc(output.schedule, output.size*(sizeof(Process)));
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
            output.schedule[output.size-1].pid = currProc.pid;
            output.schedule[output.size-1].startTime = prevTime;
            output.schedule[output.size-1].completionTime = currTime;
        }
    }
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    return output;
}

//.......................................................................................................................................

Output roundrobin(struct Process *vp, int n, double slice){
    Output output;
    output.schedule = (Process* )malloc(10000000*(sizeof(Process)));
    output.size = 0;
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    int i=0,j=0;
    double currTime = 0.0;
    int completed = 0;
    struct Queue* q = createQueue();
    while(completed != n){
        while(i<n && vp[i].startTime <= currTime){
            totalResponseTime-=vp[i].startTime; totalTurnaroundTime-=vp[i].startTime;
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
                currTime += remaining;
                totalTurnaroundTime+=currTime;
                completed++;
            }
            output.size++;
            output.schedule[output.size-1].pid = (char*)malloc(10*sizeof(char));
            output.schedule[output.size-1].pid = temp.pid;
            output.schedule[output.size-1].startTime = prevTime;
            output.schedule[output.size-1].completionTime = currTime;
        }
        else{
            currTime = vp[i].startTime;
        }
        j++;
    }
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    return output;
}

//........................................................................................................................................


int main(int argc, char *argv[]) {
    if (argc < 7) {
        printf("Usage: %s output_file num1 num2 num3 num4 num5\n", argv[0]);
        return 1;
    }
    char *outputFileName = argv[1];
    double nums[5];
    for (int i = 0; i < 5; i++) {
        nums[i] = atof(argv[i + 2]);
    }
    int n=5,i=0;
    // struct Process *processes = NULL;
    struct Process* processes = (Process* )malloc(1000000*(sizeof(Process)));
    int numEntries = 0;
    while(i<=n){
        // processes = realloc(processes, (numEntries+1) * sizeof(struct Process));
        processes[numEntries].pid = (char*)malloc(10*sizeof(char));
        sprintf(processes[numEntries].pid, "j%d",i);
        processes[numEntries].startTime = generateExponential(0.06);
        processes[numEntries].completionTime = generateExponential(0.06);
        i++; numEntries++;
    }
    Output outf = fcfs(processes, numEntries);
    Output outrr = roundrobin(processes, numEntries,nums[0]);
    Output outsjf = sjf(processes, numEntries);
    Output outsrtf = srtf(processes, numEntries);
    // Output outmlfq = mlfq(processes, numEntries,nums[1],nums[2],nums[3],nums[4]);

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error opening output file.\n");
        free(processes);
        return 1;
    }
    for(int i=0;i<outf.size;i++){
        fprintf(outputFile,"%s %.6f %.6f\n", outf.schedule[i].pid, outf.schedule[i].startTime, outf.schedule[i].completionTime);
    }fprintf(outputFile,"%0.6f %.6f\n", outf.avgTurnaroundTime, outf.avgResponseTime);

    for(int i=0;i<outrr.size;i++){
        fprintf(outputFile,"%s %.6f %.6f\n", outrr.schedule[i].pid, outrr.schedule[i].startTime, outrr.schedule[i].completionTime);
    }fprintf(outputFile,"%0.6f %.6f\n", outrr.avgTurnaroundTime, outrr.avgResponseTime);

    for(int i=0;i<outsjf.size;i++){
        fprintf(outputFile,"%s %.6f %.6f\n", outsjf.schedule[i].pid, outsjf.schedule[i].startTime, outsjf.schedule[i].completionTime);
    }fprintf(outputFile,"%0.6f %.6f\n", outsjf.avgTurnaroundTime, outsjf.avgResponseTime);

    for(int i=0;i<outsrtf.size;i++){
        fprintf(outputFile,"%s %.6f %.6f\n", outsrtf.schedule[i].pid, outsrtf.schedule[i].startTime, outsrtf.schedule[i].completionTime);
    }fprintf(outputFile,"%0.6f %.6f\n", outsrtf.avgTurnaroundTime, outsrtf.avgResponseTime);

    // for(int i=0;i<outmlfq.size;i++){
    //     fprintf("%s %.6f %.6f ", outmlfq.schedule[i].pid, outmlfq.schedule[i].startTime, outmlfq.schedule[i].completionTime);
    // }
    // fprintf("%0.6f %.6f\n", outmlfq.avgTurnaroundTime, outmlfq.avgResponseTime);
    fclose(outputFile);
    free(processes);
    printf("Process data and process array written to %s\n", outputFileName);

    return 0;
}
//........................................................................................................................................