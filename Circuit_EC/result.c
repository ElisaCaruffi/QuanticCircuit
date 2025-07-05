#include "result.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

void print_matrix(matrix m, int qubits) {
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    for (int i = 0; i < m.n_rows; i++) {                                                        // iterates rows
        for (int j = 0; j < len; j++) {                                                         // iterates columns
            if (m.rows[i].values[j].imag < 0) {                                             // if the imaginary part is negative
                printf("%lf - %lfi ", m.rows[i].values[j].real, -m.rows[i].values[j].imag); // prints the complex number
            } else {
                printf("%lf + %lfi ", m.rows[i].values[j].real, m.rows[i].values[j].imag);  // prints the complex number
            }
        }
        printf("\n");                                                                        // new line after each row
    }
}

complex c_multiply(complex c1, complex c2) {                                                // multiplies complexes
    complex result;                                                                         // output
    result.real = c1.real * c2.real - c1.imag * c2.imag;                                    // real part
    result.imag = c1.real * c2.imag + c1.imag * c2.real;                                    // imaginary part
    return result;                                                                          // returns product
}

complex c_sum(complex c1, complex c2) {                                                     // sums complexes
    complex result;                                                                         // output
    result.real = c1.real + c2.real;                                                        // real part
    result.imag = c1.imag + c2.imag;                                                        // imaginary part
    return result;                                                                          // returns product
}

void m_mult(matrix m1, matrix m2, matrix *res, int qubits) {                                // multiplies two matrices
    int len = (int)pow(2, qubits);                                                          // number of columns or rows
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        for (int j = 0; j < len; j++) {                                                     // iterates columns
            complex c = {0.0, 0.0};                                                         // initializes complex
            for (int k = 0; k < len; k++) {                                                 // iterates both
                c = c_sum(c, c_multiply(m1.rows[i].values[k], m2.rows[k].values[j]));       // gets the complex
            }
            res -> rows[i].values[j] = c;                                                   // assigns the complex
        }
    }
}

void* thread_mult(void* arg) {                                                              // thread function for matrix multiplication
    threads_data* data = (threads_data*)arg;                                                // casts the argument to threads_data
    m_mult(*data->a, *data->b, data->result, data->qubits);                                  // multiplies the matrices
    return NULL;                                                                            // returns NULL
}

void allocate_matrix(matrix* m, int len) {                                                           // allocates memory for a matrix
    m -> rows = malloc(len * sizeof(vector));                                                  // allocates memory for rows
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        m -> rows[i].values = malloc(len * sizeof(complex));                                   // allocates memory for values
    }                                                                           
}

void free_matrix(matrix m, int len) {                                                       // frees memory for a matrix
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        free(m.rows[i].values);                                                             // frees memory for values
    }   
    free(m.rows);                                                                           // frees memory for rows
}

matrix get_product(circuit all_circ, int qubits, char** order, int threads) {               // multiplies all matrices
    int num_order = 0;                                                                      // number of matrices in the order
    while (order[num_order] != NULL) {                                                      // iterates the order
        num_order++;                                                                        // increases the number of matrices
    }
    int real = 0;
    if (threads > num_order / 2) {
        real = num_order / 2;                                                            // if the number of threads is greater than the number of matrices divided by 2
    }
    else {
        real = threads;                                                     
    }
    pthread_t thread_ids[real];                                                            // array of thread ids
    int dim = num_order;
    while (dim > 1) {
        int disp = dim / 2;
        int reach = 0;
        int counter = 0;
        int used = 0;
        int index = 0;
        threads_data* data = malloc(disp * sizeof(threads_data));                           // allocates memory for the threads data
        while (reach < disp) {
            for (int c = 0; c < real; c++) {
                if (reach >= disp) {
                    break;                                                                // breaks the loop if the number of threads is reached
                }
                data[reach].a = &all_circ.cir[counter];
                data[reach].b = &all_circ.cir[counter+1];   
                data[reach].result = malloc(sizeof(matrix));                                       // sets the number of qubits
                data[reach].qubits = qubits; 
                allocate_matrix(data[reach].result, (int)pow(2, qubits));                       // allocates memory for the result matrix
                pthread_create(&thread_ids[c], NULL, thread_mult, &data[reach]);                // creates the thread
                reach++;
                used++;
                counter += 2;                                                                 // increases the counter  
            }
            for (int k = 0; k < used; k++) {                                                   // waits for the threads to finish
                pthread_join(thread_ids[k], NULL);
                all_circ.cir[index] = *data[index].result;
                index++;
            } 
            used = 0;
        }    
        index = 0;
        counter = 0;
        if (dim % 2 == 1) {
            all_circ.cir[reach] = all_circ.cir[dim - 1];
            dim = dim / 2 + 1;
        }
        else {
            dim = dim / 2;
        }
        reach = 0;
        matrix *temp = realloc(all_circ.cir, dim*sizeof(matrix));
        if (temp != NULL) {                                                                 // reallocates memory for the circuit matrices
            all_circ.cir = temp;                                                            // assigns the new circuit matrices
        }
        else {
            fprintf(stderr, "Memory allocation failed\n");                                  // error
            exit(EXIT_FAILURE);                                                             // exits the program
        }        
    }
    return all_circ.cir[0];
}

double get_module(complex c) {                                                              // gets the module squared of a complex
    return pow(c.real, 2) + pow(c.imag, 2);                                                 // returns the module
}

vector get_vout(matrix product, vector vin, int qubits, vector vout) {                      // multiplicates the product for the input vector
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    for (int i = 0; i < len; i++) {                                                             // iterates rows
        complex c = {0.0, 0.0};                                                             // initializes complex
        for (int j = 0; j < len; j++) {                                                         // iterates columns
            c = c_sum(c, c_multiply(product.rows[i].values[j], vin.values[j]));             // get the complex
        }
        vout.values[i] = c;                                                                 // assigns the complex
    }
    return vout;                                                                            // return output vector
}

double norm_control(vector vout) {                                                          // controls the norm of the output vector
    double norm = 0.0;                                                                      // initializes norm
    for (int i = 0; i < vout.length; i++) {                                                     // iterates the output vector
        norm += get_module(vout.values[i]);                                                 // gets the module squared
    }
    /*
    double tollerance = 1e-3;                                                               // tolerance for the norm
    if (norm > 1 + tollerance || norm < 1 - tollerance) {                                   // if the norm is not equal to 1
        return 0;                                                                           // error
    }
    */
    return norm;                                                                               // the norm is valid
}