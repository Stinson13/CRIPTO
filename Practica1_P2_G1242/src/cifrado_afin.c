#include "../includes/algoritmos.h"

void cipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b);
void decipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b);

//Cifrado afín
//Clave : (a, b)
//Cifrado de x: (a*x + b) % m 
//Descifrado de y: (y-b)*(a^-1) % m

int main (int argc,char *argv[]) {
	
	if (argc < 8) {
		printf("Uso: %s {-C|-D} {-m |Zm|} {-a N} {-b N} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int modo = -1;
	mpz_t m;
	mpz_t a;
	mpz_t b;
	int c = 0;
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	
	FILE* fin = NULL;
	FILE* fout = NULL;
	
	mpz_inits (m, a, b, NULL);
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"m", required_argument, 0, 'm'},
		   {"a", required_argument, 0, 'a'},
		   {"b", required_argument, 0, 'b'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+CDm:a:b:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
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
				printf("Uso: %s {-C|-D} {-m |Zm|} {-a N} {-b N} [-i filein] [-o fileout]\n", argv[0]);
				return -1;
		}
	}
	
	if (modo == -1 || !mpz_sgn(a) || !mpz_sgn(b) || !mpz_sgn(m)) {
		printf("{-C|-D} {-m |Zm|} {-a N} {-b N} son obligatorios\n");
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
	
	mpz_t inverse;
	mpz_init(inverse);
	getMultInverse(a, m, inverse);
	gmp_printf ("El inverso de %Zd en %Zd es %Zd\n", a, m, inverse);
	mpz_clear(inverse);
	
	if (mpz_sgn(inverse) == 0) {
		gmp_printf("Los parametros suplidos no crean un criptosistema:\nm=%Zd\
\na=a%Zd\n", m, a);
		mpz_clears(m, a, b, NULL);
		return -1;
	}
	
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
			if(feof(fin)) {
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
			cipher(strbuf, strbuf, len, m, a, b);
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
		} else {
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
			decipher(strbuf, strbuf, len, m, a, b);
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
	mpz_clears (m, a, b, NULL);
	return 0;
}

void cipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b) {
	int i;
	mpz_t x;
	mpz_t y;
	
	mpz_inits(x, y, NULL);
	
	for (i = 0; i < size; i++) {
		mpz_set_si(x, (long)(src[i] - 'A'));	//read x
		mpz_mul(y, a, x);						//y = a*x
		mpz_add(y, y, b);						//y = a*x + b
		mpz_fdiv_r(y, y, m);					//y = a*x + b % m
		
		dst[i] = (char)(mpz_get_si(y));	//write y
	}
	
	mpz_clears(x, y, NULL);
}

void decipher(char* src, char* dst, int size, mpz_t m, mpz_t a, mpz_t b) {
	int i;
	mpz_t x;
	mpz_t y;
	mpz_t inv;
	
	mpz_inits(x, y, inv, NULL);
	
	for (i = 0; i < size; i++) {
		mpz_set_si(y, (long)(src[i]));			//read y
		
		mpz_sub(x, y, b);						//x = y-b
		getMultInverse(a, m, inv);
		mpz_mul(x, x, inv);						//x = (y-b)*(a^-1)
		mpz_fdiv_r(x, x, m);					//x = (y-b)*(a^-1) % m
		
		dst[i] = (char)(mpz_get_si(x) + 'A');	//write x
	}
	
	mpz_clears(x, y, inv, NULL);
}
