#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <gmp.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STR 128

enum Modo {
    CIFRAR, DESCIFRAR
};

extern char *optarg;
extern int optind, opterr, optopt;

void toUpperOnly(char* src);

void getGDC(mpz_t a, mpz_t b, mpz_t gdc);
int getMultInverse(mpz_t a, mpz_t b, mpz_t inverse);
void toModM(mpz_t x, mpz_t m);
//Arguments will be read as ints, last argument *must* be 0
double average(int how_many, double* vals);

//will count patter overlaps (say 'ababa' as two 'aba')
//it could be considered a lazy matcher in an untraditional sense
char* most_common_ngram(int ngram_length, char* string);

//mpz matrices functions
void determinante(mpz_t** matrix, int n, mpz_t det, mpz_t m);
void matrixTransposed(mpz_t** matrix, int n, mpz_t** matrixTrans, mpz_t m);
void matrixCofactors(mpz_t** matrix, int n, mpz_t** matrixCof, mpz_t m);
void matrixAdjoint(mpz_t** matrix, int n, mpz_t** matrixAdj, mpz_t m);
void mulMatrixConst(mpz_t cons, mpz_t** matrix, mpz_t** matrixRes, int n);
int matrixInverse(mpz_t** matrix, int n, mpz_t m, mpz_t** matrixInv);
void mulMatrixMatrix(mpz_t** matrix1, mpz_t** matrix2, mpz_t** matrixRes, int n, mpz_t m);

mpz_t** init_mpz_matrix(mpz_t*** mat, int rows, int cols);
void free_mpz_matrix(mpz_t** mat, int rows, int cols);
