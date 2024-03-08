#ifndef MMM_H_
#define MMM_H_

// Struct to hold thread args
typedef struct {
    int start_row;
    int end_row;
} ThreadArgs;

// shared globals
extern unsigned int mode;
extern unsigned int size, num_threads;
extern double **A, **B, **SEQ_MATRIX, **PAR_MATRIX;

void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void* mmm_par(void *);
double mmm_verify();

#endif /* MMM_H_ */
