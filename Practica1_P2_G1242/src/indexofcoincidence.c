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
		printf("Uso: %s {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
		return 0;
	}
	
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
		   {"i", required_argument, 0, 'i'},
		   {"o", required_argument, 0, 'o'},
		   {"a", required_argument, 0, 'a'},
		   {"n", required_argument, 0, 'n'},
		   {0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "+a:n:i:o:",
			long_options, &option_index);
		if (c < 0)
			break;
		
		switch (c) {
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
				printf("Uso: %s {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
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
		printf("Uso: %s {-a en| -a es| -a freqsfile -n |Zn|} [-i filein] [-o fileout]\n", argv[0]);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		return EXIT_FAILURE;
	}
	
	int i = 0;
	
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
	
	double baseIoC = 0;
	
	for (i = 0; i < n; i++) {
		baseIoC += (alphabet_frequencies[i] * (alphabet_frequencies[i] - 1)) / (100 * (100 - 1));
	}
	
	if (fin == NULL) {
		fin = stdin;
	}
	if (fout == NULL) {
		fout = stdout;
	}
	
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
	
	int j = 0, k = 0;
	int block_size = 0;
	int total_occurences = 0;
	double avgIoC = 0;
	double diff = 1;
	
	int* occurences = (int*) malloc(n * sizeof(int));
	if (occurences != NULL) {
		//for each possible block size
		for (i = 1, len = ftell(ftemp); i < len; i++, avgIoC = 0) {
			//for each element index within the block
			for (j = 0; j < i; j++) {
				memset(occurences, 0, n * sizeof(int));
				//add the index of coincidence to accumulator
				for (k = j; k < len; k += i) {
					occurences[text[k]%n]++; 
				}
				for (k = 0; k < n; k++) {
					//printf("Occurences: %i\tblocks: %i\n", occurences[k], (j < (len%i) ? (len/i + 1): (len/i)));
					if (occurences[k] > 1) {
						if (j < len % i) {
							avgIoC += ((double)occurences[k] * (occurences[k] - 1)) / ((len/i + 1) * (len/i));
						} else if (len/i > 2) {
							avgIoC += ((double)occurences[k] * (occurences[k] - 1)) / (len/i * (len/i-1));
						}
					}
				}
			}
			//find average index of coincidence
			avgIoC /= i;
			printf("Avg is %lf, diff is %lf for block size %i\n", avgIoC, fabs(baseIoC - avgIoC), i);
			if (fabs(baseIoC - avgIoC) < diff) {
				block_size = i;
				diff = fabs(baseIoC - avgIoC);
			}
		}
		free(occurences);
	} else {
		printf("Insuficiente memoria para la tabla de frecuencias\n");
	}
	
	if (block_size > 0) {
		printf("El tamano de bloque es %d\n", block_size);
	} else {
		puts("El tamano de bloque no pudo ser determinado");
	}
	
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
	
	return 0;
}
