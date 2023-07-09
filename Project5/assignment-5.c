/*
Program Name: Train Scheduling Simulation Program
Program Purpose: 500 passengers waiting at Station 0 will be distributed to the four other stations (50, 100, 250, 100 passengers to the respective stations)
where train 0 holds max 100 passengers and train 1 holds max 50 passengers. Only 1 train can be in a station at one time
Author: Laurence Lu 
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_STATIONS 5

// Initialize station passengers
int passengers[NUM_STATIONS] = {500,50,100,250,100};

// Mutex locks for each station
pthread_mutex_t station_locks[NUM_STATIONS];
// Mutex lock for printing to stdout
pthread_mutex_t print_lock;

const int TRAIN_0_CAPACITY = 100;
const int TRAIN_1_CAPACITY = 50;

// Train 0 and Train 1
struct Train {
    int id;
    int capacity;
    int current_passengers;
    int current_station;
};

struct Train train_0 = {.id = 0, .capacity = TRAIN_0_CAPACITY, .current_passengers = 0, .current_station = 0};
struct Train train_1 = {.id = 1, .capacity = TRAIN_1_CAPACITY, .current_passengers = 0, .current_station = 0};

// Function to pick up passengers at station
void pick_up_passengers(struct Train *train, int station) {
    int num_passengers = passengers[station];
    int available_capacity = (train->capacity) - (train->current_passengers);
    int pickup;
    if (available_capacity < num_passengers){
        pickup = available_capacity;
    }
    else {
        pickup = num_passengers;
    }
    pthread_mutex_lock(&print_lock);
    printf("Train %d ENTERS Station %d\n", train->id, station);
    printf("\tStation %d has %d passengers to pick up\n", station, num_passengers);
    printf("\tPicking up passengers...\n");
    pthread_mutex_unlock(&print_lock);

    // Sleep to simulate picking up passengers
    int sleep_time = pickup / 10 + 1; // +1 ensures that train waits for at least 1 second even when no passengers
    sleep(sleep_time);

    train->current_passengers += pickup;
    passengers[station] -= pickup;

    pthread_mutex_lock(&print_lock);
    printf("\tTrain %d is at Station %d and has %d/%d passengers\n", train->id, station, train->current_passengers, train->capacity);
    printf("\tStation %d has %d passengers to pick up\n", station, passengers[station]);
    printf("Train %d LEAVES Station %d\n", train->id, station);
    pthread_mutex_unlock(&print_lock);
}

// Function to drop off passengers at station
void drop_off_passengers(struct Train *train, int station) {
    int num_passengers = train->current_passengers;
    int available_space = passengers[station];
    int dropoff; 
    if (available_space < num_passengers){
        dropoff = available_space;
    } 
    else {
        dropoff = num_passengers;
    } 
    pthread_mutex_lock(&print_lock);
    printf("Train %d ENTERS Station %d\n", train->id, station);
    printf("\tStation %d has %d passengers to drop off\n", station, available_space);
    printf("\tUnloading passengers...\n");
    pthread_mutex_unlock(&print_lock);

    // Sleep to simulate dropping off passengers
    int sleep_time = dropoff / 10 + 1;
    sleep(sleep_time);

    train->current_passengers -= dropoff;
    passengers[station] += dropoff;

    pthread_mutex_lock(&print_lock);
    printf("\tTrain %d is at Station %d and has %d/%d passengers\n", train->id, station, train->current_passengers, train->capacity);
    printf("\tStation %d has %d passengers to drop off\n", station, passengers[station]);
    printf("Train %d LEAVES Station %d\n", train->id, station);
    pthread_mutex_unlock(&print_lock);
}

// Train simulation thread
void *train_thread(void *arg) {
    struct Train *train = (struct Train *) arg;

    while (1) { // run until all passengers have reached their destination
        pthread_mutex_lock(&station_locks[train->current_station]); // Lock the station
        pick_up_passengers(train, train->current_station); // Pick up passengers
        pthread_mutex_unlock(&station_locks[train->current_station]); // Unlock the station

        int sleep_time = 1; 
        sleep(sleep_time);

        int next_station = (train->current_station + 1) % NUM_STATIONS; // Move to next station
        train->current_station = next_station;
        pthread_mutex_lock(&station_locks[train->current_station]);  // Lock the station
        drop_off_passengers(train, train->current_station); // Drop off passengers
        pthread_mutex_unlock(&station_locks[train->current_station]); // Unlock the station

        // Check if all passengers have reached their destination
        int total_passengers = 0;
        for (int i = 0; i < NUM_STATIONS; i++) {
            total_passengers += passengers[i];
        }
        if (total_passengers == 0) {
            exit(0);
        }
    }
    return NULL;
}

int main() {
    // Initialize mutex locks
    for (int i = 0; i < NUM_STATIONS; i++) {
        pthread_mutex_init(&station_locks[i], NULL);
    }
    pthread_mutex_init(&print_lock, NULL);

    // Create Train threads
    pthread_t train_threads[2];
    pthread_create(&train_threads[0], NULL, train_thread, (void *) &train_0);
    pthread_create(&train_threads[1], NULL, train_thread, (void *) &train_1);

    // Join Train threads
    pthread_join(train_threads[0], NULL);
    pthread_join(train_threads[1], NULL);

    // Destroy mutex locks
    for (int i = 0; i < NUM_STATIONS; i++) {
        pthread_mutex_destroy(&station_locks[i]);
    }
    pthread_mutex_destroy(&print_lock);

    return 0;
}
