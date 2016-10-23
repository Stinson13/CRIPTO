#include "../includes/algoritmos.h"

void getGDC(mpz_t a, mpz_t b, mpz_t gdc) {

    mpz_t max;
    mpz_t min;
    mpz_t r;
    mpz_t q;

    if (mpz_cmp_ui(a, 0L) == 0 || mpz_cmp_ui(b, 0L) == 0) {
    	mpz_set_ui(gdc, 0L);
    	return;
    }

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
	mpz_t r;
	mpz_inits(q, r, NULL);
	
	mpz_tdiv_qr(q, r, x, m);
	
	if (mpz_sgn(x) < 0) {
		//|x| < m
		if (mpz_sgn(q) == 0) {
			//gmp_printf("(1) %Zd %% %Zd = %Zd + %Zd\n", x, m, x, m);
			mpz_add(x, x, m);
		} else {
			mpz_abs(q, q);
			if (mpz_sgn(r) != 0) {
				mpz_add_ui(q, q, 1L);
			}
			//gmp_printf(" (2) %Zd %% %Zd = %Zd + %Zd * %Zd\n", x, m, x, q, m);
			mpz_mul(q, m, q);
			mpz_add(x, x, q);
		}
	} else if (mpz_cmp(x, m) >= 0) {
		//gmp_printf(" (3) %Zd %% %Zd = %Zd - %Zd * %Zd\n", x, m, x, q, m);
		mpz_mul(q, m, q);
		mpz_sub(x, x, q);
	}
	//else x is already % m
	
	mpz_clears(q, r, NULL);
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

void matrixTransposed(mpz_t** matrix, int n, mpz_t** matrixTrans, mpz_t m) {

	int i;
	int j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_set(matrixTrans[i][j], matrix[j][i]);
			toModM(matrixTrans[i][j], m);
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
	int a;
	int b;
	int matrix_size_adj;
	mpz_t base;
	mpz_t rop;

	if (n == 2) {
		mpz_set(matrixCof[0][0], matrix[1][1]);
		mpz_mul_si(matrixCof[0][1], matrix[1][0], -1L);
		toModM(matrixCof[0][1], m);
		mpz_mul_si(matrixCof[1][0], matrix[0][1], -1L);	
		toModM(matrixCof[1][0], m);
		mpz_set(matrixCof[1][1], matrix[0][0]);	

		return;

	} else {
		matrix_size_adj = (n - 1);
	}

	mpz_inits(base, rop, NULL);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
		
			init_mpz_matrix(&det, matrix_size_adj, matrix_size_adj);

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

			mpz_init(valueDet);

			/*for (a = 0; a < (n - 1); a++) {
				gmp_printf("|%Zd %Zd|\n", det[a][0], det[a][1]);
			}
			printf("\n");*/

			determinante(det, matrix_size_adj, valueDet, m);

			if (((i+j) % 2) == 1) {
				mpz_neg(valueDet, valueDet);
				toModM(valueDet, m);
			}
			mpz_set(matrixCof[i][j], valueDet);
			//gmp_printf("matrixCof[%d][%d] = %Zd\n", i, j, matrixCof[i][j]);

			free_mpz_matrix(det, matrix_size_adj, matrix_size_adj);

			mpz_clear(valueDet);
		}
	}
	
}

void matrixAdjoint(mpz_t** matrix, int n, mpz_t** matrixAdj, mpz_t m) {

	matrixCofactors(matrix, n, matrixAdj, m);
	return;
}

void mulMatrixConst(mpz_t cons, mpz_t** matrix, mpz_t** matrixRes, int n) {

	int i;
	int j;
	mpz_t res;

	mpz_init(res);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mpz_mul(res, matrix[i][j], cons);
			mpz_set(matrixRes[i][j], res);
		}
	}

	mpz_clear(res);
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

int matrixInverse(mpz_t** matrix, int n, mpz_t m, mpz_t** matrixInv) {

	int i;
	int j;
	mpz_t det;
	mpz_t gdc;
	mpz_t matrix_size;
	mpz_t detInv;
	mpz_t** matrixTrans;
	mpz_t** matrixAdj;

	mpz_inits(det, detInv, gdc, NULL);

	determinante(matrix, n, det, m);

	if (!mpz_cmp_si(det, 0L)) {
		printf("Error: El determinante de la matriz de claves es 0.\n");
		return -1;
	}

	mpz_init_set_si (matrix_size, n);

	getGDC(det, matrix_size, gdc);

	if (mpz_cmp_si(gdc, 1L)) {
		printf("Error: El mcd entre el determinante de la matriz de claves y n es distinto de 1.\n");
		return -1;
	}

	if (getMultInverse(det, m, detInv) < 0) {
		return -1;
	}

	init_mpz_matrix(&matrixTrans, n, n);

	if (!matrixTrans) {
		return -1;
	}

    matrixTransposed(matrix, n, matrixTrans, m);

    init_mpz_matrix(&matrixAdj, n, n);

	if (!matrixAdj) {
		return -1;
	}

    matrixAdjoint(matrixTrans, n, matrixAdj, m);

    mulMatrixConst(detInv, matrixAdj, matrixInv, n);

    for (i = 0; i < n; i++) {
    	for (j = 0; j < n; j++) {
    		toModM(matrixInv[i][j], m);
    	}
    }

    free_mpz_matrix(matrixTrans, n, n);
	free_mpz_matrix(matrixAdj, n, n);

	mpz_clears(det, detInv, gdc, matrix_size, NULL);
	return 0;
}

void mulMatrixMatrix(mpz_t** matrix1, mpz_t** matrix2, mpz_t** matrixRes, int n, mpz_t m) {

	int i;
	int j;
	int k;
	mpz_t res;

	mpz_init(res);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (k = 0; k < n; k++) {
				mpz_mul(res, matrix1[i][k], matrix2[k][j]);
				//gmp_printf("%Zd * %Zd = %Zd\n", matrix1[i][k], matrix2[k][j], res);
				mpz_add(matrixRes[i][j], matrixRes[i][j], res);
				//gmp_printf("matrixRes[%d][%d]: %Zd\n", i, j, matrixRes[i][j]);
			}
		}
	}

	printf("\n");

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			//gmp_printf("matrixRes[%d][%d]: %Zd\n", i, j, matrixRes[i][j]);
			toModM(matrixRes[i][j], m);
		}
	}

	mpz_clear(res);
	return;
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

double average(int how_many, double* vals) {
	va_list args;
	int i = 0;
	double sum = 0;
	
	for (i = 0; i < how_many; i++) {
		sum += vals[i];
	}
	return sum/how_many;
}

char* most_common_ngram(int ngram_length, char* string) {
	//upper limit bounded by this formula
	int max_table_size = 1 + (strlen(string) - ngram_length);
	int current_table_size = 0;
	
	if (max_table_size < 1) {
		return NULL;
	}
	
	int* occurences = (int*)malloc(max_table_size * sizeof(int));
	if (occurences == NULL) {
		printf("Insuficiente memoria para encontrar ngrama mas comun\n");
		return NULL;
	}
	char** ngrams = (char**)malloc(max_table_size * sizeof(char*));
	if (ngrams == NULL) {
		printf("Insuficiente memoria para encontrar ngrama mas comun\n");
		free (occurences);
		return NULL;
	}
	char* p;
	int i;
	
	//+1 to read last element too
	for(p = string; p < string + max_table_size + 1; p++) {
		
		for (i = 0; i < current_table_size; i++) {
			if (strncmp(p, ngrams[i], ngram_length) == 0) {
				occurences[i]++;
				break;
			}
		}
		if (i == current_table_size && i < max_table_size) {
			ngrams[i] = p;
			occurences[i] = 1;
			current_table_size++;
		}
	}
	
	int max_i = 0;
	for (i = 0; i < current_table_size; i++) {
		if (occurences[max_i] <= occurences[i]) {
			max_i = i;
		}
	}
	
	p = ngrams[max_i];
	free(occurences);
	free(ngrams);
	return p;
}

int getRandomLessN(int n) {

	srand(time(NULL));
	return (rand() % n);
}

void makePermutation(char* permutation, int n) {

	char* auxPerm;
	int i;
	int j = n;
	int randomValue;

	auxPerm = (char *) calloc ((n + 1), sizeof(char));
	if (!auxPerm) {
		permutation = NULL;
		return;
	}

	for (i = 0; i < n; i++) {
		auxPerm[i] = i + 1;
	}

	for (i = 0; i < n; i++) {
		randomValue = getRandomLessN(j);

		memcpy(&permutation[i], &auxPerm[randomValue], sizeof(char));

		fitArray(auxPerm, randomValue, n);

		j--;
	}

	free(auxPerm);
	return;
}

void fitArray(char* array, int posEle, int arraySize) {

	int i;
	int j = 0;

	for (i = (posEle + 1); i < arraySize; i++) {
		memcpy(&array[i - 1], &array[i], sizeof(char));
		j++;
	}

	return;
}
