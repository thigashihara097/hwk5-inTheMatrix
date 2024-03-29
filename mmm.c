#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "mmm.h"

/**
 * mmm.c
 * 
 * Created on 3/6
 * 		author: Tyler
*/

unsigned int size, num_threads;
double **A, **B, **SEQ_MATRIX, **PAR_MATRIX;

/**
 * Allocate and initialize the matrices on the heap. Populate
 * the input matrices with random numbers from 0 to 99
 */
void mmm_init() {
	// A = (double **) malloc(size * sizeof(double*));
	// allocate the rest of the matrices

	A = (double **) malloc(size * sizeof(double *));
	B = (double **) malloc(size * sizeof(double *));
	SEQ_MATRIX = (double **) malloc(size * sizeof(double *));
	PAR_MATRIX = (double **) malloc(size * sizeof(double *));

	for (int i = 0; i < size; i++){
		A[i] = (double *) malloc(size * sizeof(double));
		B[i] = (double *) malloc(size * sizeof(double));
		SEQ_MATRIX[i] = (double *) malloc(size * sizeof(double));
		PAR_MATRIX[i] = (double *) malloc(size * sizeof(double));
	}

	srand((unsigned)time(NULL));	// seed the random number generator

 	// initialize A and B with random values between 0 and 99
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			A[i][j] = rand() % 100;
			B[i][j] = rand() % 100;
		}
	}

	// initialize SEQ_MATRIX and PAR_MATRIX with 0s
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			SEQ_MATRIX[i][j] = 0.0;
			PAR_MATRIX[i][j] = 0.0;
		}
	}
}

/**
 * Reset a given matrix to zeroes (their size is in the global var)
 * @param matrix pointer to a 2D array
 */
void mmm_reset(double **matrix) {
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			matrix[i][j] = 0.0;
		}
	}
}

/**
 * Free up memory allocated to all matrices
 * (their size is in the global var)
 */
void mmm_freeup() {
	for (int i = 0; i < size; i++){
		free(A[i]);
		free(B[i]);
		free(SEQ_MATRIX[i]);
		free(PAR_MATRIX[i]);
	}
	free(A);
	free(B);
	free(SEQ_MATRIX);
	free(PAR_MATRIX);
}

/**
 * Sequential MMM (size is in the global var)
 */
void mmm_seq() {
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			for (int k = 0; k < size; k++){
				SEQ_MATRIX[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

/**
 * Parallel MMM
 */
void *mmm_par(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                PAR_MATRIX[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return NULL;


}

/**
 * Verifies the correctness between the matrices generated by
 * the sequential run and the parallel run.
 *
 * @return the largest error between two corresponding elements
 * in the result matrices
 */
double mmm_verify() {
	double max_error = 0.0;

	// compare sequential and parallel
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			double error = fabs(SEQ_MATRIX[i][j] - PAR_MATRIX[i][j]);
			if (error > max_error){
				max_error = error;
			}
		}
	}
	return max_error;
}
