# Sleeping TA Problem

A concurrent C program that simulates a teaching assistant helping students while managing a limited number of waiting chairs. The project demonstrates thread synchronization and coordination using POSIX threads, semaphores, mutexes, and a circular queue.

Developed as part of COMPSCI 3SH3 (Operating Systems) at McMaster University under Dr. Neerja Mhaskar, in collaboration with Thaneesha Sivasithambaram.

## Overview

The program models a classic concurrency problem involving one teaching assistant and multiple students.

* One **TA thread** provides help to students.
* Five **student threads** alternate between programming and seeking help.
* Three waiting chairs are available for students waiting to meet with the TA.
* Students are served in **first-come, first-served order**.
* If all chairs are occupied, a student leaves and returns later.
* Random delays simulate programming and help times, creating concurrent interactions between threads.

## Operating Systems Concepts

This project demonstrates:

* **Multithreading:** Creates and coordinates multiple POSIX threads.
* **Semaphores:** Coordinates communication between students and the TA.
* **Mutex Locks:** Protects shared resources from concurrent access.
* **Thread Synchronization:** Coordinates access to the waiting area and TA.
* **Critical Sections:** Protects modifications to shared queue and chair data.
* **Circular Queues:** Maintains the waiting students in first-come, first-served order.
* **Concurrency:** Models multiple students independently performing tasks and requesting assistance.

## Implementation

Students entering the waiting area are stored in a circular queue along with their randomly generated help time. A mutex protects modifications to the queue and the number of occupied chairs.

Two semaphores coordinate the interaction between students and the TA:

* `sem_student` signals that a student is waiting for help.
* `sem_ta` signals that the TA is ready to help a student.

When no students are waiting, the TA blocks on the student semaphore until another student arrives.

## Compilation and Execution

Compile using GCC:

```bash
gcc sleeping_ta.c -o sleeping-ta -pthread
```

Run:

```bash
./sleeping-ta
```

The simulation runs continuously and can be stopped with `Ctrl+C`.

## Example Output

```text
Student 0 is programming.
Student 1 is programming.
Student 2 is programming.
Student 0 is seeking help.
Student 0 occupies a chair.
TA is helping student 0.
Student 3 is seeking help.
Student 3 occupies a chair.
Student 4 is seeking help.
Student 4 occupies a chair.
TA is done helping student 0.
TA is helping student 3.
```

## Technologies

* **Language:** C
* **Concurrency:** POSIX Threads
* **Synchronization:** Semaphores and mutexes
* **Data Structures:** Circular queue
* **Environment:** POSIX/Linux
