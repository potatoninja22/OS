import matplotlib.pyplot as plt

# Load data from the input file
with open('gantt_data.txt', 'r') as f:
    lines = f.readlines()

# Create a dictionary to map scheduling process names to colors
scheduling_colors = {
    'First Come First Served': 'blue',
    'Round Robin': 'green',
    'Shortest Job First': 'red',
    'Shortest Remaining Time First': 'purple',
    'Multilevel Feedback Queue': 'orange'
}

# Initialize variables
current_scheduling = None
process_data = []

# Iterate through the lines
for line in lines:
    parts = line.strip().split()
    
    # Check if line represents a scheduling process
    if len(parts) == 1:
        current_scheduling = ' '.join(parts)
    elif len(parts) == 3:
        process_data.append([current_scheduling] + parts)

# Create a figure and axis for the Gantt chart
fig, ax = plt.subplots(figsize=(12, 6))

# Create a dictionary to store the start times for each process
start_times = {}

# Plot Gantt charts for each scheduling process
for data in process_data:
    scheduling_name = data[0]
    df = []
    for i in range(1, len(data), 3):
        process_name, start, end = data[i:i+3]
        df.append((process_name, float(start), float(end)))

    df.sort(key=lambda x: x[1])  # Sort processes by start time

    # Initialize arrays to store process names and their corresponding start times
    process_names = []
    process_start = []

    for process_name, start, end in df:
        process_names.append(process_name)
        process_start.append(start)
        if process_name not in start_times:
            start_times[process_name] = []

    # Plot horizontal bars for each process
    for i, process_name in enumerate(process_names):
        color = scheduling_colors.get(scheduling_name, 'gray')
        ax.barh(process_name, end - start_times[process_name][-1] if start_times[process_name] else 0, 
                left=start_times[process_name][-1] if start_times[process_name] else start, color=color)
        start_times[process_name].append(end)
    
    # Display average turnaround time and average response time if available
    if len(data) > 2:
        avg_turnaround, avg_response = data[-2], data[-1]
        ax.text(0.02, i + 0.5, f'Avg. Turnaround: {avg_turnaround}\nAvg. Response: {avg_response}',
                va='center', fontsize=8, bbox=dict(boxstyle='round,pad=0.3', edgecolor='black', facecolor='lightgray'))

# Configure the plot
ax.set_xlabel('Time')
ax.set_ylabel('Processes')
ax.set_title('Comparison of Scheduling Algorithms')
plt.grid(axis='x')

# Create custom legend for scheduling algorithms
legend_labels = [plt.Rectangle((0, 0), 1, 1, color=color) for color in scheduling_colors.values()]
legend_names = scheduling_colors.keys()
ax.legend(legend_labels, legend_names)

plt.tight_layout()
plt.show()