#ifndef RESULT_H
#define RESULT_H
#include "data.h"

complex c_multiply(complex c1, complex c2);
complex c_sum(complex c1, complex c2);
void m_mult(matrix m1, matrix m2, matrix *res, int qubits);
void copy_vector(vector* out, vector in, int qubits);
void copy_matrix(matrix* out, matrix in, int qubits);
matrix allocate_matrix(int len);
void free_matrix(matrix m, int len);
matrix get_product(circuit all_circ, int qubits, char** order);
double get_module(complex c);
vector get_vout(matrix product, vector vin, int qubits, vector vout);
int norm_control(vector vout);

#endif