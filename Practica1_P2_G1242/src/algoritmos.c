#include "../includes/algoritmos.h"

void getGDC(mpz_t a, mpz_t b, mpz_t gdc) {
	
	mpz_t max;
	mpz_t min;
	mpz_t r;
	mpz_t q;
	
	mpz_inits (max, min, q, NULL);
	mpz_init_set_ui(r, 2L);
	//gmp_printf ("a es %Zd y b es %Zd\n", a, b);
	//tell which one is the divident and the divisor
	if (mpz_cmp(a,b) > 0) {
		mpz_set(max, a);
		mpz_set(min, b);
	} else {
		mpz_set(max, b);
		mpz_set(min, a);
	}
	
	while (1) {
		mpz_fdiv_qr (q, r, max, min);
		//gmp_printf ("Q es %Zd y R es %Zd\n", gdc, r);
		if (mpz_sgn(r) == 0)
			break;
		mpz_set(max, min);
		mpz_set(min, r);
	}
	mpz_set(gdc, min);
	
	mpz_clears (max, min, r, q, NULL);
}

int getMultInverse(mpz_t a, mpz_t b, mpz_t inverse) {
	
	mpz_t gdc;
	mpz_init (gdc);
	
	getGDC(a, b, gdc);
	
	if (mpz_cmp_ui(gdc, 1L)) {
		mpz_clear (gdc);
		return -1;
	}
	
	mpz_t max;
	mpz_t min;
	mpz_inits (max, min, NULL);
	
	if (mpz_cmp(a,b) > 0) {
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
	mpz_inits (x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);
	
	mpz_set(maxAux, max);
	mpz_set_ui(x2, 1L);
	mpz_set_ui(y1, 1L);
	
	while(mpz_cmp_ui(r, 1L)) {
		// max = q*min + r
		mpz_fdiv_qr (q, r, max, min);
		
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
	
	mpz_clears (gdc, max, min, x, y, x1, y1, x2, y2, q, r, ret, maxAux, NULL);
	return 0;
}

void toUpperOnly(char* src) {
	int  i, j;
	for (i = 0, j = 0; src[i] != '\0'; i++) {
		if (isalpha(src[i])) {
			src[j] = (char)(toupper(src[i]));
			j++;
		}
	}
	src[j] = '\0';
}

void determinante(mpz_t** matrix, int n, mpz_t det) {
	
	int i;
	int j;
	int k;
	mpz_t result;
	mpz_t suma;
	
	if (n == 2) {
		// Guarda temporalmente los resultados en matrix[0][0|1]
		mpz_mul(matrix[0][0], matrix[0][0], matrix[1][1]);
		mpz_mul(matrix[0][1], matrix[1][0], matrix[0][1]);
		mpz_add(det, matrix[0][0], matrix[0][1]);
		return;
	}

	// TODO liberar y init de todos los elementos
	mpz_t **nm;

	for (i = 0; i < (n - 1); i++) {
		for (j = 0; j < (n - 1); j++) {
			mpz_init(nm[i][j]);
		}
	}
	
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (j != i) {
				for (k = 0; k < n; k++) {
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
		
		mpz_inits(result, suma, NULL);
		
		if (i % 2 == 0) {
			determinante(nm, n - 1, result);
			mpz_mul(result, matrix[i][0], result);
			mpz_add(suma, suma, result);
		} else {
			determinante(nm, n - 1, result);
			mpz_mul(result, matrix[i][0], result);
			mpz_sub(suma, suma, result);
		}
	}
	
	mpz_set(det, suma);
	
	mpz_clears(result, suma);
	for (i = 0; i < (n - 1); i++) {
		for (j = 0; j < (n - 1); j++) {
			mpz_clear(nm[i][j]);
		}
	}
}




