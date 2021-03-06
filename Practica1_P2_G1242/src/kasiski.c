#include "../includes/algoritmos.h"
#define MODULO_M 26

//Porcentajes desde la A a la Z
double spanish_1gram_freqs[] = {11.96, 0.92, 2.92, 6.87, 16.78, 0.52, 0.73, 0.89, 4.15, 0.3, 0, 8.37, 2.12, 7.01, 8.69, 2.77, 1.53, 4.94, 7.88, 3.31, 4.8, 0.39, 0, 0.06, 1.54, 0.15};
double english_1gram_freqs[] = {8.04, 1.13154, 3.06, 3.99, 12.51, 2.30, 1.96, 5.49, 7.26, 0.16, 0.67, 4.14, 2.53, 7.09, 7.60, 2.00, 0.11, 6.12, 6.54, 9.25, 2.71, 0.99, 1.92, 0.19, 1.73, 0.19};



//Cifrado de Vignere
//Clave : (k1..kn)
//Cifrado de x: (xi + k(i%n)) % m 
//Descifrado de y: yi - k(i%n) % m

int main (int argc,char *argv[]) {
	
	if (argc < 5) {
		printf("Uso: %s {-l Ngram} {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
	int l;
	int c = 0;
	char filein[MAX_STR] = {0};
	char fileout[MAX_STR] = {0};
	char filealph[MAX_STR] = {0};
	double* alphabet_frequencies = NULL;
	char strbuf[MAX_STR];
	
	FILE* fin = NULL;
	FILE* fout = NULL;
	FILE* falph = NULL;
	int n = 0;
	char read_a_option = '\0';
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
		   {"l", required_argument, 0, 'l'},
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {"a", required_argument, 0, 'a'},
		   {"n", required_argument, 0, 'n'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+l:a:n:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
			case 'l':
				l = atoi(optarg);
				break;
			case 'a':
				read_a_option = !read_a_option;
				if (strcmp("en", optarg) == 0) {
					alphabet_frequencies = english_1gram_freqs;
				} else if (strcmp("es", optarg) == 0) {
					alphabet_frequencies = spanish_1gram_freqs;
				}
				strcpy(filealph, optarg);
				break;
			case 'n':
				n = atoi(optarg);
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
				printf("Uso: %s {-l Ngram} {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				return EXIT_FAILURE;
		}
	}
	
	if (read_a_option == '\0') {
		printf("Uso: %s {-l Ngram} {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		return EXIT_FAILURE;
	}
	
	int i = 0;
	double baseIoC = 0;
	
	if (alphabet_frequencies != english_1gram_freqs
			&& alphabet_frequencies != spanish_1gram_freqs) {
		if (n < 1) {
			printf("n debe ser positivo\n");
			if (fin != NULL) {
				fclose(fin);
			}
			if (fout != NULL) {
				fclose(fout);
			}
			return EXIT_FAILURE;
		} else {
			falph = fopen(filealph, "r");
			if (falph == NULL) {
				printf("Error al abrir %s para leer\n", filealph);
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				return EXIT_FAILURE;
			}
			
			alphabet_frequencies = (double*) malloc(n * sizeof(double));
			if (alphabet_frequencies == NULL) {
				printf("Insuficiente memoria para guardar tabla de frecuencias\n");
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				fclose(falph);
				return EXIT_FAILURE;
			}
			for (i = 0; !feof(falph) && i < n; i++) {
				fgets(strbuf, MAX_STR, falph);
				alphabet_frequencies[i] = atof(strbuf);
			}
			if (i != n) {
				printf("Tabla incompleta, faltan %d elementos\n", n-i);
				
				if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				fclose(falph);
				return EXIT_FAILURE;
			}
			fclose(falph);
			falph = NULL;
		}
	} else {
		n = MODULO_M;
	}
	
	for (i = 0; i < n; i++) {
		baseIoC += (alphabet_frequencies[i] * alphabet_frequencies[i]) / (100 * 100);
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
	
	int len;
	
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
	}
	
	//now that the text is known, it has to be copied to a string for analysis
	//+1 to leave space for terminationg '\0'
	char* text = (char*) malloc((ftell(ftemp) + 1) * sizeof(char));
	if (text == NULL) {
		if (alphabet_frequencies != english_1gram_freqs
				&& alphabet_frequencies != spanish_1gram_freqs) {
			free(alphabet_frequencies);
		}
		
		if (fin != stdin) {
			fclose(fin);
		}
		if (fout != stdout) {
			fclose(fout);
		}
		fclose(ftemp);
		return EXIT_FAILURE;
	}
	//finish all writes
	fflush(ftemp);
	//back to the beggining of the file
	rewind(ftemp);
	
	char *p, *q;
	
	for(p = text; !feof(ftemp); p += len) {
		len = fread(p, sizeof(char), MAX_STR, ftemp);
	}
	//write end of string
	p[0] = '\0';
	
	//Find most common ngram first
	p = most_common_ngram(l, text);
	strncpy(ngram, p, l);
	printf("Most common ngram: %s\n", ngram);
	
	mpz_inits(distance_gdc, a, NULL);
	
	for (q = p = strstr(text, ngram); q != NULL; p = q) {
		q = strstr(p + l, ngram);
		if (q != NULL) {
			if (mpz_sgn(distance_gdc) == 0) {
				mpz_set_si(distance_gdc, (q - p)/sizeof(char));
			} else {
				mpz_set_si(a, (q - p)/sizeof(char));
				getGDC(distance_gdc, a, distance_gdc);
			}
		}
	}
	
	unsigned int gdc = mpz_get_ui(distance_gdc);
	
	int** occurences;
	occurences = (int**) malloc (gdc * sizeof(int*));
	
	double* indicesOfCoincidence;
	double cmpIoC;
	int j = 0, k = 0;
	double diff = 0;
	char* clave = NULL;
	
	if (occurences == NULL) {
		puts("Insuficiente memoria para calcular indices de coincidencia");
	} else {
		for (i = 0; i < gdc; i++) {
			occurences[i] = (int*) calloc (n, sizeof(int));
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
				
				for (i = 0; text[i] != '\0'; i++) {
					occurences[i % gdc][text[i] % n]++;
				}
				
				//calculate index of coincidence for each row
				indicesOfCoincidence = (double*) calloc (gdc, sizeof(double));
				if (indicesOfCoincidence != NULL) {
					printf("Indices de coincidencia: ");
					for(i = 0; i < gdc; i++) {
						for (j = 0; j < n; j++) {
							//ASCII equivalent, index, occurences
							//printf("%c %i %d\t", (j + 13) % n + 'A', j, occurences[i][j]);
							
							//use the approximate calculation (occurences/total)
							//not the read index of coincidence
							indicesOfCoincidence[i] +=
									(double)(occurences[i][j]
											* occurences[i][j])
									/ (ftell(ftemp) * ftell(ftemp));
						}
						if (i != gdc - 1) {
							printf("%.2lf, ", indicesOfCoincidence[i]);
						}
					}
					printf("%.2lf\n", indicesOfCoincidence[gdc - 1]);
					printf("Promedio de indices de coincidencia: %.2lf\n",
							average(gdc, indicesOfCoincidence));
					
					clave = (char*)calloc(gdc + 1, sizeof(char));
					if (clave == NULL) {
						printf("Insuficiente memoria para guardar la clave estimada");
					} else {
						cmpIoC = 0;
						for(i = 0; i < gdc; i++) {
							indicesOfCoincidence[i] = 1;
							for (j = 0; j < n; j++, cmpIoC = 0) {
								for (k = 0; k < n; k++) {
									//index of coincidence for character
									//shifted j positions
									cmpIoC +=
										(double)(occurences[i][(j + k) % n]
												* alphabet_frequencies[k])
										/ (ftell(ftemp) * 100.0);
								}
								diff = fabs(baseIoC - cmpIoC);
								//printf("Got diff %lf for j = %i\n", diff, j);
								//minimize difference towards target
								if (diff < indicesOfCoincidence[i]) {
									indicesOfCoincidence[i] = diff;
									//ASCII equivalent
									clave[i] = (j /*+ 13*/) % n + 'A';		
								}
							}
						}
						clave[gdc] = '\0';
						printf("La clave es %s\n", clave);
						free(clave);
					}
					
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
	for (i = 0; i < n - 1; i++) {
		printf("%i, ", occurences[i]);
	}
	printf("%i\n", occurences[i]);*/
	
	if (alphabet_frequencies != english_1gram_freqs
			&& alphabet_frequencies != spanish_1gram_freqs) {
		free(alphabet_frequencies);
	}
	
	if (fin != stdin) {
		fclose(fin);
	}
	if (fout != stdout) {
		fclose(fout);
	}
	fclose(ftemp);
	free(text);
	
	mpz_clears(distance_gdc, a, NULL);
	return 0;
}
