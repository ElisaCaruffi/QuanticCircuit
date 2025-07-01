#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

char* read_file(char filename[]) {                                                      // reads the file
    FILE *file = fopen(filename, "r");                                                  // opens file

    if (file == NULL) {                                                                 // if file doesn't exist
        fprintf(stderr, "Error opening file");                                          // error 
    }
    fseek(file, 0, SEEK_END);                                                           // moves to the end of the file
    long filesize = ftell(file);                                                        // gets the size of the file
    rewind(file);                                                                       // moves to the beginning of the file
    char* temp = malloc(filesize + 1);                                                  // allocates memory for the file content
    if (temp == NULL) {                                                                 // if memory allocation wasn't successful
        fprintf(stderr, "Allocation error\n");                                          // error
        fclose(file);                                                                   // closes the file
        return NULL;                                                                    // returns NULL
    }
    fread(temp, sizeof(char), filesize, file);                                          // reads the file content
    temp[filesize] = '\0';                                                              // adds null terminator
    char* lines = malloc(filesize + 2);                                                 // allocates memory for the lines
    if (lines == NULL) {                                                                // if memory allocation wasn't successful
        fprintf(stderr, "Allocation error\n");                                          // error
        fclose(file);                                                                   // closes the file
        free(temp);                                                                     // frees memory
        return NULL;                                                                    // returns NULL
    }
    int i = 0;                                                                          // index of the file content
    int j = 0;                                                                          // index of the lines
    bool new = true;                                                                    // flag to check if the line is new
    while (temp[i] != '\0') {                                                           // while the file content doesn't end
        if (new && strncmp(&temp[i], "#circ", 5) == 0) {                                // if the line starts with #circ
            if (j > 0 && lines[j - 1] != '\n') {                                        // if the previous character isn't a newline
                lines[j] = '\n';                                                        // adds newline character
                j++;                                                                    // increases index
            }
            while (temp[i] != '\n' && temp[i] != '\0') {                                // skips spaces and tabs
                lines[j] = temp[i];                                                     // adds character to the lines
                i++;                                                                    // increases index
                j++;                                                                    // increases index
            }
            if (temp[i] == '\n') {                                                      // if the line ends
                lines[j] = temp[i];                                                     // adds newline character
                i++;                                                                    // increases index 
                j++;                                                                    // increases index
            }
            new = true;                                                                 // sets new to true
            continue;                                                                   // continues to the next iteration
        }
        if (temp[i] == ' ' || temp[i] == '\t' || temp[i] == '\n') {                     // if the character is a space, tab or newline
            if (temp[i] == '\n') {                                                      // if the character is a newline
                 new = 1;                                                               // sets new to true
            }
            i++;                                                                        // moves to the next character
            continue;                                                                   // continues to the next iteration
        }
        new = false;                                                                    // sets new to false
        if (temp[i] == '#' && j != 0) {                                                 // if the character is # and the index isn't 0
            lines[j] = '\n';                                                            // adds newline character
            j++;                                                                        // increases index
        }
        lines[j] = temp[i];                                                             // adds character to the lines
        i++;                                                                            // moves to the next character
        j++;                                                                            // increases index
    }
    lines[j] = '\0';                                                                    // adds null terminator
    fclose(file);                                                                       // closes the file
    free(temp);                                                                         // frees memory

    return lines;                                                                       // returns the lines
}

int get_qubits(char* lines) {                                                           // gets the number of qubits
    if (strncmp(lines, "#qubits", 7) == 0) {                                            // if the line starts with #qubits
        int start = 7;                                                                  // starts from the 8th character

        int end = start + 1;                                                            // starts from the next character
        while (lines[end] != '\n') {                                                    // skips spaces and tabs
            end++;                                                                      // moves to the next character
        }
        char str[100];                                                                  // array
        int len = end - start;                                                          // length of the string
        strncpy(str, &lines[start], len);                                               // copies the string
        str[len] = '\0';                                                                // adds null terminator
        for (int i = 0; i < len; i++) {                                                 // iterates the string
            if (isdigit(str[i]) == 0) {                                                 // if a character is not a digit
                fprintf(stderr, "File format not valid\n");                             // error
                return -1;                                                              // returns -1
            }
        }
        int qubits = (int)strtol(str, NULL, 10);                                        // converts string to integer   
        return qubits;                                                                  // returns the number of qubits
    }
    else {                                                                              // if the line doesn't start with #qubits
        fprintf(stderr, "File format not valid\n");                                     // error
        return -1;                                                                      // returns -1 if the line doesn't start with #qubits
    };                                                                                   
}

complex get_complex(char* parse) {
    complex c = {0.0, 0.0};                                                         // initializes complex number
    char *temp1 = malloc(100 * sizeof(char));                                       // temporary array for the real part
    char *temp2 = malloc(100 * sizeof(char));                                       // temporary array for the imaginary part
    if (temp1 == NULL || temp2 == NULL) {                                           // checks if memory allocation was successful
        fprintf(stderr, "Memory allocation failed\n");                              // error
        exit(EXIT_FAILURE);                                                         // exits the program
    }
    int i = 0;                                                                      // index of the real part
    int j = 0;                                                                      // index of the imaginary part
    int iter = 0;                                                                   // iterator for the string
    char *copy = strdup(parse);                                                     // copies the string
    int count = 0;                                                                  // counter of values
    char *p;                                                                        // pointer
    char *token = strtok_r(parse, "+-", &p);                                        // splits the string
    char *tokens[100];                                                              // array of tokens
    while (token != NULL) {                                                         // while there are tokens
        tokens[count] = token;                                                      // adds token to the array
        count++;                                                                    // increases count  
        token = strtok_r(NULL, "+-", &p);                                           // gets the next token
    }
    if (count == 2) {                                                               // if there are two tokens
        int length = 0;                                                             // length of the real part
        int check = 0;                                                              // checks if there are letters
        for (int a = 0; a < strlen(tokens[0]); a++) {                               // iterates the first token
            if (isalpha(tokens[0][a])) {                                            // if the character is a letter
                check = 1;                                                          // sets check to 1
            } 
        }
        if (check == 1) {                                                           // if the are letters
            fprintf(stderr, "File format not valid1\n");                            // error
            free(temp1);                                                            // frees memory
            free(temp2);                                                            // frees memory
            return c;                                                               // returns complex number
        }   
        else {                                                                      // else
            if (copy[0] == '+' || copy[0] == '-') {                                 // if the first character is + or -
                temp1[0] = copy[0];                                                 // adds the first character to the temporary array
                i++;                                                                // increases index
                iter++;                                                             // increases iter
            } 
            for (int t1 = 0; t1 < strlen(tokens[0]); t1++) {                        // iterates the first token
                temp1[i] = tokens[0][t1];                                           // adds character to the temporary array
                i++;                                                                // increases index
                length++;                                                           // increases length
            }
            temp1[i] = '\0';                                                        // adds null terminator
            c.real = strtod(temp1, NULL);                                           // converts string to double
            int index = iter + length;                                              // index of the imaginary part's sign
            temp2[j] = copy[index];                                                 // adds the first character of the imaginary part to the temporary array
            j++;                                                                    // increases index
            if (strchr(tokens[1], 'i') != NULL) {                                   // if the second token contains i
                int len = 0;                                                        // length of the imaginary part
                for (int c = 0; c < strlen(tokens[1]); c++) {                       // iterates the second token
                    len++;                                                          // increases length
                }
                if (len == 1 && tokens[1][0] == 'i') {                              // if the imaginary part is just i
                    temp2[j] = '1';                                                 // sets imaginary part to 1
                    c.imag = strtod(temp2, NULL);                                   // converts to double
                    free(temp1);                                                    // frees memory
                    free(temp2);                                                    // frees memory
                    return c;                                                       // returns complex number
                }
                else {                                                              // else
                    for (int t2 = 0; t2 < strlen(tokens[1]); t2++) {                // iterates the second token
                        if (tokens[1][t2] == 'i') {                                 // if the character is i
                            continue;                                               // skips i
                        }
                        temp2[j] = tokens[1][t2];                                   // adds character to the temporary array
                        j++;                                                        // increases index
                    }
                    temp2[j] = '\0';                                                // adds null terminator
                    c.imag = strtod(temp2, NULL);                                   // converts string to double
                    free(temp1);                                                    // frees memory
                    free(temp2);                                                    // frees memory
                    return c;                                                       // returns complex number
                }
            }
            else {                                                                  // else
                fprintf(stderr, "File format not valid2\n");                        // error 
                free(temp1);                                                        // frees memory
                free(temp2);                                                        // frees memory
                return c;                                                           // returns complex number
            }
        }                                                                                    
    }
    else if (count == 1) {                                                          // if there is only one token
        char *t = tokens[0];                                                        // assigns the first token to t
        int try =  0;                                                               // checks if the first character is a letter
        for (int b = 0; b < strlen(t); b++) {                                       // iterates the first token
            if (isalpha(t[b])) {                                                    // if there are letters
                try = 1;                                                            // sets try to 1
            }
        }
        if (try == 0) {                                                             // if there are no letters
            if (copy[0] == '+' || copy[0] == '-') {                                 // if the first character is + or -
                temp1[0] = copy[0];                                                 // adds the first character to the temporary array
                i++;                                                                // increases index 
            }
            for (int t1 = 0; t1 < strlen(t); t1++) {                                // iterates the string
                temp1[i] = t[t1];                                                   // adds character to the temporary array
                i++;                                                                // increases index
            }
            temp1[i] = '\0';                                                        // adds null terminator
            c.real = strtod(temp1, NULL);                                           // converts string to double
            c.imag = 0.0;                                                           // sets imaginary part to 0
            free(temp1);                                                            // frees memory
            free(temp2);                                                            // frees memory
            return c;                                                               // returns complex number
        }
        else {                                                                      // if there are letters
            if (strchr(t, 'i') != NULL) {                                           // if the string contains i
                if (copy[0] == '+' || copy[0] == '-') {                             // if the first character is + or -
                    temp2[0] = copy[0];                                             // adds the first character to the temporary array
                    j++;                                                            // increases index  
                }
                int len = 0;                                                        // length of the imaginary part
                for (int c = 0; c < strlen(t); c++) {                               // iterates the string   
                    len++;                                                          // increases length
                }
                if (len == 1 && t[0] == 'i') {                                      // if the imaginary part is just i
                    temp2[j] = '1';                                                 // sets imaginary part to 1
                    c.imag = strtod(temp2, NULL);                                   // sets imaginary part to 1
                    free(temp1);                                                    // frees memory
                    free(temp2);                                                    // frees memory
                    return c;                                                       // returns complex number
                }
                else {                                                              // else
                    for (int t2 = 0; t2 < strlen(t); t2++) {                        // iterates the string
                        if (t[t2] == 'i') {                                         // if the character is i
                            continue;                                               // skips i
                        }
                        temp2[j] = t[t2];                                           // adds character to the temporary array
                        j++;                                                        // increases index
                    }
                    temp2[j] = '\0';                                                // adds null terminator
                    c.real = 0.0;                                                   // sets real part to 0
                    c.imag = strtod(temp2, NULL);                                   // converts string to double
                    free(temp1);                                                    // frees memory
                    free(temp2);                                                    // frees memory
                    return c;                                                       // returns complex number
                }
            }
            else {                                                                  // if the string doesn't contain i
                fprintf(stderr, "File format not valid3\n");                        // error 
                free(temp1);                                                        // frees memory
                free(temp2);                                                        // frees memory
                return c;                                                           // returns complex number
            }
        }
    }
    else {                                                                          // if there are more than two tokens
        fprintf(stderr, "File format not valid4\n");                                // error
        free(temp1);                                                                // frees memory
        free(temp2);                                                                // frees memory
        return c;                                                                   // returns complex number
    }
    free(copy);                                                                     // frees memory      
    free(temp1);                                                                    // frees memory
    free(temp2);                                                                    // frees memory
    return c;                                                                       // returns complex number
}

vector get_vin(char* lines, int qubits, vector vin) {                                   // gets the input vector
    int index = 0;                                                                      // index of the input vector
    int start = 0;                                                                      // first character 
    while (start < strlen(lines)) {                                                     // while start is less than the length 
        if (strncmp(&lines[start], "#init", 5) == 0) {                                  // if the line starts with #init 
            start += 5;                                                                 // increases start by 5 
            while (lines[start] == '[') {                                               // skips spaces and tabs 
                start++;                                                                // moves to the next character 
            }
            int end = start + 1;                                                        // starts from the next character 
            while (lines[end] != '\n' && lines[end] != '\0' && lines[end] != ']') {     // skips spaces and tabs 
                end++;                                                                  // moves to the next character 
            } 
            int len = end - start;                                                      // length of the string
            char *str = malloc(len + 1);                                                // array 
            if (str == NULL) {                                                          // checks if memory allocation was successful
                fprintf(stderr, "Memory allocation failed\n");                          // error
                exit(EXIT_FAILURE);                                                     // exits the program
            }
            strncpy(str, &lines[start], len);                                           // copies the string 
            str[len] = '\0';                                                            // adds null terminator 
            char **tokens = malloc((int)pow(2, qubits) * sizeof(char*));                // array of tokens
            if (tokens == NULL) {                                                       // checks if memory allocation was successful
                fprintf(stderr, "Memory allocation failed\n");                          // error
                free(str);                                                              // frees memory for the string
                exit(EXIT_FAILURE);                                                     // exits the program
            }
            int count = 0;                                                              // counter of values 
            char *p;                                                                    // pointer 
            char *token = strtok_r(str, ",", &p);                                       // splits the string 
            while (token != NULL && count < (int)pow(2, qubits)) {                      // while there are tokens 
                tokens[count] = malloc(strlen(token) + 1);                              // allocates memory for the token    
                if (!tokens[count]) {                                                   // checks if memory allocation was successful
                    fprintf(stderr, "malloc failed for token\n");                       // error
                    for (int k = 0; k < count; k++) {                                   // frees memory for the tokens   
                        free(tokens[k]);                                                // frees memory for the tokens  
                    }
                    free(tokens);                                                       // frees memory for the tokens
                    free(str);                                                          // frees memory for the string                 
                    exit(EXIT_FAILURE);                                                 // exits the program
                }
                strcpy(tokens[count], token);                                           // copies the token to the array
                count++;                                                                // increases count
                token = strtok_r(NULL, ",", &p);                                        // gets the next token
            }
            if (count != (int)pow(2, qubits)) {                                         // if values is not equal to 2^qubits 
                fprintf(stderr, "Input vector not valid");                              // error 
                for (int i = 0; i < count; i++) {                                       // frees memory for the tokens
                    free(tokens[i]);                                                    // frees memory for the tokens
                }
                free(tokens);                                                           // frees memory for the tokens
                free(str);                                                              // frees memory for the string
                exit(EXIT_FAILURE);                                                     // exits the program
            }
            vin.length = count;                                                         // sets the length of the input vector
            for (int i = 0; i < count; i++) {                                           // iterates the tokens
                complex value = get_complex(tokens[i]);                                 // gets the complex number from the token
                vin.values[index++] = value;                                            // adds the complex number to the input vector
                free(tokens[i]);                                                        // frees memory for the token
            } 
            free(tokens);                                                               // frees memory for the tokens
            free(str);                                                                  // frees memory for the string
            break;                                                                      // breaks the loop
        }
        while (lines[start] != '\n' && lines[start] != '\0') {                          // skips spaces and tabs 
            start++;                                                                    // moves to the next character 
        }
        if (lines[start] == '\n') {                                                     // if the line ends 
            start++;                                                                    // moves to the next character 
        }
    }
    return vin;                                                                         // returns the input vector 
}

void get_order(char* lines, char** order) {                                              // gets the order of the circuit
    int index = 0;                                                                      // index of the lines
    while (lines[index] != '\0') {                                                      // while the lines doesn't end
        char *l = &lines[index];                                                        // pointer to the current line
        if (strncmp(l, "#circ", 5) == 0) {                                              // if the line starts with #circ
            int start = 5;                                                              // starts from the 6th character 
            while (l[start] == ' ' || l[start] == '\t') {
                start++;
            } 
            int end = start + 1;                                                        // starts from the next character
            while (l[end] != '\n' && l[end] != '\0') {                                  // skips spaces and tabs
                end++;                                                                  // moves to the next character
            }
            int len = end - start;                                                      // length of the string
            char str[1024];                                                             // array
            strncpy(str, &l[start], len);                                               // copies the string
            str[len] = '\0';                                                            // adds null terminator
            char *p;                                                                    // pointer 
            int i = 0;
            char *token = strtok_r(str, " \t", &p);                                     // splits the string 
            while (token != NULL) {                                                     // while there are tokens 
                order[i] = strdup(token);                                               // adds token to the arder
                i++;                                                                    // increases index
                token = strtok_r(NULL, " \t", &p);                                      // gets the next token 
            }
            order[i] = NULL;  
            break; 
        }
        while (lines[index] != '\n' && lines[index] != '\0') {                          // skips spaces and tabs    
            index++;                                                                    // moves to the next character
        }
        if (lines[index] == '\n') {                                                     // if the line ends   
            index++;                                                                    // moves to the next character
        }
    }
}

char* matrix_name(char *line) {                                                         // gets the name of the matrix
    int i = 7;                                                                          // starts from the 8th character
    while (line[i] == ' ' || line[i] == '\t') {                                         // skips spaces and tabs
        i++;                                                                            // moves to the next character
    }
    int start = i;                                                                      // starts from the current character
    while (line[i] != '[' && line[i] != '\0') {                                         // skips spaces and tabs
        i++;                                                                            // moves to the next character
    }
    int len = i - start;                                                                // length of the string
    char* name = malloc(len + 1);                                                       // allocates memory for the name
    for (int j = 0; j < len; j++) {                                                     // iterates the string
        name[j] = line[start + j];                                                      // adds character to the name
    }
    name[len] = '\0';                                                                   // adds null terminator
    return name;                                                                        // returns the name
}

char* get_matrix_str(char* line) {                                                      // gets the matrix string
    char* start = NULL;                                                                 // pointer to the start of the matrix string
    char* end = NULL;                                                                   // pointer to the end of the matrix string
    for (int i = 0; line[i] != '\0'; i++) {                                             // iterates the line
        if (line[i] == '[' && start == NULL) {                                          // if the character is [ and start is NULL
            start = &line[i + 1];                                                       // sets start to the next character
        }
        if (line[i] == ']') {                                                           // if the character is ]
            end = &line[i];                                                             // sets end to the current character
        }
    }
    if (start == NULL || end == NULL || end <= start) {                                 // if start or end is NULL or end is before start
        return NULL;                                                                    // returns NULL
    }
    int len = end - start;                                                              // length of the matrix string
    char* out = malloc(len + 1);                                                        // allocates memory for the output string
    for (int i = 0; i < len; i++) {                                                     // iterates the string
        out[i] = start[i];                                                              // adds character to the output string
    }
    out[len] = '\0';                                                                    // adds null terminator
    return out;                                                                         // returns the output string
}

int num_rows(char* matrix_str) {                                                        // counts the number of rows in the matrix string
    int count = 0;                                                                      // counter for the number of rows
    for (int i = 0; matrix_str[i] != '\0'; i++) {                                       // iterates the matrix string
        if (matrix_str[i] == '(') {                                                     // if the character is (
            count++;                                                                    // increases the counter
        }
    }
    return count;                                                                       // returns the number of rows
}

int num_columns(char* row) {                                                            // counts the number of columns in a row
    int count = 0;                                                                      // counter for the number of columns
    char* temp = strdup(row);                                                           // creates a copy of the row string
    char* saveptr = NULL;                                                               // pointer for strtok_r
    char* token = strtok_r(temp, ",", &saveptr);                                        // splits the row string by commas
    while (token != NULL) {                                                             // while there are tokens
        count++;                                                                        // increases the counter
        token = strtok_r(NULL, ",", &saveptr);                                          // gets the next token
    }
    free(temp);                                                                         // frees the temporary string
    return count;                                                                       // returns the number of columns
}

vector parse_row(char* row_str, int cols) {                                             // parses a row string into a vector
    vector v;                                                                           // initializes a vector
    v.length = cols;                                                                    // sets the length of the vector
    v.values = malloc(cols * sizeof(complex));                                          // allocates memory for the values
    while (*row_str == ' ' || *row_str == '\t' || *row_str == '(') {                    // skips spaces, tabs, and (
        row_str++;                                                                      // moves to the next character
    }
    char* saveptr = NULL;                                                               // pointer for strtok_r
    char* token = strtok_r(row_str, ",", &saveptr);                                     // splits the row string by commas
    int index = 0;                                                                      // index for the vector values
    while (token != NULL && index < cols) {                                             // while there are tokens and index is less than the columns
        while (*token == ' ' || *token == '\t') {                                       // skips spaces and tabs
            token++;                                                                    // moves to the next character
        }
        v.values[index] = get_complex(token);                                           // gets the complex number from the token
        index++;                                                                        // increases the index
        token = strtok_r(NULL, ",", &saveptr);                                          // gets the next token
    }
    return v;                                                                           // returns the vector
}

circuit get_matrices(char* lines, int qubits, char** order, circuit all_circ, matrix m, vector row) {   // gets the matrices
    int num_order = 0;                                                                  // number of matrices
    while (order[num_order] != NULL) {                                                  // counts the matrices
        num_order++;                                                                    // increments nem_order
    }
    char** a_names= malloc(num_order * sizeof(char*));                                  // allocates memory for unique matrix names
    matrix* mats = malloc(num_order * sizeof(matrix));                                  // array
    int counter = 0;                                                               
    char* lines_copy = strdup(lines);
    char* ptr_lines = NULL;
    char* line = strtok_r(lines_copy, "\n", &ptr_lines);
    while (line != NULL) {
        if (strncmp(line, "#define", 7) == 0) {
            char* name = matrix_name(line);
            char* matrix_str = get_matrix_str(line);
            if (name == NULL || matrix_str == NULL) {
                free(name);
                free(matrix_str);
                line = strtok_r(NULL, "\n", &ptr_lines);
                continue;
            }
            int found_index = -1;
            for (int i = 0; i < counter; i++) {
                if (strcmp(a_names[i], name) == 0) {
                    found_index = i;
                    break;
                }
            }
            if (found_index == -1) {
                int rows = num_rows(matrix_str);
                matrix mat;
                mat.n_rows = rows;
                mat.rows = malloc(rows * sizeof(vector));
                int r = 0;
                char* ptr_row = NULL;
                char* row_str = strtok_r(matrix_str, ")", &ptr_row);
                while (row_str != NULL && r < rows) {
                    int cols = num_columns(row_str);
                    vector v = parse_row(row_str, cols);
                    mat.rows[r++] = v;
                    row_str = strtok_r(NULL, ")", &ptr_row);
                }
                a_names[counter] = strdup(name);
                mats[counter] = mat;
                counter++;
            }
            free(name);
            free(matrix_str);
        }
        line = strtok_r(NULL, "\n", &ptr_lines);
    }
    free(lines_copy);
    all_circ.cir = malloc(num_order * sizeof(matrix));
    all_circ.n = num_order;
    for (int i = 0; i < num_order; i++) {
        int idx = -1;
        for (int j = 0; j < counter; j++) {
            if (strcmp(order[i], a_names[j]) == 0) {
                idx = j;
                break;
            }
        }
        if (idx == -1) {
            fprintf(stderr, "Matrix %s not found in definitions!\n", order[i]);
            all_circ.cir[i].n_rows = 0;
            all_circ.cir[i].rows = NULL;
            continue;
        }
        matrix* src = &mats[idx];
        int rows = src->n_rows;
        all_circ.cir[i].n_rows = rows;
        all_circ.cir[i].rows = malloc(rows * sizeof(vector));
        for (int r = 0; r < rows; r++) {
            int len = src->rows[r].length; 
            vector v;
            v.length = len;
            v.values = malloc(len * sizeof(complex));
            for (int c = 0; c < len; c++) {
                v.values[c] = src->rows[r].values[c];
            }
            all_circ.cir[i].rows[r] = v;                                                // assigns v
        }
    }
    for (int i = 0; i < counter; i++) {                                                 // frees memory for unique matrices
        free(a_names[i]);                                                               // frees memory for a_names
        for (int r = 0; r < mats[i].n_rows; r++) {                                      // iterates rows
            free(mats[i].rows[r].values);                                               // frees memory for values
        }
        free(mats[i].rows);                                                             // frees memory for rows
    }
    free(a_names);
    free(mats);                                                                         // frees memory for unique_mats
    return all_circ;                                                                    // returns the circuit
}

void print_c(circuit all_circ, char** order, int qubits) {                              // print all the matrices
    int num_order = 0;
    while (order[num_order] != NULL) {
        num_order++;
    }
    for (int i = 0; i < num_order; i++) {
        for (int j = 0; j < (int)pow(2, qubits); j++) {
            for (int k = 0; k < (int)pow(2, qubits); k++) {
                if (all_circ.cir[i].rows[j].values[k].imag < 0) {                      
                    printf("%lf - %lfi ", all_circ.cir[i].rows[j].values[k].real, -all_circ.cir[i].rows[j].values[k].imag); 
                } else {                                                                
                    printf("%lf + %lfi ", all_circ.cir[i].rows[j].values[k].real, all_circ.cir[i].rows[j].values[k].imag); 
                }
            }
            printf("\n");                                                              
        }
        printf("\n");   
    }
}
