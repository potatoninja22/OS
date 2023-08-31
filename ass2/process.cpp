#include <bits/stdc++.h>
using namespace std;

class Process {
private:
    int pid;
    int runTime;
    int startTime;

public:
    int getPid() const { return pid; }
    int getRunTime() const { return runTime; }
    int getStartTime() const { return startTime; }

    Process(int pid, int startTime, int runTime) : pid(pid), runTime(runTime), startTime(startTime) {}

    static function<bool(const Process&, const Process&)> startTimeComparator() {
        return [](const Process& p1, const Process& p2) {
            return p1.startTime < p2.startTime;
        };
    }

    bool operator<(const Process& p) const {
        return runTime < p.runTime;
    }
};