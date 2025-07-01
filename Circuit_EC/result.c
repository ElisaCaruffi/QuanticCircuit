#include "result.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

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

void copy_vector(vector* out, vector in, int qubits) {                                      // copies the vector
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    for (int i = 0; i<len; i++) {                                                           // iterates
        out -> values[i] = in.values[i];                                                    // assigns vector
    }
}

void copy_matrix(matrix* out, matrix in, int qubits) {                                      // copies the matrix
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    for (int i = 0; i<len; i++) {                                                           // iterates
        copy_vector(&out -> rows[i], in.rows[i], qubits);                                   // asiigns the matrix
    }
}

matrix allocate_matrix(int len) {                                                           // allocates memory for a matrix
    matrix m;                                                                               // initializes matrix
    m.rows = malloc(len * sizeof(vector));                                                  // allocates memory for rows
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        m.rows[i].values = malloc(len * sizeof(complex));                                   // allocates memory for values
    }
    return m;                                                                               // returns the matrix
}

void free_matrix(matrix m, int len) {                                                       // frees memory for a matrix
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        free(m.rows[i].values);                                                             // frees memory for values
    }   
    free(m.rows);                                                                           // frees memory for rows
}

matrix get_product(circuit all_circ, int qubits, char** order) {                            // multiplies all matrices
    int num_order = 0;                                                                      // number of matrices in the order
    while (order[num_order] != NULL) {                                                      // iterates the order
        num_order++;                                                                        // increases the number of matrices
    }
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    matrix temp = allocate_matrix(len);                                                     // allocates memory for the matrix
    matrix result = allocate_matrix(len);                                                   // allocates memory for the result matrix
    copy_matrix(&temp, all_circ.cir[0], qubits);                                            // copies the first matrix to temp
    for (int i = 1; i < num_order; i++) {                                                   // iterates the order
        m_mult(temp, all_circ.cir[i], &result, qubits);                                     // multiplies the matrices
        copy_matrix(&temp, result, qubits);                                                 // copies the result to temp
    }
    free_matrix(result, len);                                                               // frees memory for the result matrix
    return temp;                                                                            // returns the product
}

double get_module(complex c) {                                                              // gets the module squared of a complex
    return pow(c.real, 2) + pow(c.imag, 2);                                                 // returns the module
}

vector get_vout(matrix product, vector vin, int qubits, vector vout) {                      // multiplicates the product for the input vector
    int len = (int)pow(2, qubits);                                                          // number of rows or columns
    for (int i = 0; i < len; i++) {                                                         // iterates rows
        complex c = {0.0, 0.0};                                                             // initializes complex
        for (int j = 0; j < len; j++) {                                                     // iterates columns
            c = c_sum(c, c_multiply(product.rows[i].values[j], vin.values[j]));             // get the complex
        }
        vout.values[i] = c;                                                                 // assigns the complex
    }
    return vout;                                                                            // return output vector
}

int norm_control(vector vout) {                                                             // controls the norm of the output vector
    double norm = 0.0;                                                                      // initializes norm
    for (int i = 0; i < vout.length; i++) {                                                 // iterates the output vector
        norm += get_module(vout.values[i]);                                                 // gets the module squared
    }
    double tollerance = 1e-3;                                                               // tolerance for the norm
    if (norm > 1 + tollerance || norm < 1 - tollerance) {                                   // if the norm is not equal to 1
        return 0;                                                                           // error
    }
    return 1;                                                                               // the norm is valid
}