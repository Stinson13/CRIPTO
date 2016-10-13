#include "../includes/algoritmos.h"

void cipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m);
void decipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m);

int main(int argc, char *argv[]) {

    if (argc < 7) {
        printf("Uso: %s {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n", argv[0]);
        return 0;
    }

    int modo = -1;
    int n = -1;
    int i;
    int j;
    int index;
    mpz_t m;
    mpz_t **matrixP;
    mpz_t **matrixC;
    mpz_t **matrixK;
	mpz_t **matrixInv;
    //mpz_t **matrixInv;
    //mpz_t** matrixRes;
    int c = 0;
    char filek[MAX_STR] = {0};
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};

    FILE* fk = NULL;
    FILE* fin = NULL;
    FILE* fout = NULL;

    mpz_init(m);

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"C", no_argument, 0, 'C'},
            {"D", no_argument, 0, 'D'},
            {"m", required_argument, 0, 'm'},
            {"n", required_argument, 0, 'n'},
            {"k", required_argument, 0, 'k'},
            {"i", required_argument, 0, 'i'},
            {"o", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };
        c = getopt_long(argc, argv, "+CDm:n:k:i:o:",
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
            case 'n':
                n = atoi(optarg);
                break;
            case 'k':
                strcpy(filek, optarg);
                fk = fopen(filek, "rb");
                if (fk == NULL) {
                    printf("Error al abrir %s para leer\n", filek);
                    mpz_clear(m);
                    if (fin != NULL) {
						fclose(fin);
					}
					if (fout != NULL) {
						fclose(fout);
					}
                    return EXIT_FAILURE;
                }
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
                    if (fout != NULL) {
						fclose(fout);
					}
					if (fk != NULL) {
						fclose(fk);
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
                    if (fin != NULL) {
						fclose(fin);
					}
					if (fk != NULL) {
						fclose(fk);
					}
                    return EXIT_FAILURE;
                }
                break;
            default:
                printf("Uso: %s {-C|-D} {-m |Zm|} {-n Nk} {-k filek} [-i filein] [-o fileout]\n", argv[0]);
                mpz_clear(m);
                if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
				if (fk != NULL) {
					fclose(fk);
				}
                return EXIT_FAILURE;
        }
    }

    if (modo == -1 || n == -1 || !mpz_sgn(m)) {
        printf("{-C|-D} {-m |Zm|} {-n Nk} {-k filek} son obligatorios\n");
        mpz_clear(m);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		if (fk != NULL) {
			fclose(fk);
		}
        return EXIT_FAILURE;
    }

    if (n < 2) {
		printf("La dimension de la matiz debe ser al menos 2");
		mpz_clear(m);
		if (fin != NULL) {
			fclose(fin);
		}
		if (fout != NULL) {
			fclose(fout);
		}
		if (fk != NULL) {
			fclose(fk);
		}
        return EXIT_FAILURE;
	}

    init_mpz_matrix(&matrixP, n, n);
    if (matrixP == NULL) {
		mpz_clear(m);
		return EXIT_FAILURE;
	}

    init_mpz_matrix(&matrixC, n, n);
    if (matrixC == NULL) {
		free_mpz_matrix(matrixP, n, n);
		mpz_clear(m);
		return EXIT_FAILURE;
	}

    init_mpz_matrix(&matrixK, n, n);
    if (matrixK == NULL) {
		free_mpz_matrix(matrixP, n, n);
		free_mpz_matrix(matrixC, n, n);
		mpz_clear(m);
		return EXIT_FAILURE;
	}

	init_mpz_matrix(&matrixInv, n, n);
    if (matrixInv == NULL) {
		free_mpz_matrix(matrixP, n, n);
		free_mpz_matrix(matrixC, n, n);
		free_mpz_matrix(matrixK, n, n);
		mpz_clear(m);
		return EXIT_FAILURE;
	}

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            mpz_inp_str(matrixK[i][j], fk, 10);
            toModM(matrixK[i][j], m);
        }
    }

	if (matrixInverse(matrixK, n, m, matrixInv) < 0) {
		printf("La matriz clave no tiene inversa.\n");
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

			/*for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					fprintf(fout, "%li ", mpz_get_si(matrixC[i][j]));
				}
			}*/
			cipher(strbuf, strbuf, len, matrixK, n, m);
		
			fwrite(strbuf, len, sizeof(char), fout);
			
		} else {
			// Descifrar
			printf("Descifrar\n");
		}
	}

    /*printf("La matriz dada es (modulo m): \n");
    for (i = 0; i < n; i++) {
    	printf("| ");
    	for (j = 0; j < n; j++) {
    		gmp_printf("%Zd ", matrix[i][j]);
    	}
    	printf("|\n");
    }

    init_mpz_matrix(matrixInv, n, n);
    
    if (matrixInv == NULL) {
		free_mpz_matrix(matrixC, n, n);
		free_mpz_matrix(matrixP, n, n);
		free_mpz_matrix(matrixK, n, n);
		mpz_clear(m);
		return EXIT_FAILURE;
	}

    matrixInverse(matrix, n, m, matrixInv);

    printf("Su matriz inversa es: \n");
    for (i = 0; i < n; i++) {
    	printf("| ");
    	for (j = 0; j < n; j++) {
    		gmp_printf("%Zd ", matrixInv[i][j]);
    	}
    	printf("|\n");
    }

    init_mpz_matrix(&matrixRes, n, n);
    
    if (matrixRes == NULL) {
		free_mpz_matrix(matrixC, n, n);
		free_mpz_matrix(matrixP, n, n);
		free_mpz_matrix(matrixK, n, n);
		free_mpz_matrix(matrixInv, n, n);
		mpz_clear(m);
		return EXIT_FAILURE;
	}
	
    mulMatrixMatrix(matrix, matrixInv, matrixRes, n, m);

    printf("El resultado de multiplicar la matriz dada por su inversa es: \n");
    for (i = 0; i < n; i++) {
    	printf("| ");
    	for (j = 0; j < n; j++) {
    		gmp_printf("%Zd ", matrixRes[i][j]);
    	}
    	printf("|\n");
    }*/


    if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	fclose(fk);

	free_mpz_matrix(matrixP, n, n);
	free_mpz_matrix(matrixC, n, n);
	free_mpz_matrix(matrixK, n, n);
	//free_mpz_matrix(matrixInv, n, n);
	//free_mpz_matrix(matrixRes, n, n);
	mpz_clear(m);
	
    return EXIT_SUCCESS;
}

void cipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m) {
	
	int i;
	int j;
	int index = 0;
	int	index2;
	mpz_t** matrixP;
	mpz_t** matrixC;

	init_mpz_matrix(&matrixP, key_size, key_size);
    if (matrixP == NULL) {
		return;
	}

    init_mpz_matrix(&matrixC, key_size, key_size);
    if (matrixC == NULL) {
		free_mpz_matrix(matrixP, key_size, key_size);
		return;
	}

	while (src[index] != '\0') {
		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				if (src[index] == '\0') {
					//Zero pad matrix
					mpz_set_ui(matrixP[i][j], 0L);
					//index2++;					

				} else {
					mpz_set_ui(matrixP[i][j], (long)src[index]);
					index++;
					//index2 = index;
				}
			}
		}
		index2++;

		mulMatrixMatrix(matrixP, key, matrixC, key_size, m);

		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				dst[(index2-1)*key_size*key_size + i*key_size + j] = mpz_get_si(matrixC[i][j]);
			}
		}
	}

	free_mpz_matrix(matrixP, key_size, key_size);
	free_mpz_matrix(matrixC, key_size, key_size);
}

void decipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m) {
	
}
