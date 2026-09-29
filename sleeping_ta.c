#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CHAIRS 3 // maximum number of students in lobby
#define N 5          // total number of students

sem_t sem_ta;           // semaphore to signal ta's availibility
sem_t sem_student;      // semaphore to signal that is student is waiting for help
pthread_mutex_t chairs; // mutex lock to signal modification of chairs_occupied

int chairs_occupied = 0;

// use queue to ensure first come first serve for students in lobby
int student_id_queue[MAX_CHAIRS];
int student_help_time_queue[MAX_CHAIRS];
int head = 0;
int tail = 0;

// enqueue student id and corresponding help time circularly
void enqueue(int id, int help_time)
{
    student_id_queue[tail] = id;
    student_help_time_queue[tail] = help_time;
    tail = (tail + 1) % MAX_CHAIRS;
}

// dequeue student id and corresponding help time circularly
void dequeue(int *id, int *help_time)
{
    *id = student_id_queue[head];
    *help_time = student_help_time_queue[head];
    head = (head + 1) % MAX_CHAIRS;
}

void *student_runner(void *param)
{
    int student_id = *(int *)param;

    while (true)
    {
        // student starts off by programming
        printf("Student %d is programming.\n", student_id);
        sleep(rand() % 10 + 1);
        printf("Student %d is seeking help.\n", student_id);

        // aquire lock to reserve a chair
        pthread_mutex_lock(&chairs);

        if (chairs_occupied < MAX_CHAIRS)
        {
            int help_time = rand() % 5 + 1; // randomize student's help time

            // student occupies chair
            enqueue(student_id, help_time);
            chairs_occupied++;
            printf("Student %d occupies a chair.\n", student_id);

            // release lock upon exiting critical section
            pthread_mutex_unlock(&chairs);

            sem_post(&sem_student); // notify TA that a student is ready
            sem_wait(&sem_ta);      // wait until TA is ready

            // printf("Student %d is receiving help from TA.\n", student_id);
            sleep(help_time);
            // printf("Student %d has received help.\n", student_id);
        }
        else
        {
            // since all chairs are occupied, we aren't modifying chairs_occupied
            // mutex is no longer needed
            pthread_mutex_unlock(&chairs);
            printf("There are no free chairs, so student %d will come back later.\n", student_id);
        }
    }
}

void *ta_runner(void *param)
{
    while (true)
    {
        int student_id, help_time;

        // TA is 'sleeping' until a student wakes them up, signified by signalling student semaphore
        sem_wait(&sem_student);

        // student is leaving chair (entering office) so we are modifying shared data
        pthread_mutex_lock(&chairs);
        dequeue(&student_id, &help_time);
        chairs_occupied--;

        // release lock upon exiting critical section
        pthread_mutex_unlock(&chairs);

        // TA signals that they're ready
        sem_post(&sem_ta);

        printf("TA is helping student %d.\n", student_id);
        sleep(help_time);
        printf("TA is done helping student %d.\n", student_id);
    }
}

int main()
{
    srand(time(NULL));

    pthread_t ta_thread;
    pthread_t student_threads[N];

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int student_ids[N];

    if (sem_init(&sem_student, 0, 0) != 0 || sem_init(&sem_ta, 0, 0) != 0)
    {
        printf("Error in creating semaphore(s).\n");
        return 1;
    }

    if (pthread_mutex_init(&chairs, NULL) != 0)
    {
        printf("Error in initializing mutex.\n");
        return 1;
    }

    if (pthread_create(&ta_thread, &attr, ta_runner, NULL) != 0)
    {
        printf("Error in initializing TA thread.\n");
        return 1;
    }

    for (int i = 0; i < N; i++)
    {
        student_ids[i] = i;

        if (pthread_create(&student_threads[i], &attr, student_runner, &student_ids[i]) != 0)
        {
            printf("Error initializing student thread.\n");
            return 1;
        }
    }

    pthread_join(ta_thread, NULL);

    for (int i = 0; i < N; i++)
        pthread_join(student_threads[i], NULL);

    pthread_mutex_destroy(&chairs);
    sem_destroy(&sem_ta);
    sem_destroy(&sem_student);

    return 0;
}