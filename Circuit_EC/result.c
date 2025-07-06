#include "result.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

complex c_multiply(complex c1, complex c2) {                                                /* multiplies complexes*/
    complex result;                                                                         /* output*/
    result.real = c1.real * c2.real - c1.imag * c2.imag;                                    /* real part*/
    result.imag = c1.real * c2.imag + c1.imag * c2.real;                                    /* imaginary part*/
    return result;                                                                          /* returns product*/
}

complex c_sum(complex c1, complex c2) {                                                     /* sums complexes*/
    complex result;                                                                         /* output*/
    result.real = c1.real + c2.real;                                                        /* real part*/
    result.imag = c1.imag + c2.imag;                                                        /* imaginary part*/
    return result;                                                                          /* returns product*/
}

void m_mult(matrix m1, matrix m2, matrix *res, int qubits) {                                /* multiplies two matrices*/
    int len = (int)pow(2, qubits);                                                          /* number of columns or rows*/
    int i = 0;                                                                              /* initializes i for rows*/
    for (i = 0; i < len; i++) {                                                             /* iterates rows*/
        int j = 0;                                                                          /* initializes j for columns*/
        for (j = 0; j < len; j++) {                                                         /* iterates columns*/
            complex c = {0.0, 0.0};                                                         /* initializes complex*/
            int k = 0;                                                                      /* initializes k for the sum*/
            for (k = 0; k < len; k++) {                                                     /* iterates both*/
                c = c_sum(c, c_multiply(m1.rows[i].values[k], m2.rows[k].values[j]));       /* gets the complex*/
            }
            res -> rows[i].values[j] = c;                                                   /* assigns the complex*/
        }
    }
}

void* thread_mult(void* arg) {                                                              /* thread function for matrix multiplication*/
    threads_data* data = (threads_data*)arg;                                                /* casts the argument to threads_data*/
    m_mult(*data->a, *data->b, data->result, data->qubits);                                 /* multiplies the matrices*/
    return NULL;                                                                            /* returns NULL*/
}

void allocate_matrix(matrix* m, int len) {                                                  /* allocates memory for a matrix*/
    m -> rows = malloc(len * sizeof(vector));                                               /* allocates memory for rows*/
    int i = 0;                                                                              /* initializes i for rows*/
    for (i = 0; i < len; i++) {                                                             /* iterates rows*/
        m -> rows[i].values = malloc(len * sizeof(complex));                                /* allocates memory for values*/
    }                                                                           
}

void free_matrix(matrix m, int len) {                                                       /* frees memory for a matrix*/
    int i = 0;                                                                              /* initializes i for rows*/
    for (i = 0; i < len; i++) {                                                             /* iterates rows*/
        free(m.rows[i].values);                                                             /* frees memory for values*/
    }   
    free(m.rows);                                                                           /* frees memory for rows*/
}

matrix get_product(circuit all_circ, int qubits, char** order, int threads) {               /* multiplies all matrices*/
    int num_order = 0;                                                                      /* number of matrices in the order*/
    while (order[num_order] != NULL) {                                                      /* iterates the order*/
        num_order++;                                                                        /* increases the number of matrices*/
    }
    int real = 0;                                                                           /* number of threads to use*/
    if (threads > num_order / 2) {                                                          /* if the number of threads is greater than the number of matrices divided by 2*/
        real = num_order / 2;                                                               /* if the number of threads is greater than the number of matrices divided by 2*/
    }
    else {                                                                                  /* if the number of threads is less than the number of matrices divided by 2*/
        real = threads;                                                                     /* sets the number of threads to the number of threads specified by the user*/
    }
    pthread_t thread_ids[real];                                                             /* array of thread ids*/
    int dim = num_order;                                                                    /* number of matrices in the circuit*/
    while (dim > 1) {                                                                       /* while there are more than one matrix*/
        int disp = dim / 2;                                                                 /* number of threads to use*/
        int reach = 0;                                                                      /* number of threads reached*/
        int counter = 0;                                                                    /* counter for the matrices*/
        int used = 0;                                                                       /* number of threads used*/
        int index = 0;                                                                      /* index for the result matrices*/
        threads_data* data = malloc(disp * sizeof(threads_data));                           /* allocates memory for the threads data*/
        while (reach < disp) {                                                              /* while the reach is less than the number of threads*/
            int c = 0;                                                                      /* initializes c for the threads*/
            for (c = 0; c < real; c++) {                                                    /* iterates the threads*/
                if (reach >= disp) {                                                        /* if the reach is greater than the number of threads*/
                    break;                                                                  /* breaks the loop if the number of threads is reached*/
                }
                data[reach].a = &all_circ.cir[counter];                                     /* assigns the matrices to multiply*/
                data[reach].b = &all_circ.cir[counter+1];                                   /* assigns the matrices to multiply*/
                data[reach].result = malloc(sizeof(matrix));                                /* sets the number of qubits*/
                data[reach].qubits = qubits; 
                allocate_matrix(data[reach].result, (int)pow(2, qubits));                   /* allocates memory for the result matrix*/
                pthread_create(&thread_ids[c], NULL, thread_mult, &data[reach]);            /* creates the thread*/
                reach++;                                                                    /* increases the reach*/
                used++;                                                                     /* increases the used threads*/
                counter += 2;                                                               /* increases the counter  */
            }
            int k = 0;                                                                      /* initializes k for the threads*/
            for (k = 0; k < used; k++) {                                                    /* waits for the threads to finish*/
                pthread_join(thread_ids[k], NULL);                                          /* waits for the thread to finish*/
                all_circ.cir[index] = *data[index].result;                                  /* assigns the result matrix to the circuit */
                index++;                                                                    /* increases the index*/
            } 
            used = 0;                                                                       /* resets the used threads*/
        }    
        index = 0;                                                                          /* resets the index*/
        counter = 0;                                                                        /* resets the counter*/
        if (dim % 2 == 1) {                                                                 /* if the number of matrices is odd*/
            all_circ.cir[reach] = all_circ.cir[dim - 1];                                    /* assigns the last matrix to the circuit*/
            dim = dim / 2 + 1;                                                              /* increases the number of matrices by 1*/
        }
        else {                                                                              /* if the number of matrices is even*/
            dim = dim / 2;                                                                  /* divides the number of matrices by 2*/
        }
        reach = 0;                                                                          /* resets the reach*/
        matrix *temp = realloc(all_circ.cir, dim*sizeof(matrix));                           /* reallocates memory for the circuit matrices*/
        if (temp != NULL) {                                                                 /* reallocates memory for the circuit matrices*/
            all_circ.cir = temp;                                                            /* assigns the new circuit matrices*/
        }
        else {                                                                              /* if realloc fails*/
            fprintf(stderr, "Memory allocation failed\n");                                  /* error*/
            exit(EXIT_FAILURE);                                                             /* exits the program*/
        }        
    }
    return all_circ.cir[0];                                                                 /* returns the product matrix*/
}

double get_module(complex c) {                                                              /* gets the module squared of a complex*/
    return pow(c.real, 2) + pow(c.imag, 2);                                                 /* returns the module*/
}

vector get_vout(matrix product, vector vin, int qubits, vector vout) {                      /* multiplicates the product for the input vector*/
    int len = (int)pow(2, qubits);                                                          /* number of rows or columns*/
    int i = 0;                                                                              /* initializes i for rows*/
    for (i = 0; i < len; i++) {                                                             /* iterates rows*/
        complex c = {0.0, 0.0};                                                             /* initializes complex*/
        int j = 0;                                                                          /* initializes j for columns*/
        for (j = 0; j < len; j++) {                                                         /* iterates columns*/
            c = c_sum(c, c_multiply(product.rows[i].values[j], vin.values[j]));             /* get the complex*/
        }
        vout.values[i] = c;                                                                 /* assigns the complex*/
    }
    return vout;                                                                            /* return output vector*/
}

double norm_control(vector vout) {                                                          /* controls the norm of the output vector*/
    double norm = 0.0;                                                                      /* initializes norm*/
    int i = 0;                                                                              /* initializes i for the output vector*/
    for (i = 0; i < vout.length; i++) {                                                     /* iterates the output vector*/
        norm += get_module(vout.values[i]);                                                 /* gets the module squared*/
    }
    /*
    double tollerance = 1e-6;                                                               
    if (norm > 1 + tollerance || norm < 1 - tollerance) {                                   
        return 0;                                                                           
    }   
    */ 
    return norm;                                                                            /* the norm is valid*/
}