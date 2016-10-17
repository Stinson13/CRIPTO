#include "../includes/algoritmos.h"
#define MODULO_M 26

//Porcentajes desde la A a la Z
double spanish_1gram_freqs[] = {11.96, 0.92, 2.92, 6.87, 16.78, 0.52, 0.73, 0.89, 4.15, 0.3, 0, 8.37, 2.12, 7.01, 8.69, 2.77, 1.53, 4.94, 7.88, 3.31, 4.8, 0.39, 0, 0.06, 1.54, 0.15};
double english_1gram_freqs[] = {8.04, 1.54, 3.06, 3.99, 12.51, 2.30, 1.96, 5.49, 7.26, 0.16, 0.67, 4.14, 2.53, 7.09, 7.60, 2.00, 0.11, 6.12, 6.54, 9.25, 2.71, 0.99, 1.92, 0.19, 1.73, 0.19};



//Cifrado de Vignere
//Clave : (k1..kn)
//Cifrado de x: (xi + k(i%n)) % m 
//Descifrado de y: yi - k(i%n) % m

int main (int argc,char *argv[]) {
	
	if (argc < 3) {
		printf("Uso: %s {-l Ngram} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int l;
	int c = 0;
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	char* clave = NULL;
	
	FILE* fin = NULL;
	FILE* fout = NULL;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"l", no_argument, 0, 'l'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+l:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'l':
				l = atoi(optarg);
				break;
			case 'i':
				strcpy(filein, optarg);
				fin = fopen(filein, "rb");
				if (fin == NULL) {
					printf("Error al abrir %s para leer\n", filein);
					
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
					
					if (fin != NULL) {
						fclose(fin);
					}
					return EXIT_FAILURE;
				}
				break;
			default:
				printf("Uso: %s {-l Ngram} [-i filein] [-o fileout]\n", argv[0]);
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				return EXIT_FAILURE;
		}
	}
	
	if (l < 1) {
		printf("l debe ser positivo\n");
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		return EXIT_FAILURE;
	}
	
	if (fin == NULL) {
		fin = stdin;
	}
	if (fout == NULL) {
		fout = stdout;
	}
	
	char strbuf[MAX_STR];
	char ngram[MAX_STR];
	memset(ngram, 0, MAX_STR * sizeof(char));
	
	mpz_t a;
	mpz_t distance_gdc;
	
	//I can't rely on ftell because it moves to -1 after each read for stdin
	//so I'll copy everything to a file I can read when the block size
	//is known
	FILE* ftemp = tmpfile();
	if (ftemp == NULL) {
		perror(NULL);
		
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		
		return EXIT_FAILURE;
	}
	mpz_t m;
	
	int i = 0, j = 0, k;
	int len;
	
	mpz_inits(distance_gdc, a, m, NULL);
	mpz_set_ui(m, MODULO_M);
	
	if (fin == stdin) {
		puts("Presione Ctrl-D para terminar el programa");
	}
	
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
		//save read bytes to buffer file
		fwrite(strbuf, len, sizeof(char), ftemp);
		
		//get ngram if there isn't one already
		if (ngram[0] == '\0') {
			strncpy(ngram, strbuf, l);
			i += l;
		}
		
		//get all ngram occurences and their distances' gdc
		for (; i < len; i++) {
			//ngram at start of string
			if (strstr(strbuf + i, ngram) == strbuf + i) {
				//get gdc
				if (mpz_sgn(distance_gdc) == 0) {
					mpz_set_si(distance_gdc, j);
				} else {
					mpz_set_si(a, j);
					getGDC(distance_gdc, a, distance_gdc);
				}
				
				i += l - 1;
				j = 0;
			} else {
				j++;
			}
		}
		i = 0;
	}
	
	unsigned int gdc = mpz_get_ui(distance_gdc);
	
	//finish all writes
	fflush(ftemp);
	//back to the beggining of the file
	rewind(ftemp);
	
	int** occurences;
	occurences = (int**) malloc (gdc * sizeof(int*));
	
	double* indicesOfCoincidence;
	
	if (occurences == NULL) {
		puts("Insuficiente memoria para calcular indices de coincidencia");
	} else {
		for (i = 0; i < gdc; i++) {
			occurences[i] = (int*) calloc (MODULO_M, sizeof(int));
			if (occurences[i] == NULL) {
				puts("Insuficiente memoria para calcular indices de coincidencia");
				for (i--; i > -1; i--) {
					free(occurences[i]);
				}
				free(occurences);
				break;
			}
		}
		
		if (occurences != NULL) {
			//do nothing if block size wasn't found
			if (gdc > 0) {
				printf("La longitud de bloque estimada es de %u\n", gdc);
				
				for (i = 0; 1; i = (i + 1) % gdc) {
					c = fgetc(ftemp);
					if (feof(ftemp))
						break;
					occurences[i][c % MODULO_M]++;
				}
				
				//calculate index of coincidence for each row
				indicesOfCoincidence = (double*) calloc (gdc, sizeof(double));
				if (indicesOfCoincidence != NULL) {
					printf("Indices de coincidencia: ");
					for(i = 0; i < gdc; i++) {
						for (j = 0; j < MODULO_M; j++) {
							//ASCII equivalent, index, occurences
							//printf("%c %i %d\t", (j + 13) % MODULO_M + 'A', j, occurences[i][j]);
							
							//use the approximate calculation (occurences/total)
							//not the read index of coincidence
							indicesOfCoincidence[i] +=
									((double)(occurences[i][j]
											* occurences[i][j]))
									/ (ftell(ftemp) * ftell(ftemp));
						}
						if (i != gdc - 1) {
							printf("%.2lf, ", indicesOfCoincidence[i]);
						}
					}
					printf("%.2lf\n", indicesOfCoincidence[gdc - 1]);
					printf("Promedio de indices de coincidencia: %lf\n",
							average(gdc, indicesOfCoincidence));
					
					free(indicesOfCoincidence);
				}
			} else {
				puts("La longitud del bloque no pudo ser determinada");
			}
			
			for(i = 0; i < gdc; i++) {
				free(occurences[i]);
			}
			free(occurences);
		}
	}
	
	
	
	/*printf("Occurences: ");
	for (i = 0; i < MODULO_M - 1; i++) {
		printf("%i, ", occurences[i]);
	}
	printf("%i\n", occurences[i]);*/
	
	if (fin != stdin) {
		fclose(fin);
	}
	if (fout != stdout) {
		fclose(fout);
	}
	fclose(ftemp);
	
	mpz_clears(distance_gdc, a, m, NULL);
	return 0;
}
