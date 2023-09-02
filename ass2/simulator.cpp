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
    //map<Process*, bool> mp;
    int n = vp.size();
    int i=0; int currTime = 0;
    priority_queue<Process*, vector<Process*>, decltype(&Process::durationComparator)> pq(Process:: durationComparator);
    while(i<n || pq.size() > 0){
        if(pq.size() == 0) currTime = max(currTime, vp[i]->getStartTime());
        for(;i<n;i++){
            if(vp[i]->getStartTime() <= currTime) pq.push(vp[i]);
            else break;
        }
        auto proc = pq.top();
        pq.pop();
        //mp[proc] = true;
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

void Qboost (queue<Process* > &q1, queue<Process* > &q2, queue<Process* > &q3, int* boostTime, int boost){
    q1.insert(q1.end(), q2.begin(), q2.end());
    q2.clear();
    q1.insert(q1.end(), q3.begin(), q3.end());
    q3.clear();
    boostTime += boost;
}

vector<pair<string, pair<int, int>>> MLFQ(vector<Process* > vp, int slice1, int slice2, int slice3, int boost) {
    vector<pair<string, pair<int,int>>> schedule;
    map<int, int> sl2;
    map<int, int> sl3;
    queue<Process* > q1, q2, q3;
    int i = 0, currTime = 0, nextTime = 0, prevTime = 0;
    int boostTime = boost;
    while (i < n || !q1.empty() || !q2.empty() || !q3.empty()) {
        if (q1.empty() && q2.empty() && q3.empty()) {
            currTime = max(currTime, vp[i]->getStartTime());           // since all the queues are empty there is nothing to boost, hence we dont need to check for boostTime
            while(i<n && vp[i]->getStartTime() <= currTime)
            q1.push_back(vp[i++]);
        }
        if (currTime >= boostTime) {
            Qboost(q1, q2, q3, boostTime, boost);
        }
        while (!q1.empty()) {
            Process* p = q1.front();
            q1.pop();
            int rem = p->getCompletionTime();
            prevTime = currTime;   // this is what our time is right now. 
            if(rem > slice1){          // if rem is greater than slice1, we will check for new arrivals and boosts between prevTime and prevTime + slice1. After this our current job will get finished and we will push it to queue2
                nexTime = prevTime + slice1;   // take into account whatever happens between prevTime and nexTime (arrival + boost)

                if(boostTime > prevTime && boostTime < nextTime){
                    for(;i<n && vp[i]->getStartTime() >= prevTime && vp[i]->getStartTime() <= boostTime;i++){
                        q1.push(vp[i]);
                    }
                    int temp = boostTime;
                    Qboost(q1, q2, q3, boostTime, boost);
                    for(;i<n && vp[i]->getStartTime() >= temp && vp[i]->getStartTime() <= nextTime;i++){
                        q1.push(vp[i]);
                    }
                }
                // now everything that was supposed to happen in the intermediate time interval has happened, and I can set currTime to nextTime
                // I will also push the process to Q2 since it did not get completed within the time slice
                currTime = nextTime;                
                schedule.push_back({p->getPid(), {prevTime, currTime}});
                p->setCompletionTime(rem-slice1);
                q2.push(p);
            }
            else{
                nextTime = prevTime + rem;
                // first take in all new arrivals in between nextTime and prevTime
                for(;i<n && vp[i]->getStartTime()<=nextTime;i++){
                    q1.push(vp[i]);
                }
                // now everything that was supposed to happen in the intermediate time interval has happened, and I can set currTime to nextTime
                // the process p is complete, so no need to push to q2
                currTime = nextTime;
                if(currTime >= boostTime){
                    Qboost(q1, q2, q3, boostTime, boost);
                }
                schedule.push_back({p->getPid(), {prevTime, currTime}});
            }

        // now we come to the time when q1 has become empty and q2 is non empty. We will consider time slice2; First process in q2 will run from 

        while (!q2.empty() && q1.empty()) {
            Process p = q2.front();
            q2.pop_front();
            currTime = max(currTime, p->getStartTime());

            if (i < processes.size() && processes[i].start_time < time + std::min(p.remaining_time, sl2[p.pid])) {
                q1.push_back(processes[i++]);
            }

            if (q1.empty()) {
                if (time + std::min(sl2[p.pid], p.remaining_time) > boost) {
                    schedule[p.pid].push_back({time, boost});
                    q2.push_front({p.pid, boost, time + p.remaining_time - boost, 0});
                    q1.insert(q1.end(), q2.begin(), q2.end());
                    q2.clear();
                    q1.insert(q1.end(), q3.begin(), q3.end());
                    q3.clear();
                    sl2.clear();
                    time = boost;
                    boost += boost;
                } else {
                    schedule[p.pid].push_back({time, time + std::min(sl2[p.pid], p.remaining_time)});
                    if (p.remaining_time > sl2[p.pid]) {
                        q3.push_back({p.pid, time, p.remaining_time - sl2[p.pid], 0});
                    }
                    sl2.erase(p.pid);
                }
            } else {
                Process t = q1.front();
                if (boost < t.start_time) {
                    schedule[p.pid].push_back({time, boost});
                    q2.push_front({p.pid, boost, time + p.remaining_time - boost, 0});
                    q2.insert(q2.end(), q3.begin(), q3.end());
                    q3.clear();
                    q2.insert(q2.end(), q1.begin(), q1.end());
                    q1.clear();
                    q1.insert(q1.end(), q2.begin(), q2.end());
                    q2.clear();
                    sl2.clear();
                    time = boost;
                    boost += boost;
                } else {
                    schedule[p.pid].push_back({time, t.start_time});
                    q2.push_front({p.pid, t.start_time, time + p.remaining_time - t.start_time, 0});
                    sl2[p.pid] -= t.start_time - time;
                }
            }
        }

        while (!q3.empty() && q1.empty() && q2.empty()) {
            Process p = q3.front();
            q3.pop_front();
            if (sl3.find(p.pid) == sl3.end()) {
                sl3[p.pid] = slice3;
            }
            time = std::max(time, p.start_time);

            if (i < processes.size() && processes[i].start_time <= time + std::min(sl3[p.pid], p.remaining_time)) {
                q1.push_back(processes[i++]);
            }

            if (q1.empty()) {
                if (time + std::min(sl3[p.pid], p.remaining_time) > boost) {
                    schedule[p.pid].push_back({time, boost});
                    q3.push_front({p.pid, boost, time + p.remaining_time - boost, 0});
                    q1.insert(q1.end(), q2.begin(), q2.end());
                    q2.clear();
                    q1.insert(q1.end(), q3.begin(), q3.end());
                    q3.clear();
                    sl3.clear();
                    time = boost;
                    boost += boost;
                } else {
                    schedule[p.pid].push_back({time, time + std::min(sl3[p.pid], p.remaining_time)});
                    if (p.remaining_time > sl3[p.pid]) {
                        q3.push_back({p.pid, time, p.remaining_time - sl3[p.pid], 0});
                    }
                    sl3.erase(p.pid);
                }
            } else {
                Process t = q1.front();
                if (boost < t.start_time) {
                    schedule[p.pid].push_back({time, boost});
                    q3.push_front({p.pid, boost, time + p.remaining_time - boost, 0});
                    q2.insert(q2.end(), q3.begin(), q3.end());
                    q3.clear();
                    q2.insert(q2.end(), q1.begin(), q1.end());
                    q1.clear();
                    q1.insert(q1.end(), q2.begin(), q2.end());
                    q2.clear();
                    sl3.clear();
                    time = boost;
                    boost += boost;
                } else {
                    schedule[p.pid].push_back({time, t.start_time});
                    q3.push_front({p.pid, t.start_time, time + p.remaining_time - t.start_time, 0});
                    sl3[p.pid] += time - t.start_time;
                }
            }
        }
    }
    return schedule;
}

int main(){
    // int n = argv[1]  // number of processes
    // int lamda = argv[2];  // inter-arrival time parameter
    vector<Process* > listProcess;   // list to store all the processes

    Process* process1 = new Process("Job1", 2, 6);
    Process* process2 = new Process("Job2", 5, 2);
    Process* process3 = new Process("Job3", 1, 8);
    Process* process4 = new Process("Job4", 0, 3);
    Process* process5 = new Process("Job5", 4, 4);


    listProcess.push_back(process1);
    listProcess.push_back(process2);
    listProcess.push_back(process3);
    listProcess.push_back(process4);
    listProcess.push_back(process5);

    auto ans = sjf(listProcess);
    for(auto it:ans){
        cout<<it.first<<" "<<it.second.first<<" "<<it.second.second<<endl;
    }
    return 0;
}
