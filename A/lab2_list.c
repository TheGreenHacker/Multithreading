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
#include <signal.h>
#include "SortedList.h"

/* Globals */
SortedList_t *head;
SortedListElement_t *elements;
int keylen = 21;
int opt_yield = 0;
int lock = 0;
int mutex_sync = 0;
int spin = 0;
pthread_mutex_t mutex;

/* Defaults */
long long nthreads = 1, iterations = 1;
char *testName = "list-none-none";

/* Random string generator of length 21 (including null byte) */
void rand_str(char *str) {
    char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int j = keylen - 1;
    
    while (j > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *str = charset[index];
        str++;
        j--;
    }
    *str = '\0';
}

/* Domain decomposition */
struct Domain {
    long long from, to;
};

/* Catch segmentation faults */
void signal_handler(int sig)
{
    if (sig == SIGSEGV) {
        fprintf(stderr, "Caught segmentation fault.\n");
        exit(2);
    }
}

void *listTasks(void *domain)
{
    struct Domain* t_domain = (struct Domain*) domain;
    long long i, length, from = t_domain->from, to = t_domain->to;
    
    /* All list operations can be treated as a critical section */
    if (mutex_sync)
        pthread_mutex_lock(&mutex);
    else if (spin){
        while (__sync_lock_test_and_set(&lock, 1))
            ; // spin-wait
    }
    
    for (i = from; i < to; i++)
        SortedList_insert(head, &elements[i]);
    
    length = (long long) SortedList_length(head);
    if (length < iterations){
        fprintf(stderr, "Did not insert enough items.\n");
        exit(2);
    }

    for (i = from; i < to; i++) {
        SortedListElement_t *find = SortedList_lookup(head, elements[i].key);
        if (find == head) {
            fprintf(stderr, "Cannot find inserted key.\n");
            exit(2);
        }
        SortedList_delete(find);
    }
    
    if (mutex_sync)
        pthread_mutex_unlock(&mutex);
    else if (spin)
        __sync_lock_release(&lock);
    
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    /* Initialize an empty list */
    head = malloc(sizeof(SortedList_t));
    head->key = NULL;
    head->prev = head;
    head->next = head;
        
    long long operations, runtime;
    /* Parse command line arguments */
    static struct option long_options[] = {
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"threads", required_argument, NULL, 't'},
        {"yield", required_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };
    
    int option;
    while ((option = getopt_long(argc, argv, "i:s:t:y:", long_options, NULL)) != -1) {
        switch (option) {
            case 'i':
                iterations = atoll(optarg);
                break;
            case 's':
                switch (optarg[0]) {
                    case 'm':
                        mutex_sync = 1;
                        testName = "list-none-m";
                        break;
                    case 's':
                        spin = 1;
                        testName = "list-none-s";
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
                while (*optarg) {
                    switch (*optarg) {
                        case 'd':
                            opt_yield += DELETE_YIELD;
                            break;
                        case 'i':
                            opt_yield += INSERT_YIELD;
                            break;
                        case 'l':
                            opt_yield += LOOKUP_YIELD;
                            break;
                        default:
                            fprintf(stderr, "Usage: lab2_list [--iterations=iterations] [--threads=threads] [--yield=[idl]]\n");
                                   exit(1);
                    }
                    optarg++;
                }
                break;
            default:
                fprintf(stderr, "Usage: lab2_list [--iterations=iterations] [--threads=threads] [--yield=[idl]]\n");
                exit(1);
        }
    }
    
    if (iterations <= 0 || nthreads <= 0){
        fprintf(stderr, "Cannot have nonpositive number of iterations and/or threads.\n");
        exit(1);
    }
    signal(SIGSEGV, signal_handler);
    
    switch (opt_yield) {
        case 1:
            if (mutex_sync)
                testName = "list-i-m";
            else if (spin)
                testName = "list-i-s";
            else
                testName = "list-i-none";
            break;
        case 2:
            if (mutex_sync)
                testName = "list-d-m";
            else if (spin)
                testName = "list-d-s";
            else
                testName = "list-d-none";
            break;
        case 3:
            if (mutex_sync)
                testName = "list-id-m";
            else if (spin)
                testName = "list-id-s";
            else
                testName = "list-id-none";
            break;
        case 4:
            if (mutex_sync)
                testName = "list-l-m";
            else if (spin)
                testName = "list-l-s";
            else
                testName = "list-l-none";
            break;
        case 5:
            if (mutex_sync)
                testName = "list-il-m";
            else if (spin)
                testName = "list-il-s";
            else
                testName = "list-il-none";
            break;
        case 6:
            if (mutex_sync)
                testName = "list-dl-m";
            else if (spin)
                testName = "list-dl-s";
            else
                testName = "list-dl-none";
            break;
        case 7:
            if (mutex_sync)
                testName = "list-idl-m";
            else if (spin)
                testName = "list-idl-s";
            else
                testName = "list-idl-none";
            break;
        default:
            break;
    }
    
    if (mutex_sync)
        pthread_mutex_init(&mutex, NULL);
    operations = nthreads * iterations * 3;
    
    long long i, j = 0, nElements = nthreads * iterations;

    /* Create threads * iterations list elements with random keys */
    elements = malloc(nElements * sizeof(SortedListElement_t));
    for (i = 0; i < nElements; i++) {
        char *str = malloc(21 * sizeof(char));
        rand_str(str);
        elements[i].key = str;
    }
    
    /* Domain decomposition */
    struct Domain domains[nthreads];
    for (i = 0; i < nthreads; i++) {
        domains[i].from = j;
        domains[i].to = j + iterations;
        j += iterations;
    }
    
    /* Note starting time for the run */
    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        fprintf(stderr, "Failed to get start time.\n");
        exit(1);
    }
    
    /* Start specified number of threads */
    pthread_t threads[nthreads];
    for (i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, listTasks, (domains + i))) {
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
    
    if (SortedList_length(head)) {
        fprintf(stderr, "List not empty at end of test.\n");
        exit(2);
    }
    
    /* Print comma-separated-value recording */
    runtime = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("%s,%lld,%lld,%lld,%lld,%lld,%lld\n", testName, nthreads, iterations, (long long) 1, operations, runtime, runtime/operations);
    
    if (mutex_sync)
        pthread_mutex_destroy(&mutex);
    
    /* Deallocate all allocated memory */
    free(elements);
    free(head);
    exit(0);
}
