/*
Program Name: CPU Scheduling Algorithms Program
Program Purpose: The program will automatically simulate multiple CPU scheduling algorithms for any given set of processes.
The program will maintain wait and turnaround times for each process over time and then display the average wait and turnaround time for all processes
Author: Laurence Lu 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 20

// Struct called process which stores information about each process
struct process { 
    char id[3];
    int arrival_time;
    int burst_time;
    int wait_time;
    int turnaround_time;
    int remaining_time;
    int finished;
};

// First Come First Served Function
void fcfs(struct process *procs, int n) {
    printf("First Come First Served\n");
    int t = 0; // Initial time

    for (int i = 0; i < n; i++) {
        while (t < procs[i].burst_time) { // Loop until current time "t" is greater than or equal to the burst time of the current process procs[i]
            t++;
        }
        printf("T%d : %s  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", i, procs[i].id, procs[i].burst_time, t - procs[i].burst_time, t);
        procs[i].wait_time = t - procs[i].burst_time;
        procs[i].turnaround_time = t;
    }
    // Print wait and turnaround times for each process
    for (int i = 0; i < n; i++) {
        printf("%s\n", procs[i].id);
        printf("\tWaiting time:         %d\n", procs[i].wait_time);
        printf("\tTurnaround time:      %d\n", procs[i].turnaround_time);
    }
}

// Shortest Job First
void sjf(struct process *procs, int n) {
    printf("Shortest Job First\n");
    int t = 0, completed = 0;

    while (completed < n) { // Loop continues until all process have completed execution
        int shortest = -1;
        for (int i = 0; i < n; i++) { // Loop through all processes
            if (procs[i].burst_time > 0 && (shortest == -1 || procs[i].burst_time < procs[shortest].burst_time)) {
                shortest = i;
            }
        }
        if (shortest == -1) {
            // No process is ready, so wait, and then increment current time "t"
            t++;
        } else {
            // Execute the selected process
            procs[shortest].burst_time--;
            if (procs[shortest].burst_time == 0) {
                // Process has completed, update completion, wait, and turnaround time
                completed++;
                procs[shortest].turnaround_time = t + 1;
                procs[shortest].wait_time = procs[shortest].turnaround_time - procs[shortest].burst_time;
            }
            printf("T%d : %s  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", t, procs[shortest].id, procs[shortest].burst_time, procs[shortest].turnaround_time - procs[shortest].burst_time, procs[shortest].turnaround_time);
            t++;
            completed++;
        }
    }
    // Print wait and turnaround times for each process
    for (int i = 0; i < n; i++) {
        printf("%s\n", procs[i].id);
        printf("\tWaiting time:          %d\n", procs[i].wait_time);
        printf("\tTurnaround time:       %d\n", procs[i].turnaround_time);
    }
}

// Round Robin
void rr(struct process *procs, int n, int quantum) {
    printf("\nRound Robin with Quantum %d\n", quantum);
    int current_time = 0, completed = 0, t = 0;

    // Loop continues until all process have completed execution
    while (completed < n) {
        // Loop through all processes
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival_time <= current_time && !procs[i].finished) {
                // Run for time quantum or until process finishes
                for (int j = 0; j < quantum && procs[i].remaining_time > 0; j++) {
                    current_time++;
                    procs[i].remaining_time--;
                }

                // If process has finished, set its finished flag and calculate waiting and turnaround time
                if (procs[i].remaining_time == 0) {
                    procs[i].finished = 1;
                    completed++;

                    procs[i].turnaround_time = current_time - procs[i].arrival_time;
                    procs[i].wait_time = procs[i].turnaround_time - procs[i].burst_time;
                }
            }
            printf("T%d : %s  - Burst left  %d, Wait time  %d, Turnaround time  %d\n", i, procs[i].id, procs[i].burst_time, t - procs[i].burst_time, t);
        }
    }
    // Print wait and turnaround times for each process
    for (int i = 0; i < n; i++) {
        printf("%s\n", procs[i].id);
        printf("\tWaiting time:          %d\n", procs[i].wait_time);
        printf("\tTurnaround time:       %d\n", procs[i].turnaround_time);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // check for correct command line parameters 
        printf("Proper usage is ./assignment-4 [-f|-s|-r <quantum>] <Input file name>\n");
        return 1;
    }

    char *filename = argv[2];
    char *algorithm = argv[1];
    int quantum = 0;

    if (argc == 4) {
        if (strcmp(algorithm, "-r") != 0) { // if algorithem chosen is not round robin, error
            printf("Proper usage is ./assignment-4 [-f|-s|-r <quantum>] <Input file name>\n");
            return 1;
        }
        quantum = atoi(argv[2]);
        filename = argv[3];
    }

    // check file
    FILE *file = fopen(filename, "r");
    if (file == NULL) { 
        printf("Could not open file %s\n", filename);
        return 1;
    }

    // read process information from given file
    char line[100];
    int n = 0;
    struct process procs[MAX_PROCESSES]; // store process information in struct in the procs array
    while (fgets(line, 100, file) != NULL && n < MAX_PROCESSES) {
        sscanf(line, "%s\t%d", procs[n].id, &procs[n].burst_time);
        n++;
    }
    fclose(file);

    /* 
    / Choose the algorithm and run it
    / -f for First Come First Served, calls fcfs function
    / -s for Shortest Job First, calls sjf function
    / -r <integer> for Round Robin, calls rr function
    */
    if (strcmp(algorithm, "-f") == 0) {
        fcfs(procs, n);
    } else if (strcmp(algorithm, "-s") == 0) {
        sjf(procs, n);
    } else if (strcmp(algorithm, "-r") == 0) {
        rr(procs, n, quantum);
    } else {
        printf("Invalid algorithm\n");
        return 1;
    }

    // Calculate and display average wait and turnaround times
    int total_wait_time = 0, total_turnaround_time = 0;
    for (int i = 0; i < n; i++) {
        total_wait_time += procs[i].wait_time;
        total_turnaround_time += procs[i].turnaround_time;
    }
    double avg_wait_time = (double)total_wait_time / n;
    double avg_turnaround_time = (double)total_turnaround_time / n;
    printf("Total average waiting time:     %.1f\n", avg_wait_time);
    printf("Total average turnaround time:  %.1f\n", avg_turnaround_time);
    return 0;
}

