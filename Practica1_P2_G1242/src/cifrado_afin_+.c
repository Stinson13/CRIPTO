#include "../includes/algoritmos.h"

void cipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b, mpz_t c, mpz_t d);
void decipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b, mpz_t c, mpz_t d);

//Cifrado afín
//Clave : (a, b)
//Cifrado de x: (a*x + b) % m 
//Descifrado de y: (y-b)*(a^-1) % m

int main (int argc,char *argv[]) {
	
	if (argc < 12) {
		printf("Uso: %s {-C|-D} {-m |Zm|} {-a N} {-b N} {-c N} {-d N} \
[-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int modo = -1;
	mpz_t m;
	mpz_t a;
	mpz_t b;
	mpz_t c;
	mpz_t d;
	int ch = 0;
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	
	FILE* fin = NULL;
	FILE* fout = NULL;
	
	mpz_inits (m, a, b, c, d, NULL);
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"m", required_argument, 0, 'm'},
		   {"a", required_argument, 0, 'a'},
		   {"b", required_argument, 0, 'b'},
		   {"c", required_argument, 0, 'c'},
		   {"d", required_argument, 0, 'd'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		ch = getopt_long(argc, argv, "+CDm:a:b:c:d:i:o:",
			long_options, &option_index);
		if (ch < 0)
			break;
		
		switch (ch) {
			case 'C':
				modo = CIFRAR;
				break;
			case 'D':
				modo = DESCIFRAR;
				break;
			case 'a':
				mpz_set_str(a, optarg, 10);
				break;
			case 'b':
				mpz_set_str(b, optarg, 10);
				break;
			case 'c':
				mpz_set_str(c, optarg, 10);
				break;
			case 'd':
				mpz_set_str(d, optarg, 10);
				break;
			case 'm':
				mpz_set_str(m, optarg, 10);
				break;
			case 'i':
				strcpy(filein, optarg);
				fin = fopen(filein, "rb");
				if (fin == NULL) {
					printf("Error al abrir %s para leer\n", filein);
					return EXIT_FAILURE;
				}
				break;
			case 'o':
				strcpy(fileout, optarg);
				fout = fopen(fileout, "wb");
				if (fout == NULL) {
					printf("Error al abrir %s para escribir\n", fileout);
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C|-D} {-m |Zm|} {-a N} {-b N} {-c N} {-d N} [-i filein] [-o fileout]\n", argv[0]);
				return -1;
		}
	}
	
	if (modo == -1 || !mpz_sgn(a) || !mpz_sgn(b) || !mpz_sgn(c) || !mpz_sgn(d) || !mpz_sgn(m)) {
		printf("{-C|-D} {-m |Zm|} {-a N} {-b N} {-c N} {-d N} son obligatorios\n");
		return -1;
	}
	
	/*printf("%s %s -m %d -a %d -b %d",
			argv[0],
			modo==CIFRAR? "cifrando":"descifrando",
			m,
			a,
			b);
	if (filein[0] == '\0' && fileout[0] == '\0') {
		printf("\n");
	} else {
		if (filein[0] != '\0') {
			printf(" -i %s", filein);
		}
		if (fileout[0] != '\0') {
			printf(" -o %s", fileout);
		}
		printf("\n");
	}*/
	
	mpz_t gdc;
	mpz_init(gdc);
	getGDC(a, b, gdc);
	gmp_printf ("El mcd de %Zd y %Zd es %Zd\n", a, b, gdc);
	mpz_clear(gdc);
	
	mpz_t inverseA;
	mpz_t inverseC;
	mpz_inits(inverseA, inverseC, NULL);
	getMultInverse(a, m, inverseA);
	getMultInverse(c, m, inverseC);
	gmp_printf ("El inverso de %Zd en %Zd es %Zd\n", a, m, inverseA);
	gmp_printf ("El inverso de %Zd en %Zd es %Zd\n", c, m, inverseC);
	
	if (mpz_sgn(inverseA) == 0 || mpz_sgn(inverseC) == 0) {
		gmp_printf("Los parametros suplidos no crean un criptosistema:\nm=%Zd\
\na=%Zd\nc=%Zd\n", m, a, c);
		mpz_clears(m, a, b, c, d, NULL);
		return -1;
	}
	mpz_clears(inverseA, inverseC, NULL);
	
	if (fin == NULL) {
		fin = stdin;
	}
	if (fout == NULL) {
		fout = stdout;
	}
	
	char strbuf[MAX_STR];
	int len;
	int i;
	while (!feof(fin) && !ferror(fin)) {
		if (fin == stdin) {
			fgets(strbuf, MAX_STR, fin);
			if (feof(fin)) {
				break;
			}
			len = strlen(strbuf);
			if (strbuf[len-1] == '\n') {
				len--;
				strbuf[len] = '\0';			
			}			
		} else {
			len = fread(strbuf, sizeof(char), MAX_STR, fin);
		}

		printf("Read %d bytes\n", len);
		
		if (modo == CIFRAR) {
			toUpperOnly(strbuf);
			puts(strbuf);
			len = strlen(strbuf);
			//printf("Texto empieza por %hhx\n", strbuf[0]);
			cipher(strbuf, strbuf, len, m, a, b, c, d);
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
		} else {
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
			decipher(strbuf, strbuf, len, m, a, b, c, d);
			//printf("Texto empieza por %hhx\n", strbuf[0]);
		}
		fwrite(strbuf, len, sizeof(char), fout);
	}
	
	if (fin != stdin) {
		fclose(fin);
	}
	if (fout != stdout) {
		fclose(fout);
	}
	mpz_clears (m, a, b, c, d, NULL);
	return 0;
}

void cipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b, mpz_t c, mpz_t d) {
	int i;
	mpz_t last;
	mpz_t x;
	mpz_t y;
	
	mpz_inits(last, x, y, NULL);
	
	for (i = 0; i < size; i++, mpz_set(last, x)) {
		mpz_set_si(x, (long)(src[i]) - 'A');	//read x
		//gmp_printf("Ciphering: Current is: %Zd\tLast was %Zd\n", x, last);
		mpz_add(y, x, last);					//y = x + x'
		mpz_mul(y, a, y);						//y = a*(x+x')
		mpz_add(y, y, b);						//y = a*(x+x') + b
		mpz_mul(y, c, y);						//y = c*(a*(x+x') + b)
		mpz_add(y, y, d);						//y = c*(a*(x+x') + b) + d
		mpz_fdiv_r(y, y, m);					//y = c*(a*(x+x') + b) + d % m
		dst[i] = (char)(mpz_get_si(y));			//write y
	}
	
	mpz_clears(last, x, y, NULL);
}

void decipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b, mpz_t c, mpz_t d) {
	int i;
	mpz_t x;
	mpz_t y;
	mpz_t inv;
	mpz_t last;
	
	mpz_inits(x, y, inv, last, NULL);
	
	for (i = 0; i < size; i++, mpz_set(last, x)) {
		mpz_set_si(y, (long)(src[i]));	//read y
		
		mpz_sub(x, y, d);						//x = y-d
		getMultInverse(c, m, inv);
		mpz_mul(x, x, inv);						//x = (y-d)*(c^-1)
		mpz_sub(x, x, b);						//x = (y-d)*(c^-1)-b
		
		getMultInverse(a, m, inv);
		mpz_mul(x, x, inv);						//x = ((y-d)*(c^-1)-b)*(a^-1)
		mpz_sub(x, x, last);					//x = ((y-d)*(c^-1)-b)*(a^-1)-x'
		mpz_fdiv_r(x, x, m);					//x = ((y-d)*(c^-1)-b)*(a^-1)-x' % m
		//gmp_printf("Deciphering: Current is: %Zd\tLast was %Zd\n", x, last);
		dst[i] = (char)(mpz_get_si(x) + 'A');	//write x
	}
	
	mpz_clears(x, y, inv, last, NULL);
}

