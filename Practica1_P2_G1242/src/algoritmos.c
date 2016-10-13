#include "../includes/algoritmos.h"

void getGDC(mpz_t a, mpz_t b, mpz_t gdc) {

    mpz_t max;
    mpz_t min;
    mpz_t r;
    mpz_t q;

    mpz_inits(max, min, q, NULL);
    mpz_init_set_ui(r, 2L);
    //gmp_printf ("a es %Zd y b es %Zd\n", a, b);
    //tell which one is the divident and the divisor
    if (mpz_cmp(a, b) > 0) {
        mpz_set(max, a);
        mpz_set(min, b);
    } else {
        mpz_set(max, b);
        mpz_set(min, a);
    }

    while (1) {
        mpz_fdiv_qr(q, r, max, min);
        //gmp_printf ("Q es %Zd y R es %Zd\n", gdc, r);
        if (mpz_sgn(r) == 0)
            break;
        mpz_set(max, min);
        mpz_set(min, r);
    }
    mpz_set(gdc, min);

    mpz_clears(max, min, r, q, NULL);
}

int getMultInverse(mpz_t a, mpz_t b, mpz_t inverse) {

    mpz_t gdc;
    mpz_init(gdc);

    getGDC(a, b, gdc);

    if (mpz_cmp_ui(gdc, 1L)) {
        mpz_clear(gdc);
        return -1;
    }

    mpz_t max;
    mpz_t min;
    mpz_inits(max, min, NULL);

    if (mpz_cmp(a, b) > 0) {
        mpz_set(max, a);
        mpz_set(min, b);
    } else {
        mpz_set(max, b);
        mpz_set(min, a);
    }

    // Casos especiales
    if (!mpz_cmp_ui(min, 0L) || !mpz_cmp_ui(min, 1L)) {
        mpz_set(inverse, min);
        return 0;
    }

    mpz_t x;
    mpz_t y;
    mpz_t x1;
    mpz_t y1;
    mpz_t x2;
    mpz_t y2;
    mpz_t q;
    mpz_t r;
    mpz_t ret;
    mpz_t maxAux;
    mpz_inits(x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);

    mpz_set(maxAux, max);
    mpz_set_ui(x2, 1L);
    mpz_set_ui(y1, 1L);

    while (mpz_cmp_ui(r, 1L)) {
        // max = q*min + r
        mpz_fdiv_qr(q, r, max, min);

        // x2 - q*x1
        mpz_mul(ret, q, x1);
        mpz_sub(x, x2, ret);

        // y2 - q*y1
        mpz_mul(ret, q, y1);
        mpz_sub(y, y2, ret);

        // Reajustar valores
        mpz_set(max, min);
        mpz_set(min, r);
        mpz_set(x2, x1);
        mpz_set(x1, x);
        mpz_set(y2, y1);
        mpz_set(y1, y);
    }

    if (mpz_cmp_ui(y, 0L) < 0) {
        mpz_add(ret, maxAux, y);
        mpz_set(inverse, ret);

    } else {
        mpz_set(inverse, y);
    }

    mpz_clears(gdc, max, min, x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);
    return 0;
}

void toUpperOnly(char* src) {
    int i, j;
    for (i = 0, j = 0; src[i] != '\0'; i++) {
        if (isalpha(src[i])) {
            src[j] = (char) (toupper(src[i]));
            j++;
        }
    }
    src[j] = '\0';
}

void toModM(mpz_t x, mpz_t m) {
	mpz_t q;
	mpz_init(q);
	
	mpz_tdiv_q(q, x, m);
	
	if (mpz_sgn(x) < 0) {
		//|x| < m
		if (mpz_sgn(q) == 0) {
			mpz_add(x, x, m);
		} else {
			mpz_abs(q, q);
			mpz_add_ui(q, q, 1L);
			mpz_mul(q, m, q);
			mpz_add(x, x, q);
		}
	} else if (mpz_cmp(x, m) >= 0) {
		mpz_mul(q, m, q);
		mpz_sub(x, x, q);
	}
	//else x is already % m
	
	mpz_clear(q);
}

void determinante(mpz_t** matrix, int n, mpz_t det, mpz_t m) {

    int i;
    int j;
    int k;
    mpz_t result;
    mpz_t suma;

    if (n == 2) {
        mpz_t r1, r2;
        mpz_inits(r1, r2, NULL);
        mpz_mul(r1, matrix[0][0], matrix[1][1]);
        mpz_mul(r2, matrix[1][0], matrix[0][1]);
        mpz_sub(det, r1, r2);
        toModM(det, m);
        mpz_clears(r1, r2, NULL);
        return;
    }

    mpz_t **nm;

    init_mpz_matrix(&nm, n-1, n-1);
    if (nm == NULL) {
		return;
	}

	mpz_inits(result, suma, NULL);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
        	if (j != i) {
                for (k = 1; k < n; k++) {
                    int index = -1;

                    if (j < i) {
                        index = j;
                    } else if (j > i) {
                        index = j - 1;
                    }
                    mpz_set(nm[index][k - 1], matrix[j][k]);
                }
            }
        }
        
		determinante(nm, (n - 1), result, m);
            
        if (i % 2 == 0) {
			mpz_mul(result, matrix[i][0], result);
            mpz_add(suma, suma, result);

        } else {
        	mpz_mul(result, matrix[i][0], result);
            mpz_sub(suma, suma, result);
        }

        toModM(suma, m);
    }

    mpz_set(det, suma);

    mpz_clears(result, suma, NULL);
    free_mpz_matrix(nm, n-1, n-1);

	return;
}

void matrixTransposed(mpz_t** matrix, int n, mpz_t** matrixTrans) {

	int i;
	int j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_set(matrixTrans[i][j], matrix[j][i]);
		}
	}
}

void matrixCofactors(mpz_t** matrix, int n, mpz_t** matrixCof, mpz_t m) {

	int i;
	int j;
	int k;
	int l;

	mpz_t** det;
	mpz_t valueDet;
	
	init_mpz_matrix(&det, n-1, n-1);
	if (det == NULL) {
		return;
	}
	
	mpz_init(valueDet);
	
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (k = 0; k < n; k++) {
				if (k != i) {
					for (l = 0; l < n; l++) {
						if (l != j) {
							int index1 = k < i ? k : k-1;
							int index2 = l < j ? l : l-1;
							mpz_set(det[index1][index2], matrix[k][l]);
						}
					}
				}
			}
			determinante(det, n-1, valueDet, m);
			if (((i+j) % 2) == 1) {
				mpz_neg(valueDet, valueDet);
				toModM(valueDet, m);
			}
			mpz_set(matrixCof[i][j], valueDet);
		}
	}
	
	free_mpz_matrix(det, n-1, n-1);
	mpz_clear(valueDet);
}

void matrixAdjoint(mpz_t** matrix, int n, mpz_t** matrixAdj, mpz_t m) {

	matrixCofactors(matrix, n, matrixAdj, m);
	matrixTransposed(matrix, n, matrixAdj);

	return;
}

mpz_t** init_mpz_matrix(mpz_t*** mat, int rows, int cols) {
	int i, j;
	mpz_t** mt = *mat = (mpz_t**) malloc(sizeof (mpz_t*) * rows);
	
	if (!mt) {
		printf("Error al reservar memoria\n");
		return NULL;
	}
	
	for (i = 0; i < rows; i++) {
		
		mt[i] = (mpz_t*) malloc(sizeof (mpz_t) * rows);
		if (!mt[i]) {
			free_mpz_matrix(mt, i, cols);
			printf("Error al reservar memoria\n");
			return NULL;
		}
		
		for (j = 0; j < cols; j++) {
			mpz_init(mt[i][j]);
		}
	}
	
	return *mat;
}

void free_mpz_matrix(mpz_t** mat, int rows, int cols) {
       int i, j;
       for(i = 0; i < rows; i++) {
               for (j = 0; j < cols; j++) {
                       mpz_clear(mat[i][j]);
               }
               free(mat[i]);
       }
       free(mat);
}


