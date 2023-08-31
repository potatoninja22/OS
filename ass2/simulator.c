#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Process {
    char pid[10];
    int startTime;
    int completionTime;
};

int startTimeComparator(const void *a, const void *b) {
    return ((struct Process *)a)->startTime - ((struct Process *)b)->startTime;
}

int durationComparator(const void *a, const void *b) {
    if (((struct Process *)a)->completionTime != ((struct Process *)b)->completionTime)
        return ((struct Process *)b)->completionTime - ((struct Process *)a)->completionTime;

    return ((struct Process *)b)->startTime - ((struct Process *)a)->startTime;
}

int durationComparator2(const void *a, const void *b) {
    return ((struct Process *)a)->completionTime - ((struct Process *)b)->completionTime;
}

// Returns the process id
struct SchedulePair {
    char pid[10];
    int startTime;
    int endTime;
};

struct SchedulePair* fcfs(struct Process *vp, int n) {
    printf("helllo there\n");
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    struct SchedulePair *schedule = (struct SchedulePair *)malloc(n * sizeof(struct SchedulePair));
    int currTime = 0;
    for (int i = 0; i < n; i++) {
        currTime = currTime > vp[i].startTime ? currTime : vp[i].startTime;
        int endTime = currTime + vp[i].completionTime;
        strcpy(schedule[i].pid, vp[i].pid);
        schedule[i].startTime = currTime;
        schedule[i].endTime = endTime;
        currTime = endTime;
    }
    return schedule;
}

struct SchedulePair* sjf(struct Process *vp, int n) {
    qsort(vp, n, sizeof(struct Process), startTimeComparator);
    struct SchedulePair *schedule = (struct SchedulePair *)malloc(n * sizeof(struct SchedulePair));
    int currTime = 0;
    int i = 0;
    int completed = 0;
    struct Process *pq[n];
    int pqSize = 0;
    while (i < n || pqSize > 0) {
        if (pqSize == 0) currTime = currTime > vp[i].startTime ? currTime : vp[i].startTime;
        for (; i < n; i++) {
            if (vp[i].startTime <= currTime) {
                pq[pqSize++] = &vp[i];
            } else {
                break;
            }
        }
        qsort(pq, pqSize, sizeof(struct Process *), durationComparator);
        struct Process *proc = pq[--pqSize];
        schedule[completed].startTime = currTime;
        schedule[completed].endTime = currTime + proc->completionTime;
        strcpy(schedule[completed++].pid, proc->pid);
        currTime += proc->completionTime;
    }
    return schedule;
}

struct SchedulePair* srtf(struct Process *vp, int n) {
    struct Process temp[n];
    for (int i = 0; i < n; i++) temp[i] = vp[i];
    qsort(temp, n, sizeof(struct Process), startTimeComparator);
    struct SchedulePair *schedule = (struct SchedulePair *)malloc(2 * n * sizeof(struct SchedulePair));
    int arrivalTimes[n];
    for (int i = 0; i < n; i++) arrivalTimes[i] = temp[i].startTime;

    struct Process *pq[n];
    int pqSize = 0;

    int i = 0;
    int currTime = 0;
    int nextTime = 0;
    int scheduleSize = 0;
    while (i < n) {
        currTime = arrivalTimes[i];
        while (i < n && arrivalTimes[i] == currTime) {
            pq[pqSize++] = &temp[i];
            i++;
        }
        nextTime = (i == n) ? 1000000000 : arrivalTimes[i];
        qsort(pq, pqSize, sizeof(struct Process *), durationComparator2);
        while (pqSize > 0 && currTime < nextTime) {
            struct Process *proc = pq[--pqSize];
            int remaining = proc->completionTime;
            if (nextTime - currTime < remaining) {
                int prevTime = currTime;
                proc->completionTime = remaining - (nextTime - currTime);
                currTime = nextTime;
                pq[pqSize++] = proc;
                strcpy(schedule[scheduleSize].pid, proc->pid);
                schedule[scheduleSize].startTime = prevTime;
                schedule[scheduleSize++].endTime = currTime;
            } else {
                int prevTime = currTime;
                currTime += remaining;
                proc->completionTime = 0;
                strcpy(schedule[scheduleSize].pid, proc->pid);
                schedule[scheduleSize].startTime = prevTime;
                schedule[scheduleSize++].endTime = currTime;
            }
        }
    }

    while (pqSize > 0) {
        struct Process *proc = pq[--pqSize];
        int prevTime = currTime;
        currTime += proc->completionTime;
        strcpy(schedule[scheduleSize].pid, proc->pid);
        schedule[scheduleSize].startTime = prevTime;
        schedule[scheduleSize++].endTime = currTime;
    }

    return schedule;
}

int main() {
    printf("\n********************************");
    struct Process listProcess[4]; // Change the size accordingly
    printf("hello there what is up");
    strcpy(listProcess[0].pid, "Job1");
    listProcess[0].startTime = 1;
    listProcess[0].completionTime = 100;

    strcpy(listProcess[1].pid, "Job2");
    listProcess[1].startTime = 5;
    listProcess[1].completionTime = 3;

    strcpy(listProcess[2].pid, "Job3");
    listProcess[2].startTime = 4;
    listProcess[2].completionTime = 5;

    strcpy(listProcess[3].pid, "Job4");
    listProcess[3].startTime = 20;
    listProcess[3].completionTime = 2;

    int n = sizeof(listProcess) / sizeof(listProcess[0]);

    struct SchedulePair *ans = fcfs(listProcess, n);
    for (int i = 0; i < n; i++) {
        printf("%s %d %d\n", ans[i].pid, ans[i].startTime, ans[i].endTime);
    }

    free(ans);

    return 0;
}
