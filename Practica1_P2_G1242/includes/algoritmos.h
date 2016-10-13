#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <gmp.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

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

void determinante(mpz_t** matrix, int n, mpz_t det, mpz_t m);
void matrixTransposed(mpz_t** matrix, int n, mpz_t** matrixTrans);
void matrixCofactors(mpz_t** matrix, int n, mpz_t** matrixCof, mpz_t m);
void matrixAdjoint(mpz_t** matrix, int n, mpz_t** matrixAdj, mpz_t m);

mpz_t** init_mpz_matrix(mpz_t*** mat, int rows, int cols);
void free_mpz_matrix(mpz_t** mat, int rows, int cols);

