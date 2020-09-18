/*
 NAME: Jack Li
 EMAIL: jackli2014@gmail.com
 ID: 604754714
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

long long counter = 0;  // the shared resource

/* Flags and protections */
int opt_yield = 0;
int lock = 0;
int compare_and_swap = 0;
int mutex_sync = 0;
int spin = 0;
pthread_mutex_t mutex;


void CASadd(long long *p, long long val)
{
    long long orig;
    do {
        orig = *p;
        if (opt_yield)
            sched_yield();
    } while (__sync_val_compare_and_swap(p, orig, orig + val) != orig);
}

void add(long long *pointer, long long value)
{
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

/* No synchronization */
void *updateCounter(void *iterations)
{
    long long i, niterations = (long long)iterations;
    for (i = 0; i < niterations; i++) {
        if (compare_and_swap) {
            /*
            while (__sync_val_compare_and_swap(&counter, 0, 1)){
                if (opt_yield)
                    sched_yield();
            }
            add(&counter, -1);
            */
            CASadd(&counter, 1);
            CASadd(&counter, -1);
        }
        else if (mutex_sync){
            pthread_mutex_lock(&mutex);
            add(&counter, 1);
            add(&counter, -1);
            pthread_mutex_unlock(&mutex);
        }
        else if (spin){
            while (__sync_lock_test_and_set(&lock, 1))
                ; // spin-wait
            add(&counter, 1);
            add(&counter, -1);
            __sync_lock_release(&lock);
        }
        else{
            add(&counter, 1);
            add(&counter, -1);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    /* Defaults */
    long long nthreads = 1, iterations = 1;
    char *testName = "add-none";
    
    long long operations, runtime;
    /* Parse command line arguments */
    static struct option long_options[] = {
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"threads", required_argument, NULL, 't'},
        {"yield", no_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };
    
    int option;
    while ((option = getopt_long(argc, argv, "i:s:t:y", long_options, NULL)) != -1) {
        switch (option) {
            case 'i':
                iterations = atoll(optarg);
                break;
            case 's':
                switch (optarg[0]) {
                    case 'c':
                        compare_and_swap = 1;
                        testName = "add-c";
                        break;
                    case 'm':
                        mutex_sync = 1;
                        testName = "add-m";
                        break;
                    case 's':
                        spin = 1;
                        testName = "add-s";
                        break;
                    default:
                        fprintf(stderr, "Usage: lab2_add [--iterations=iterations] [--sync=c, m, s] [--threads=threads]\n");
                        exit(1);
                }
                break;
            case 't':
                nthreads = atoll(optarg);
                break;
            case 'y':
                opt_yield = 1;
                break;
            default:
                fprintf(stderr, "Usage: lab2_add [--iterations=iterations] [--sync=c, m, s] [--threads=threads]\n");
                exit(1);
        }
    }
    
    if (iterations <= 0 || nthreads <= 0){
        fprintf(stderr, "Cannot have nonpositive number of iterations and/or threads.\n");
        exit(1);
    }
    if (opt_yield) {
        if (compare_and_swap)
            testName = "add-yield-c";
        else if (mutex_sync)
            testName = "add-yield-m";
        else if (spin)
            testName = "add-yield-s";
        else
            testName = "add-yield-none";
    }
    if (mutex_sync)
        pthread_mutex_init(&mutex, NULL);
    operations = nthreads * iterations * 2;
    
    
    /* Note high resolution starting time for the run */
    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        fprintf(stderr, "Failed to get start time.\n");
        exit(1);
    }
    
    
    /* Start specified number of threads to call add function */
    pthread_t threads[nthreads];
    long long i;
    for (i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, updateCounter, (void *)iterations)) {
            fprintf(stderr, "Failed to create thread.\n");
            exit(1);
        }
    }
    
    
    /* Join the threads */
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Failed to join thread.\n");
            exit(1);
        }
    }
    
    
    /* Note high resolution ending time for the run */
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        fprintf(stderr, "Failed to get end time.\n");
        exit(1);
    }
    
    
    /* Print comma-separated-value recording */
    runtime = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("%s,%lld,%lld,%lld,%lld,%lld,%lld\n", testName, nthreads, iterations, operations, runtime, runtime/operations, counter);
    
    
    if (mutex_sync)
        pthread_mutex_destroy(&mutex);
    exit(0);
}
