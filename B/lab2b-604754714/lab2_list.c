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
SortedList_t *heads;
SortedListElement_t *elements;
int *locks;
pthread_mutex_t *mutexes;
int opt_yield = 0;
int spin = 0;
int mutex = 0;
int keylen = 21;
long long lists = 1;

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

/* Hash algorithm for element keys */
long long hash(const char *key)
{
    long long x = 97;
    int i;
    for (i = 0; i < keylen; i++)
        x ^= key[i];
    return x % lists;
}

/* Get time for each critical section */
long long locktime(struct timespec start, struct timespec end)
{
    return 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}

/* Divide array of elements among threads */
struct Domain {
    long long from, to, locktime;
};

/* Catch segmentation faults */
void signal_handler(int sig)
{
    if (sig == SIGSEGV) {
        //printf("%d\n", SortedList_length(head));
        fprintf(stderr, "Caught segmentation fault.\n");
        exit(2);
    }
}

void *listTasks(void *domain)
{
    struct Domain* t_domain = (struct Domain*) domain;
    long long i, length = 0, from = t_domain->from, to = t_domain->to;
    
    struct timespec start, end;
    /* Insert elements into particular sublist based on hash of its key */
    for (i = from; i < to; i++){
        long long hashval = hash(elements[i].key);
        if (spin) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            while (__sync_lock_test_and_set(&locks[hashval], 1))
                ; // spin-wait
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }
        else if (mutex) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            pthread_mutex_lock(&mutexes[hashval]);
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }

        SortedList_insert(&heads[hashval], &elements[i]);
        if (spin)
             __sync_lock_release(&locks[hashval]);
        else if (mutex)
             pthread_mutex_unlock(&mutexes[hashval]);
    }
    
    /* Find length of entire list, consisting of all sublists */
    for (i = 0; i < lists; i++){
        if (spin) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            while (__sync_lock_test_and_set(&locks[i], 1))
                ; // spin-wait
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }
        else if (mutex) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            pthread_mutex_lock(&mutexes[i]);
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }
        
        length += SortedList_length(&heads[i]);
        if (spin)
            __sync_lock_release(&locks[i]);
        else if (mutex)
            pthread_mutex_unlock(&mutexes[i]);
    }
    
    if (length < iterations) {
        fprintf(stderr, "Did not insert enough items.\n");
        exit(2);
    }
    
    /* Look up and delete all keys */
    for (i = from; i < to; i++) {
        /* Find sublist containing key */
        const char *key = elements[i].key;
        long long hashval = hash(key);
        SortedListElement_t *find;
        if (spin) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            while (__sync_lock_test_and_set(&locks[hashval], 1))
                ; // spin-wait
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }
        else if (mutex) {
            //struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            pthread_mutex_lock(&mutexes[hashval]);
            clock_gettime(CLOCK_MONOTONIC, &end);
            t_domain->locktime += locktime(start, end);
        }
        
        find = SortedList_lookup(&heads[hashval], key);
        if (find == &heads[hashval]) {
            fprintf(stderr, "Cannot find inserted element.\n");
            exit(2);
        }
        
        /* Delete it from the sublist */
        SortedList_delete(find);
        
        if (spin)
            __sync_lock_release(&locks[hashval]);
        else if (mutex)
            pthread_mutex_unlock(&mutexes[hashval]);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    long long operations, runtime, totallocktime = 0;
    /* Parse command line arguments */
    static struct option long_options[] = {
        {"iterations", required_argument, NULL, 'i'},
        {"lists", required_argument, NULL, 'l'},
        {"sync", required_argument, NULL, 's'},
        {"threads", required_argument, NULL, 't'},
        {"yield", required_argument, NULL, 'y'},
        {0, 0, 0, 0}
    };
    
    int option;
    while ((option = getopt_long(argc, argv, "i:l:s:t:y:", long_options, NULL)) != -1) {
        switch (option) {
            case 'i':
                iterations = atoll(optarg);
                break;
            case 'l':
                lists = atoll(optarg);
                break;
            case 's':
                switch (optarg[0]) {
                    case 'm':
                        mutex = 1;
                        testName = "list-none-m";
                        break;
                    case 's':
                        spin = 1;
                        testName = "list-none-s";
                        break;
                    default:
                        fprintf(stderr, "Usage: lab2_add [--iterations=iterations] --sync=[m,s] [--threads=threads]\n");
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
                            fprintf(stderr, "Usage: lab2_list [--iterations=iterations] --sync=[m,s] [--threads=threads] [--yield=[idl]]\n");
                                   exit(1);
                    }
                    optarg++;
                }
                break;
            default:
                fprintf(stderr, "Usage: lab2_list [--iterations=iterations] --sync=[m,s] [--threads=threads] [--yield=[idl]]\n");
                exit(1);
        }
    }
    
    if (iterations <= 0 || nthreads <= 0 || lists <= 0){
        fprintf(stderr, "Cannot have nonpositive number of iterations, threads, and/or lists.\n");
        exit(1);
    }
    signal(SIGSEGV, signal_handler);
    
    switch (opt_yield) {
        case 1:
            if (mutex)
                testName = "list-i-m";
            else if (spin)
                testName = "list-i-s";
            else
                testName = "list-i-none";
            break;
        case 2:
            if (mutex)
                testName = "list-d-m";
            else if (spin)
                testName = "list-d-s";
            else
                testName = "list-d-none";
            break;
        case 3:
            if (mutex)
                testName = "list-id-m";
            else if (spin)
                testName = "list-id-s";
            else
                testName = "list-id-none";
            break;
        case 4:
            if (mutex)
                testName = "list-l-m";
            else if (spin)
                testName = "list-l-s";
            else
                testName = "list-l-none";
            break;
        case 5:
            if (mutex)
                testName = "list-il-m";
            else if (spin)
                testName = "list-il-s";
            else
                testName = "list-il-none";
            break;
        case 6:
            if (mutex)
                testName = "list-dl-m";
            else if (spin)
                testName = "list-dl-s";
            else
                testName = "list-dl-none";
            break;
        case 7:
            if (mutex)
                testName = "list-idl-m";
            else if (spin)
                testName = "list-idl-s";
            else
                testName = "list-idl-none";
            break;
        default:
            break;
    }
        
    long long i, j = 0, nElements = nthreads * iterations;
    operations = nElements * 3;

    /* Initialize array of list heads */
    heads = malloc(lists * sizeof(SortedList_t));
    for (i = 0; i < lists; i++) {
        heads[i].key = NULL;
        heads[i].prev = &heads[i];
        heads[i].next = &heads[i];
    }
    
    /* Initialize array of locks */
    if (spin) {
        locks = malloc(lists * sizeof(int));
        for (i = 0; i < lists; i++)
            locks[i] = 0;
    }
    else if (mutex) {
        mutexes = malloc(lists * sizeof(pthread_mutex_t));
        for (i = 0; i < lists; i++)
            pthread_mutex_init(&mutexes[i], NULL);
    }
    
    /* Create threads * iterations list elements with random keys */
    elements = malloc(nElements * sizeof(SortedListElement_t));
    for (i = 0; i < nElements; i++) {
        char *str = malloc(keylen * sizeof(char));
        rand_str(str);
        //printf("%s: %lld\n", str, hash(str));
        elements[i].key = str;
    }
    
    /* Domain decomposition */
    struct Domain domains[nthreads];
    for (i = 0; i < nthreads; i++) {
        domains[i].from = j;
        domains[i].to = j + iterations;
        domains[i].locktime = 0;
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
    
    /* Make sure entire list is empty */
    for (i = 0; i < lists; i++) {
        if (SortedList_length(&heads[i])) {
            fprintf(stderr, "Sublist not empty.\n");
            exit(2);
        }
    }
    
    /* Compute average wait-per-lock */
    for (i = 0; i < nthreads; i++)
        totallocktime += domains[i].locktime;
    
    /* Print comma-separated-value recording */
    runtime = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("%s,%lld,%lld,%lld,%lld,%lld,%lld,%lld\n", testName, nthreads, iterations, lists, operations, runtime, runtime/operations, totallocktime/operations);
    
    /* Deallocate all allocated memory */
    free(elements);
    free(heads);
    if (spin)
        free(locks);
    else if (mutex){
        for (i = 0; i < lists; i++)
            pthread_mutex_destroy(&mutexes[i]);
        free(mutexes);
    }
    exit(0);
}
