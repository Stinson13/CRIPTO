#include "../includes/algoritmos.h"

void cipher(char* src, char* dst, int size, char* clave, mpz_t m);
void decipher(char* src, char* dst, int size, char* clave, mpz_t m);

//Cifrado de Vignere
//Clave : (k1..kn)
//Cifrado de x: (xi + k(i%n)) % m 
//Descifrado de y: yi - k(i%n) % m

int main (int argc,char *argv[]) {
	
	if (argc < 5) {
		printf("Uso: %s {-C|-D} {-m |Zm|} {-k clave} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int modo = -1;
	mpz_t m;
	int c = 0;
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	char* clave = NULL;
	
	FILE* fin = NULL;
	FILE* fout = NULL;
	
	mpz_init (m);
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"C", no_argument, 0, 'C'},
		   {"D", no_argument, 0, 'D'},
		   {"m", required_argument, 0, 'm'},
		   {"k", required_argument, 0, 'k'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+CDm:k:i:o:",
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
			case 'k':
				clave = (char*)malloc(sizeof(char) * (strlen(optarg)+1));
				if (clave == NULL) {
					printf("Insuficiente memoria para guardar clave de tamano %ld",
							strlen(optarg)+1);
					mpz_clear(m);
					if (fin != NULL) {
						fclose(fin);
					}
					if (fout != NULL) {
						fclose(fout);
					}
					return EXIT_FAILURE;
				}
				strcpy(clave, optarg);
				break;
			case 'm':
				mpz_set_str(m, optarg, 10);
				break;
			case 'i':
				strcpy(filein, optarg);
				fin = fopen(filein, "rb");
				if (fin == NULL) {
					printf("Error al abrir %s para leer\n", filein);
					
					mpz_clear(m);
					
					if (clave != NULL) {
						free(clave);
					}
					
					if (fout != NULL) {
						fclose(fout);
					}
					return EXIT_FAILURE;
				}
				break;
			case 'o':
				strcpy(fileout, optarg);
				fout = fopen(fileout, "wb");
				if (fout == NULL) {
					printf("Error al abrir %s para escribir\n", fileout);
					
					mpz_clear(m);
					
					if (clave != NULL) {
						free(clave);
					}
					
					if (fin != NULL) {
						fclose(fin);
					}
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-C|-D} {-m |Zm|} {-k clave} [-i filein] [-o fileout]\n", argv[0]);
				
				mpz_clear(m);
				
				if (clave != NULL) {
					free(clave);
				}
				
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				return EXIT_FAILURE;
		}
	}
	
	if (modo == -1 || !mpz_sgn(m) || clave == NULL) {
		printf("Uso: %s {-C|-D} {-m |Zm|} {-k clave} [-i filein] [-o fileout]\n", argv[0]);
		return EXIT_FAILURE;
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

		//printf("Read %d bytes\n", len);
		
		if (modo == CIFRAR) {
			toUpperOnly(strbuf);
			//puts(strbuf);
			len = strlen(strbuf);
			//printf("Texto empieza por %hhx\n", strbuf[0]);
			cipher(strbuf, strbuf, len, clave, m);
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
		} else {
			//printf("Cifrado empieza por %hhx\n", strbuf[0]);
			decipher(strbuf, strbuf, len, clave, m);
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
	mpz_clear (m);
	return 0;
}

void cipher(char* src, char* dst, int size, char* clave, mpz_t m) {
	int i;
	int n = strlen(clave);
	mpz_t x;
	mpz_t k;
	mpz_t y;
	
	mpz_inits(x, k, y, NULL);
	
	for (i = 0; i < size; i++) {
		mpz_set_si(x, (long)(src[i]));		//read x
		mpz_set_si(k, (long)(clave[i%n]));	//read k
		mpz_add(y, x, k);					//y = x + k
		toModM(y, m);						//y = x + k % m
		//gmp_printf ("y = %Zd\n", y);
		dst[i] = (char)(mpz_get_si(y));		//write y
	}
	
	mpz_clears(x, k, y, NULL);
}

void decipher(char* src, char* dst, int size, char* clave, mpz_t m) {
	int i;
	int n = strlen(clave);
	mpz_t x;
	mpz_t k;
	mpz_t y;
	
	mpz_inits(x, k, y, NULL);
	
	for (i = 0; i < size; i++) {
		mpz_set_si(y, (long)(src[i]));		//read y
		mpz_set_si(k, (long)(clave[i%n]));	//read k
		mpz_sub(x, y, k);					//x = y-k
		toModM(x, m);						//x = y - k % m
		
		dst[i] = (char)(mpz_get_si(x));		//write x
	}
	
	mpz_clears(x, k, y, NULL);
}
