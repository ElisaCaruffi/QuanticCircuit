#ifndef RESULT_H
#define RESULT_H
#include "data.h"

typedef struct {
    matrix *a;
    matrix *b;
    matrix *result;
    int qubits;
} threads_data;

complex c_multiply(complex c1, complex c2);
complex c_sum(complex c1, complex c2);
void m_mult(matrix m1, matrix m2, matrix *res, int qubits);
void allocate_matrix(matrix* m, int len);
void free_matrix(matrix m, int len);
matrix get_product(circuit all_circ, int qubits, char** order, int threads);
double get_module(complex c);
vector get_vout(matrix product, vector vin, int qubits, vector vout);
double norm_control(vector vout);

#endif