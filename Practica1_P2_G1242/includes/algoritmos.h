#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <gmp.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR 128

enum Modo {CIFRAR, DESCIFRAR};

extern char *optarg;
extern int optind, opterr, optopt;

void getGDC(mpz_t a, mpz_t b, mpz_t gdc);
int getMultInverse(mpz_t a, mpz_t b, mpz_t inverse);
void toUpperOnly(char* src);
void determinante(mpz_t** matrix, int n, mpz_t det);
