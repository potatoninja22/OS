//.......................................................................................................................................
//........................................................................................................................................
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Process {
    char pid[10];
    double startTime;
    double completionTime;
};

typedef struct{
    double startTime;
    double endTime;
} Pair; 

typedef struct {
    char* pid;
    Pair second;
}PairPair;

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

typedef struct {
    char output[1000000];
    double avgTurnaroundTime;
    double avgResponseTime;
} FCFSOutput;

FCFSOutput fcfs(struct Process **vp, int n) {

    FCFSOutput output;
    output.output[0] = '\0';
    qsort(vp, n, sizeof(struct Process *), startTimeComparator);
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;
    double currTime = 0.0;
    
    for (int i = 0; i < n; i++) {
        currTime = (vp[i]->startTime > currTime) ? vp[i]->startTime : currTime;
        double endTime = currTime + vp[i]->completionTime;
        
        char line[10000];
        sprintf(line, "%s %.6lf %.6lf ", vp[i]->pid, currTime, endTime);
        strcat(output.output, line);
        
        totalTurnaroundTime += endTime - vp[i]->startTime;
        totalResponseTime += currTime - vp[i]->startTime;
        
        currTime = endTime;
    }
    
    output.avgTurnaroundTime = totalTurnaroundTime / n;
    output.avgResponseTime = totalResponseTime / n;
    
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

        if (fscanf(inputFile, "%s %lf %lf", processes[numEntries].pid, &processes[numEntries].startTime, &processes[numEntries].completionTime) != 3) {
            break;
        }
        numEntries++;
    }

    printf("%d\n",numEntries);
    for(int i=0;i<numEntries;i++){
        printf("%s %0.6f %0.f\n",processes[i].pid,processes[i].startTime,processes[i].completionTime);
    }

    struct Process **processesArray = malloc(numEntries * sizeof(struct Process *));
    for (int i = 0; i < numEntries; i++) {
        processesArray[i] = &processes[i];
    }

    FCFSOutput fcfsStats = fcfs(processesArray, numEntries);

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error opening output file.\n");
        free(processes);
        free(processesArray);
        return 1;
    }

    fprintf(outputFile, "%s\n", fcfsStats.output);
    fprintf(outputFile, "%.2lf %.2lf\n", fcfsStats.avgTurnaroundTime, fcfsStats.avgResponseTime);
    fclose(inputFile);
    fclose(outputFile);

    free(processes);
    free(processesArray);

    printf("Process data and process array written to %s\n", outputFileName);

    return 0;
}
//........................................................................................................................................