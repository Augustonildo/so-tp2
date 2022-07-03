#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

enum types {
    CPU_BOUND,
    S_BOUND,
    IO_BOUND,
};
static const char *types_names[] = {"CPU_BOUND", "S_BOUND","IO_BOUND"};
int PROCESS_TYPES = 3;
int MULTIPLIER = 3;

void cpuBoundProcess()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 1000000; j++)
        {
            asm("nop");
        }
    }
}

void sBoundProcess()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 1000000; j++)
        {
            asm("nop");
        }
        yield();
    }
}

void ioBoundProcess()
{
    for (int i = 0; i < 100; i++)
    {
        sleep(1);
    }
}

int main(int argc, char* argv[]) 
{
    int number = atoi(argv[1]);
    if (number < 0) exit();

    int processes[PROCESS_TYPES], retimeByTypes[PROCESS_TYPES], rutimeByTypes[PROCESS_TYPES], stimeByTypes[PROCESS_TYPES];
    // initializing empty arrays
    for (int i = 0; i < PROCESS_TYPES; i++)
    {
        processes[i] = 0;
        retimeByTypes[i] = 0;
        rutimeByTypes[i] = 0;
        stimeByTypes[i] = 0;
    }

    for (int i = 0; i < MULTIPLIER * number; i++)
    {
        int pid = fork();
        if (pid==0)
        {
            enum types type = getpid() % 3;
            switch (type)
            {
                case CPU_BOUND:
                    cpuBoundProcess();
                    break;
                case S_BOUND:
                    sBoundProcess();
                    break;
                case IO_BOUND:
                    ioBoundProcess();
                    break;
                default:
                    break;
            }
            exit();
        } else
        {
            enum types type = pid%3;
            processes[type]++;
        }
    }

    for (int i = 0; i < MULTIPLIER * number; i++)
    {
        int retime, rutime, stime;
        int pid = wait2(&retime, &rutime, &stime);
        enum types type = pid % 3;
        printf(1, "\nPid: %d\n Tipo=%s\n Tempo de espera=%d\n Tempo executando=%d\n Tempo de IO=%d\n", pid, types_names[type], retime, rutime, stime);
        retimeByTypes[type] += retime;
        stimeByTypes[type] += stime;
        rutimeByTypes[type] += retime + rutime + stime;
    }

    for (int j = 0; j < PROCESS_TYPES; j++)
    {
        int averageReadyTime[PROCESS_TYPES], averageSleepingTime[PROCESS_TYPES], averageTurnaroundTime[PROCESS_TYPES];
        averageReadyTime[j] = retimeByTypes[j]/number;
        averageSleepingTime[j] = stimeByTypes[j]/number;
        averageTurnaroundTime[j] = rutimeByTypes[j]/number;
        printf(1, "\nTipo: %s\n Número de processos: %d\n Tempo médio SLEEPING: %d\n Tempo médio READY: %d\n Tempo médio de turnaround: %d", types_names[j], processes[j], averageSleepingTime[j], averageReadyTime[j], averageTurnaroundTime[j]);
        printf(1, "\n\n");
    }
    exit();
}