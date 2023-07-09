/*
Program Name: Prime Number Count and Sum Program (up to 1,000,000) 
Program Purpose: Accept two command-line parameters which indicate the number of threads to create and the maximum number to calculate prime numbers up to (denoted N). The program will then count the number of primes up to N and the sum of those primes
Author: Laurence Lu 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#define MAX_PRIMES 1000000

int num_threads;
int max_num;
int prime_count = 0;
signed int prime_sum = 0;

pthread_mutex_t lock; // create mutex variable named lock

void *primes(void* thread_id){
    int low, high, count = 0, sum = 0;
    int is_prime; // flag to check if prime

    // calculate the range of numbers, low and high, this thread will check
    int thread_num = *((int*) thread_id);
    low = (thread_num * max_num) / num_threads;
    if (thread_num == num_threads - 1) {
    high = max_num;
    } 
    else {
    high = ((thread_num + 1) * max_num) / num_threads - 1;
    }
    printf("Thread # %d is finding primes from low = %d to high = %d\n", thread_num, low, high);

    // count the number of primes and calculate the sum
    for (int i = low; i <= high; i++) {
        is_prime = 1;
        for (int j = 2; j <= sqrt(i); j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime && i >= 2) {
            count++; // count number of primes
            sum += i; // sum of all the primes
        }
    }

    // update global count and sum
    pthread_mutex_lock(&lock); // use mutex to ensure that only one thread can update the global count and sum variables at a time
    prime_count += count;
    prime_sum += sum;
    pthread_mutex_unlock(&lock); // released after update is complete

    // print results for this thread
    printf("Sum of thread %d is %d, Count is %d\n", thread_num, sum, count);
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) { // check for correct number of command line parameters 
        printf("Proper usage is ./assignment-3 <threadCount> <highestInt>\n");
        return 1;
    }

    num_threads = atoi(argv[1]);
    max_num = atoi(argv[2]);

    if (num_threads < 1 || max_num < 2 || max_num > MAX_PRIMES) { 
        /*
        checks if the number of threads is less than 1, at least one thread is required to perform calculations
        checks if the maximum number is less than 2, smallest prime number is 2
        checks if the maximum number is greater than the maximum limit set by the program, set to 1,000,000
        */
        printf("Invalid input\n");
        return 1;
    }

    // initialize mutex and create threads
    pthread_mutex_init(&lock, NULL);
    pthread_t threads[num_threads]; // create an array of threads
    int thread_ids[num_threads]; // create an array of thread IDs
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, primes, &thread_ids[i]); // loop through the thread IDs and creates threads using the pthread_create() function, passing thread ID as an argument.
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL); // wait for threads to finish
    }
    // print grand sum and count of prime numbers calculated
    printf("\n        GRAND SUM IS %d, COUNT IS %d\n", prime_sum, prime_count);

    // cleanup mutex
    pthread_mutex_destroy(&lock);

    return 0;
}
