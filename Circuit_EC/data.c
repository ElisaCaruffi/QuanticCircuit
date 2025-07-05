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
    printf("Parsing complex number: %s\n", copy);                                  // prints the complex number being parsed
    int count = 0;                                                                  // counter of values
    char *token = strtok(parse, "+-");                                              // splits the string
    char *tokens[100];                                                              // array of tokens
    while (token != NULL) {                                                         // while there are tokens
        tokens[count] = token;                                                      // adds token to the array
        count++;                                                                    // increases count  
        token = strtok(NULL, "+-");                                                 // gets the next token
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
                    char* t;
                    t = strdup(temp2);
                    printf("Imaginary part: %s\n", t);                          // prints the real and imaginary parts
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
            char *token = strtok(str, ",");                                             // splits the string 
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
                token = strtok(NULL, ",");                                              // gets the next token
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
            int i = 0;
            char *token = strtok(str, " \t");                                           // splits the string 
            while (token != NULL) {                                                     // while there are tokens 
                order[i] = strdup(token);                                               // adds token to the arder
                i++;                                                                    // increases index
                token = strtok(NULL, " \t");                                            // gets the next token 
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
    char* token = strtok(temp, ",");                                                    // splits the row string by commas
    while (token != NULL) {                                                             // while there are tokens
        count++;                                                                        // increases the counter
        token = strtok(NULL, ",");                                                      // gets the next token
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
    char** little_c = malloc(cols * sizeof(char*));                                 // allocates memory for the row values                                   
    char* token = strtok(row_str, ",");                                     // splits the row string by commas
    int little_counter = 0;
    while (token != NULL) {                                          // while there are tokens
        little_c[little_counter] = malloc(strlen(token) + 1);                     // allocates memory for the row value
        strcpy(little_c[little_counter], token);                                                            // assigns the first token to the array
        token = strtok(NULL, ",");                                                      // gets the next token
        little_counter++;
    }
    int index = 0;                                                                      // index for the vector values
    for (index = 0; index<cols; index++) {                                             // while there are tokens and index is less than the columns
        v.values[index] = get_complex(little_c[index]);                                           // gets the complex number from the token
    }
    for (int i = 0; i < cols; i++) {                                           // frees memory for the row values
        free(little_c[i]);                                                              // frees memory for the row value
    }
    free(little_c);                                                                    // frees memory for the array of row values
    return v;                                                                           // returns the vector
}

circuit get_matrices(char* lines, int qubits, char** order, circuit all_circ, matrix m, vector row) {   // gets the matrices
    int num_order = 0;                                                                  // number of matrices
    while (order[num_order] != NULL) {                                                  // counts the matrices
        num_order++;                                                                    // increments nem_order
    }
    char** a_names= malloc(num_order * sizeof(char*));                                  // allocates memory for matrix names
    matrix* mats = malloc(num_order * sizeof(matrix));                                  // array
    int counter = 0;                                                                    // counter for the number of matrices
    char* lines_copy = strdup(lines);                                                   // creates a copy of the lines
    int height = 0;
    for (int h = 0; h < strlen(lines_copy); h++) {
        if (lines_copy[h] == '#') {
            height++;                                                                   // counts the number of lines starting with #
        }
    }
    char** all = malloc(height*sizeof(char*));                                    // allocates memory for all the lines
    char* line = strtok(lines_copy, "\n");                                      // splits the lines by newlines
    int dx = 0;
    while (line != NULL) {                                                              // while there are lines
        all[dx] = malloc(strlen(line)+1);                                                    // adds the line to the array
        strcpy(all[dx], line);                                                          // copies the line to the array
        dx++;                                                                      // increases the counter
        line = strtok(NULL, "\n");                                              // gets the next line
    }
    for (int b = 0; b<height; b++) {                                                              // while there are lines
        if (strncmp(all[b], "#define", 7) == 0) {                                         // if the line starts with #define
            char* name = matrix_name(all[b]);                                             // gets the name of the matrix
            char* m_str = get_matrix_str(all[b]);                                         // gets the matrix string
            if (name == NULL || m_str == NULL) {                                        // if name or matrix_str is NULL
                free(name);                                                             // frees memory for name 
                free(m_str);                                                            // frees memory for matrix_str
                continue;                                                               // continues to the next iteration
            }
            int j = -1;                                                                 // index of the matrix in the matrices
            for (int i = 0; i < counter; i++) {                                         // iterates through the matrices
                if (strcmp(a_names[i], name) == 0) {                                    // if the name matches
                    j = i;                                                              // sets the index
                    break;                                                              // breaks the loop
                }
            }
            if (j == -1) {                                                              // if the matrix is not found
                int rows = num_rows(m_str);                                             // gets the number of rows
                matrix mat;                                                             // initializes a matrix
                mat.n_rows = rows;                                                      // sets the number of rows
                mat.rows = malloc(rows * sizeof(vector));                               // allocates memory for the rows
                int r = 0;                                                              // row index
                char** rr = malloc(rows * sizeof(char*));                                 // array of row strings
                char* row_str = strtok(m_str, ")");                         // splits the matrix string by )
                while (row_str != NULL && r < rows) {                                   // while there are rows and r is less than the number of rows
                    rr[r] = malloc(strlen(row_str) + 1);                        // allocates memory for the row string
                    strcpy(rr[r], row_str);                                             // copies the row string
                    row_str = strtok(NULL, ")");                            // gets the next row string
                    r++;
                }
                for (int l = 0; l<rows; l++) {
                    int cols = num_columns(rr[l]);                                    // gets the number of columns in the row
                    vector v = parse_row(rr[l], cols);                                // parses the row string into a vector
                    /*
                    for (int i = 0; i < (int)pow(2, qubits); i++) {
                        if (v.values[i].imag < 0)
                            printf("%0.5lf - i%0.5lf", v.values[i].real, -v.values[i].imag);
                        else
                            printf("%0.5lf + i%0.5lf", v.values[i].real, v.values[i].imag);

                        if (i != (int)pow(2, qubits) - 1)
                            printf(", ");
                    }
                    printf("\n");
                    */
                    mat.rows[l] = v;                                                  // assigns the vector to the matrix row
                }
                a_names[counter] = strdup(name);                                        // adds the name to the matrix names
                mats[counter] = mat;                                                    // assigns the matrix
                counter++;                                                              // increments the counter
                for (int w = 0; w<rows;w++) {
                    free(rr[w]);                                                      // frees memory for the row strings
                }          
                free(rr);                                                           // frees memory for the array of row strings
            }
            free(name);                                                                 // frees memory for name
            free(m_str);                                                      // frees memory for matrix_str
        }
    }
    for (int z =0; z<height; z++) {
            free(all[z]);
        }
        free(all);
    free(lines_copy);                                                                   // frees memory for the lines copy
    all_circ.cir = malloc(num_order * sizeof(matrix));                                  // allocates memory for the circuit matrices
    all_circ.n = num_order;                                                             // sets the number of matrices in the circuit
    for (int i = 0; i < num_order; i++) {                                               // iterates through the order
        int k = -1;                                                                     // index of the matrix
        for (int j = 0; j < counter; j++) {                                             // iterates through the matrices
            if (strcmp(order[i], a_names[j]) == 0) {                                    // if the name matches
                k = j;                                                                  // sets the index
                break;                                                                  // breaks the loop
            }
        }
        if (k == -1) {                                                                  // if the matrix is not found
            fprintf(stderr, "Matrix %s not found in definitions!\n", order[i]);         // error
            all_circ.cir[i].n_rows = 0;                                                 // sets the number of rows to 0
            all_circ.cir[i].rows = NULL;                                                // sets the rows to NULL
            continue;                                                                   // continues to the next iteration
        }
        matrix* imp = &mats[k];                                                         // gets the source matrix
        int rows = imp->n_rows;                                                         // gets the number of rows
        all_circ.cir[i].n_rows = rows;                                                  // sets the number of rows in the circuit matrix   
        all_circ.cir[i].rows = malloc(rows * sizeof(vector));                           // allocates memory for the rows in the circuit matrix
        for (int r = 0; r < rows; r++) {                                                // iterates through the rows
            int len = imp -> rows[r].length;                                            // gets the length of the row
            vector v;                                                                   // initializes a vector
            v.length = len;                                                             // sets the length of the vector
            v.values = malloc(len * sizeof(complex));                                   // allocates memory for the values in the vector
            for (int c = 0; c < len; c++) {                                             // iterates through the columns
                v.values[c] = imp -> rows[r].values[c];                                 // assigns the value to the vector
            }
            all_circ.cir[i].rows[r] = v;                                                // assigns v
        }
    }
    for (int i = 0; i < counter; i++) {                                                 // frees memory for matrices
        free(a_names[i]);                                                               // frees memory for a_names
        for (int r = 0; r < mats[i].n_rows; r++) {                                      // iterates rows
            free(mats[i].rows[r].values);                                               // frees memory for values
        }
        free(mats[i].rows);                                                             // frees memory for rows
    }
    free(a_names);
    free(mats);                                                                         // frees memory for mats
    return all_circ;                                                                    // returns the circuit
}