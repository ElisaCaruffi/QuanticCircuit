#include "data.h"
#include "result.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

int main() {
    // reads the init file
    printf("Insert the init file name: ");
    char init[1024];
    scanf("%s", init);
    char* lines = read_file(init);
    if (lines == NULL) {
        fprintf(stderr, "Error reading init file\n");
        return 1;
    }
    // gets the number of qubits
    int qubits = get_qubits(lines);
    int len = (int)pow(2, qubits);

    // gets the input vector
    vector vin;
    vin.values = malloc(len * sizeof(complex));
    vin = get_vin(lines, qubits, vin);
    free(lines);

    // reads the circ file
    printf("Insert the circ file name: ");
    char circ[1024];
    scanf("%s", circ);
    char* lines2 = read_file(circ);
    if (lines2 == NULL) {
        fprintf(stderr, "Error reading circ file\n");
        free(vin.values);
        return 1;
    }

    // gets the matrices order
    char* order[1024] = {NULL};
    get_order(lines2, order);

    // inverts the order
    int num_order = 0;
    while (order[num_order] != NULL) num_order++;
    int i = 0;
    for (i = 0; i < num_order / 2; i++) {
        char* temp = order[i];
        order[i] = order[num_order - 1 - i];
        order[num_order - 1 - i] = temp;
    }

    // gets the circuit
    circuit all_circ;
    all_circ.cir = malloc(num_order * sizeof(matrix));
    if (!all_circ.cir) {
        fprintf(stderr, "malloc failed\n");
        free(vin.values);
        free(lines2);
        return 1;
    }
    all_circ = get_matrices(lines2, qubits, order, all_circ, (matrix){0}, (vector){0});
    free(lines2);

    // gets the number of threads
    int threads;
    printf("Insert the number of threads: ");
    scanf("%i", &threads);
    if (threads < 1) {
        fprintf(stderr, "The number of threads must be greater than 0\n");
        free(vin.values);
        free(all_circ.cir);
        return 1;
    }
    
    // gets the matrices' product
    matrix product = get_product(all_circ, qubits, order, threads);

    // gets the output vector
    vector vout;
    vout.values = malloc(len * sizeof(complex));
    vout.length = len;
    vout = get_vout(product, vin, qubits, vout);

    // checks the norm of the output vector
    double norm = norm_control(vout);
    printf("The output vector's norm is: %lf\n", norm);
    printf("The output vector is:\n[(");
    int i = 0;
    for (i = 0; i < len; i++) {
        if (vout.values[i].imag < 0)
            printf("%0.5lf - i%0.5lf", vout.values[i].real, -vout.values[i].imag);
        else
            printf("%0.5lf + i%0.5lf", vout.values[i].real, vout.values[i].imag);

        if (i != len - 1)
            printf(", ");
    }
    printf(")]\n");

    // frees memory
    free(vout.values);
    free(vin.values);
    free_matrix(product, len);
    free(all_circ.cir);

    return 0;
}