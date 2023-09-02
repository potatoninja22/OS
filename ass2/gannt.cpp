#include <iostream>
#include <matplotlibcpp.h>

namespace plt = matplotlibcpp;

int main() {
    // Sample data
    std::vector<std::string> labels = {"P1", "P2", "P3"};
    std::vector<double> start_times = {0, 3, 7};
    std::vector<double> durations = {3, 4, 5};

    // Create Gantt chart
    plt::figure_size(10, 6);
    plt::barh(labels, durations, 0.5, start_times, true);
    plt::xlabel("Time");
    plt::ylabel("Processes");
    plt::title("Gantt Chart");
    plt::show();

    return 0;
}

#include <iostream>
#include <vector>
#include <list>
#include <map>

struct Process {
    int pid;
    int burst_time;
    int remaining_time;
    int queue_level;
    int start_time;
};

struct CPU {
    int start_time;
    int end_time;
};

std::map<int, std::vector<CPU>> scheduleMLFQ(int slice1, int slice2, int slice3, int boost, std::vector<Process>& processes) {
    std::map<int, std::vector<CPU>> schedule;
    std::map<int, int> sl2;
    std::map<int, int> sl3;
    std::list<Process> q1, q2, q3;
    int i = 0, time = 0;

    while (i < processes.size() || !q1.empty() || !q2.empty() || !q3.empty()) {
        if (q1.empty() && q2.empty() && q3.empty()) {
            time = std::max(time, processes[i].start_time);
            q1.push_back(processes[i++]);
        }
        if (time >= boost) {
            q1.insert(q1.end(), q2.begin(), q2.end());
            q2.clear();
            q1.insert(q1.end(), q3.begin(), q3.end());
            q3.clear();
            boost += boost;
        }
        while (!q1.empty()) {
            Process p = q1.front();
            q1.pop_front();
            time = std::max(time, p.start_time);

            for (; i < processes.size() && processes[i].start_time <= time + slice1; i++) {
                q1.push_back(processes[i]);
            }

            if (schedule.find(p.pid) == schedule.end()) {
                schedule[p.pid] = std::vector<CPU>();
            }

            if (time + std::min(slice1, p.remaining_time) > boost) {
                schedule[p.pid].push_back({time, boost});
                q1.push_front({p.pid, boost, time + p.remaining_time - boost, 0});
                q1.insert(q1.end(), q2.begin(), q2.end());
                q2.clear();
                q1.insert(q1.end(), q3.begin(), q3.end());
                q3.clear();
                time = boost;
                boost += boost;
            } else {
                schedule[p.pid].push_back({time, time + std::min(slice1, p.remaining_time)});
                if (p.remaining_time > slice1) {
                    q2.push_back({p.pid, time, p.remaining_time - slice1, 0});
                }
            }
        }

        while (!q2.empty() && q1.empty()) {
            Process p = q2.front();
            q2.pop_front();
            if (sl2.find(p.pid) == sl2.end()) {
                sl2[p.pid] = slice2;
            }
            time = std::max(time, p.start_time);

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

int main() {
    // Define and initialize processes
    std::vector<Process> processes = {
        {0, 6, 6, 0, 0},
        {1, 8, 8, 0, 0},
        {2, 10, 10, 0, 0},
        {3, 4, 4, 0, 0},
    };

    // Call the MLFQ scheduler
    std::map<int, std::vector<CPU>> result = scheduleMLFQ(2, 4, 8, 10, processes);

    // Print the scheduling results
    for (const auto& entry : result) {
        std::cout << "Process " << entry.first << ":\n";
        for (const CPU& cpu : entry.second) {
            std::cout << "Start Time: " << cpu.start_time << " End Time: " << cpu.end_time << "\n";
        }
    }

    return 0;
}

