#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <sstream>
#include "helper.h"
using namespace std;

/**
 * FCFS.cpp
 * 
 * An implementation of the FCFS (First come first serve) algorithm.
 * Processes are loaded into a process_list queue and will be put into the
 * ready_queue when its arrival_time is reached.
 * 
 * Result will then be stored in FCFS.csv.
 * 
 * Variables:
 * - process_list: list of processes
 * - ready_queue: queue of ready to execute processes
 * - cpu_time: stimulated CPU time (clock)
 * - no_processes: number of processes
 * 
 */
int main() {
    string line;
    ifstream file ("processes");
    vector<Process> process_list;
    queue<Process*> ready_queue;
    double cpu_time     = 0;
    int    no_processes = 0;

    /* 
     * Create processes class from files and store it inside
     * a queue.
     */    
    if (file.is_open()) {
        while (getline(file, line)) {
            double pid, btime, atime = 0;
            stringstream(line) >> pid >> btime >> atime;
            process_list.push_back(Process(pid, btime, atime));
            no_processes++;
        }
    }
    file.close();
    /* */
    vector<Process>::iterator next_process = process_list.begin();

    ofstream fcfs_result;
    fcfs_result.open("FCFS.csv");
    fcfs_result << "process_id\twaiting_time\tturn_around" << endl;

    while (no_processes > 0) {

        if ((next_process != process_list.end()) && 
            (next_process->arrival_time <= cpu_time)) {
                ready_queue.push(&(*next_process));
                ++next_process;
        }

        Process* process = ready_queue.front();

        if (ready_queue.size() > 0) {
            Helper::update_wait_time(&process_list, process, cpu_time);
            
            process->run();

            if (process->is_complete()) {
                ready_queue.pop();

                double turn_around = process->wait_time + process->burst_time; 
                fcfs_result << process->process_id << '\t' 
                            << process->wait_time << '\t' 
                            << turn_around << endl;

                no_processes--;
            }

        }

        cpu_time++;
    }


    fcfs_result.close();
    cout << "[Complete] Please check FCFS.csv for result." << endl;

    return 0;
}
