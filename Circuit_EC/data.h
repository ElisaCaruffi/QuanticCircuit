#ifndef DATA_H
#define DATA_H

typedef struct {
    double real;
    double imag;
} complex;

typedef struct {
    int length;
    complex* values;
} vector;

typedef struct {
    int n_rows;
    vector* rows;
} matrix;

typedef struct {
    matrix* cir;
    int n;
} circuit;

char* read_file(char filename[]);
int get_qubits(char* lines);
complex get_complex(char* parse);
vector get_vin(char* lines, int qubits, vector vin);
void get_order(char* lines, char** order);
char* matrix_name(char *line);
char* get_matrix_str(char* line);
int num_rows(char* matrix_str);
int num_columns(char* row);
vector parse_row(char* row_str, int cols);
circuit get_matrices(char* lines, int qubits, char** order, circuit all_circ, matrix m, vector row);

#endif