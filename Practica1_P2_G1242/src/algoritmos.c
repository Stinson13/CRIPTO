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
