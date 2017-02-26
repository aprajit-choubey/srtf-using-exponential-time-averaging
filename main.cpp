/*----------------Standard Header file-----------------*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

#define BURST_LEN 1

using namespace std;

/*-----------Defining Process Structure----------------*/
struct Process
{
    int id;
    int completed;
    int cpu_bursts[BURST_LEN];                 //Assuming 3 cpu bursts for each process.
    int arrival_time;
    int waiting_start;
    int waiting_time;
    int processor_time;
    int total_processor_time;
    int start_time;
    int completion_time;
    int last_burst;
};

/*-------------Initializing a new process-------------*/
int init_process(Process& proc, const int& i)
{
    proc.id = i;
    proc.start_time = 0;
    proc.completion_time = 0;
    proc.waiting_start = 0;
    proc.waiting_time = 0;
    proc.last_burst = 0;
    proc.processor_time = 0;
    proc.completed = 0;
    proc.total_processor_time = 0;

    proc.arrival_time = rand() % 50;
    proc.waiting_start = proc.arrival_time;

    for(int i = 0; i < BURST_LEN; i++)
    {
        proc.cpu_bursts[i] = (rand() % 2) + 2;
        proc.total_processor_time += proc.cpu_bursts[i];
    }

    return 0;
}

/*----------------Displaying Process Stat----------------*/
void display_proc(Process& proc)
{
    cout << proc.arrival_time << " " << proc.waiting_time << " " << proc.processor_time << " "
         << proc.cpu_bursts[proc.last_burst] << " " << proc.total_processor_time << "\n";
}

/*----------------Checking if all process done-----------*/
int all_proc_done(vector<Process>& proc_q, int& N)
{
    int done = 1;

    for(int i = 0; i < N; i++)
    {
        if(proc_q[i].completed == 0)
        {
            done = 0;
            break;
        }
    }

    return done;
}

/*------------------Writing Output------------------------*/
void write_process_stats(vector<Process>& proc_q, float alpha, int time_elapsed, int N)
{
    float avg_comp = 0.0;
    float avg_wait = 0.0;
    float avg_tround = 0.0;

    for(int i = 0; i < N; i++)
    {
        avg_comp += (proc_q[i].completion_time - proc_q[i].arrival_time);
        avg_wait += proc_q[i].waiting_time;
        avg_tround += (proc_q[i].completion_time - proc_q[i].arrival_time) / (proc_q[i].completion_time - proc_q[i].start_time);
    }

    ofstream out;

    out.open("data100.txt", ios_base::app);

    out << alpha << " " << (avg_comp / N) << " " << (avg_wait / N) << " " << (avg_tround / N) << "\n";

    out.close();
}

/*------------------Main Function-------------------------*/
int main(int argc, char** argv)
{
    int N;
    int i;

    cout << "Enter the number of processes\n";
    cin >> N;

    vector<Process> proc_q(N);

    float alpha = 0.0;

    for(alpha = 0.0; alpha <= 1.0; alpha += 0.05)
    {
        srand(333);                         //Same seeding for every init.

        for(i = 0; i < N; i++)
        {
            init_process(proc_q[i], N);
        }

        int time_elapsed = 0;
        float tau_n, t_n;
        int wait_id = -1;

        while(!all_proc_done(proc_q, N))
        {
            if(time_elapsed == 0)
                tau_n = 10;
            else
                tau_n = (alpha * t_n) + (1 - alpha) * (tau_n);

            int shortest_proc_id = -1;
            int time_temp = 9999;
            int temp;

            //This part helped by Ameya Godbole, 140102078
            for(i = 0; i < N; i++)
            {
                if(i == wait_id)
                    continue;
                else if(proc_q[i].arrival_time > time_elapsed)
                    continue;
                else if(proc_q[i].completed == 1)
                    continue;

                temp = proc_q[i].total_processor_time - proc_q[i].processor_time;

                if(temp < time_temp)
                {
                    shortest_proc_id = i;
                    time_temp = temp;
                }
            }

            Process& new_srt = proc_q[shortest_proc_id];

            if(new_srt.processor_time == 0)
                new_srt.start_time = time_elapsed;

            new_srt.waiting_time += (time_elapsed - new_srt.waiting_start);

            if(new_srt.cpu_bursts[new_srt.last_burst] <= ((int)tau_n))
            {
                t_n = new_srt.cpu_bursts[new_srt.last_burst];

                if(t_n < 0)
                    cout << "FAIL\n";

                time_elapsed += t_n;
                wait_id = shortest_proc_id;

                new_srt.processor_time += new_srt.cpu_bursts[new_srt.last_burst];
                new_srt.waiting_start = time_elapsed;

                new_srt.last_burst++;
            }
            else
            {
                t_n = tau_n;
                time_elapsed += (int)t_n;

                wait_id = -1;

                new_srt.processor_time += (int)t_n;
                new_srt.waiting_start = time_elapsed;

            }

            if(new_srt.processor_time >= new_srt.total_processor_time)
            {
                new_srt.completion_time = time_elapsed;
                new_srt.completed = 1;
            }
        }

        write_process_stats(proc_q, alpha, time_elapsed, N);
        cout << alpha << "\n";
    }

    return 0;
}
