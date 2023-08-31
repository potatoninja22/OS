#include <bits/stdc++.h>
using namespace std;


class Process {
private:
    string pid;
    int startTime;
    int completionTime;

public:
    string getPid() const { return pid; }
    int getCompletionTime() const { return completionTime; }
    int getStartTime() const { return startTime; }

    void setCompletionTime(int time) { this->completionTime = time; }

    Process(string pid, float startTime, float completionTime){
        this->pid = pid;
        this->startTime = startTime;
        this->completionTime = completionTime;
    }
    
    static bool startTimeComparator(Process *p1, Process *p2){
        return p1->getStartTime() < p2->getStartTime();
    }

    static bool durationComparator(Process *p1, Process *p2){
        if(p1->getCompletionTime() != p2->getCompletionTime())
        return p1->getCompletionTime() > p2->getCompletionTime();

        return p1->getStartTime() > p2->getStartTime();
    }

    static bool durationComparator2(Process* p1, Process* p2){
        return p1->getCompletionTime() < p2->getCompletionTime();
    }
};

// The function returns the process id 
vector<pair<string, pair<int,int>>> fcfs (vector<Process* > vp){
    sort(vp.begin(), vp.end(), Process::startTimeComparator);
    vector<pair<string, pair<int,int>>> schedule;
    int currTime = 0;
    for(auto it: vp){
        currTime = max(it->getStartTime(), currTime);
        int endTime = currTime + it->getCompletionTime();
        schedule.push_back(make_pair(it->getPid(), make_pair(currTime, endTime)));
        currTime = endTime;
    }

    return schedule;
}


vector<pair<string,pair<int,int>>> sjf(vector<Process* > vp){
    vector<pair<string,pair<int,int>>> schedule;
    sort(vp.begin(), vp.end(), Process::startTimeComparator);
    map<Process*, bool> mp;
    int n = vp.size();
    int i=0; int currTime = 0;
    priority_queue<Process*, vector<Process*>, decltype(&Process::durationComparator)> pq(Process:: durationComparator);
    while(i<n || pq.size() > 0){
        if(pq.size() == 0) currTime = max(currTime, vp[i]->getStartTime());
        for(;i<n;i++){
            if(vp[i]->getStartTime() <= currTime && mp.find(vp[i]) == mp.end()) pq.push(vp[i]);
            else break;
        }
        auto proc = pq.top();
        pq.pop();
        mp[proc] = true;
        schedule.push_back(make_pair(proc->getPid(), make_pair(currTime, currTime + proc->getCompletionTime())));
        currTime += proc->getCompletionTime();
    }

    return schedule;
}

vector<pair<string, pair<int,int>>> srtf(vector<Process* > temp){
    vector<Process* > vp = temp;  // make a shallow copy to not have pointer bt?
    vector<pair<string, pair<int,int>>> schedule;
    sort(vp.begin(), vp.end(), Process:: startTimeComparator);
    int n = vp.size();
    vector<int> arrivalTimes;
    for(auto it:vp) arrivalTimes.push_back(it->getStartTime());

    priority_queue<Process*, vector<Process*>, decltype(&Process::durationComparator)> pq(Process:: durationComparator);

    int i=0, currTime = 0, nextTime = 0;
    while(i<n){
        // cout<<i<<endl;
        currTime = arrivalTimes[i];
        while(i<n && arrivalTimes[i] == currTime){
            pq.push(vp[i]);
            i++;
        }
        nextTime = (i == n)? INT_MAX: arrivalTimes[i];
        // cout<<i<<" "<<currTime<<" "<<nextTime<<endl;
        while(pq.size() > 0 && currTime < nextTime) {
            auto it = pq.top(); 
            pq.pop();
            int remaining = it->getCompletionTime();
            if(nextTime - currTime < remaining){
                int prevTime = currTime;
                it->setCompletionTime(remaining-nextTime+currTime);
                currTime = nextTime;
                // cout<<it->getPid()<<" "<<it->getCompletionTime()<<endl;
                pq.push(it);
                schedule.push_back(make_pair(it->getPid(), make_pair(prevTime, currTime)));
            }
            else{
                int prevTime = currTime;
                currTime += remaining;
                it->setCompletionTime(0);
                schedule.push_back(make_pair(it->getPid(), make_pair(prevTime, currTime)));
            }
        }
    }

    cout<<"This loop was exited"<<endl;

    while(pq.size() > 0){
        auto it = pq.top();
        pq.pop();
        int prevTime = currTime;
        currTime += it->getCompletionTime();
        schedule.push_back(make_pair(it->getPid(), make_pair(prevTime, currTime)));
    }
    
    return schedule;
}

vector<pair<string, pair<int, int>>> roundrobin(vector<Process* > vp, int slice){
    vector<pair<string, pair<int,int>>> schedule;
    sort(vp.begin(), vp.end(), Process:: startTimeComparator);
    vector<int> rem;
    for(auto it: vp) rem.push_back(it->getCompletionTime());
    int n = vp.size();
    int i=0;
    int currTime = 0;
    int completed = 0;
    queue<Process* > q;
    while(completed != n){
        while(i<n && vp[i]->getStartTime() <= currTime){
            q.push(vp[i]);
            i++;
        }
        if(q.size() > 0){
            auto it = q.front();
            q.pop();
            int rem = it->getCompletionTime();
            if(rem > slice){
                int prev = currTime;
                it->setCompletionTime(rem-slice);
                currTime += slice;
                schedule.push_back(make_pair(it->getPid(), make_pair(prev, currTime)));
                while(i<n && vp[i]->getStartTime() <= currTime){
                    q.push(vp[i]);
                    i++;
                }
                q.push(it);
            }
            else{
                int prev = currTime;
                it->setCompletionTime(0);
                currTime += rem;
                schedule.push_back(make_pair(it->getPid(), make_pair(prev, currTime)));
                completed++;
            }
        }
        else{
            currTime = vp[i]->getStartTime();
        }
    }

    return schedule;

}

int main(){
    // int n = argv[1]  // number of processes
    // int lamda = argv[2];  // inter-arrival time parameter
    vector<Process* > listProcess;   // list to store all the processes

    Process* process1 = new Process("Job1", 1, 100);
    Process* process2 = new Process("Job2", 5, 3);
    Process* process3 = new Process("Job3", 4, 5);
    Process* process4 = new Process("Job4", 20, 2);

    listProcess.push_back(process1);
    listProcess.push_back(process2);
    listProcess.push_back(process3);
    listProcess.push_back(process4);

    auto ans = roundrobin(listProcess, 3);
    for(auto it:ans){
        cout<<it.first<<" "<<it.second.first<<" "<<it.second.second<<endl;
    }
    return 0;
}
