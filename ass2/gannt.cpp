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
