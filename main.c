#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

#define NUM_RUNS 3

/**
 * main.c
 * 
 * This makes an attempt at performing sequential and parallel 
 * Mult-threaded Matrix Multiplication, my clock timing in here
 * appears to be slightly off and my mmm_verify() is not... working 
 * right... I might have other problems but this is as good as it's 
 * gonna get cause my flight will not allow me any more time to spare...
 * 
 * Created on 3/6
 * 		author: Tyler
*/

// Function for mmm_par
void* mmm_par(void *args);


int main(int argc, char *argv[]) {
    // Check command-line arguments
    if (argc != 3 && argc != 4) {
        printf("Invalid Syntax\nUsage: ./mmm S <size>\nUsage: ./mmm P <num threads> <size>\n");
        return 1;
    }
   
    // Determine mode
    char *mode = argv[1];


    if (strcmp(mode, "S") == 0) { // Sequential mode
        // Convert "size" from string to int
        if (sscanf(argv[2], "%d", &size) != 1 || size <= 0) {
            printf("Error: Size must be positive\n");
            return 1;
        }


        // title print statements
        printf("========\n");
        printf("mode: sequential\n");
        printf("thread count: 1\n");
        printf("size: %d\n", size);
        printf("========\n");


        // Initialize matrices
        mmm_init();

        double clockstart, clockend;    // initialize clock
        double total_time = 0.0;
        for (int run = 0; run < NUM_RUNS; ++run) {
            clockstart = rtclock();         // start the clock


            // Call sequential MMM
            mmm_seq();
            clockend = rtclock(); // stop the clock
            double time_taken = clockend - clockstart;
            total_time += time_taken;
        }
        double avg_time = total_time / NUM_RUNS;
        printf("Sequential Time (avg of %d runs): %.6f sec\n", NUM_RUNS, avg_time);


        mmm_freeup(); // free stuff


    } else if (strcmp(mode, "P") == 0) { // Parallel mode
        // Convert "size" and "num threads" from string to int
        if (sscanf(argv[3], "%d", &size) != 1 || size <= 0) {
            printf("Error: Size must be positive\n");
            return 1;
        }

        // Initialize matrices
        mmm_init();

        int num_threads;
        if (sscanf(argv[2], "%d", &num_threads) != 1 || num_threads <= 0) {
            printf("Error: Number of threads must be positive\n");
            return 1;
        }
        if (num_threads > size) {
            printf("Error: Number of threads must be less than or equal to size\n");
            return 1;
        }

        // Title print statements
        printf("========\n");
        printf("mode: parallel\n");
        printf("thread count: %d\n", num_threads);
        printf("size: %d\n", size);
        printf("========\n");

        double max_error;
        double seq_total_time = 0.0;
        double par_total_time = 0.0;

        for (int run = 0; run < NUM_RUNS; ++run) {
            double clockstart, clockend;    // initialize clock

            // Sequential mode
            clockstart = rtclock();         // start the clock
            mmm_seq();
            clockend = rtclock(); // stop the clock
            double seq_time_taken = clockend - clockstart;
            seq_total_time += seq_time_taken;


			int rows_per_thread = size / num_threads;
			ThreadArgs args[num_threads];
			pthread_t threads[num_threads];
            // Parallel mode
            clockstart = rtclock();         // start the clock

            for (int i = 0; i < num_threads; i++) {
                args[i].start_row = i * rows_per_thread;
                if (i == num_threads - 1) {
                    args[i].end_row = size;
                } else {
                    args[i].end_row = (i + 1) * rows_per_thread;
                }
                pthread_create(&threads[i], NULL, mmm_par, &args[i]);
            }
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }
            clockend = rtclock(); // stop the clock
            double par_time_taken = clockend - clockstart;
            par_total_time += par_time_taken;

            // Verify correctness
            max_error = mmm_verify();
            mmm_reset(PAR_MATRIX); // reset parallel matrix for next run
        }

        // Calculate average times
        double seq_avg_time = seq_total_time / NUM_RUNS;
        double par_avg_time = par_total_time / NUM_RUNS;

        // Calculate speedup
        double speedup = seq_avg_time / par_avg_time;

        printf("Sequential Time (avg of %d runs): %.6f sec\n", NUM_RUNS, seq_avg_time);
        printf("Parallel Time (avg of %d runs): %.6f sec\n", NUM_RUNS, par_avg_time);
        printf("Speedup: %.6f\n", speedup);
        printf("Verifying... largest error between parallel and sequential matrix: %.6f\n", max_error);

        mmm_freeup(); // free stuff

    } else {
        printf("Error: mode must be either S (sequential) or P (parallel)\n");
        return 1;
    }

    return 0;
}
