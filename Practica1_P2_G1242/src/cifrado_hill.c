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
    int cipher_len;
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
			strbuf[len]='\0';
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
			
			//ceiling with respect to n*n not to lose information about the last matrix
			//floor first
			cipher_len = (len/(n*n))*(n*n);
			//add n*n if len wasn't already % n*n
			if ((len % (n*n)) != 0) {
				cipher_len += n*n;
			}

			for (i=0; i< cipher_len; i++) {
				printf("%02x\t", strbuf[i]);
			}

			putchar('\n');
			fwrite(strbuf, cipher_len, sizeof(char), fout);
			
		} else {
			decipher(strbuf, strbuf, len, matrixK, n, m);

			cipher_len = (len/(n*n))*(n*n);
			//add n*n if len wasn't already % n*n
			if ((len % (n*n)) != 0) {
				cipher_len += n*n;
			}

			for (i=0; i< cipher_len; i++) {
				printf("%02x\t", strbuf[i]);
			}

			putchar('\n');
			fwrite(strbuf, cipher_len, sizeof(char), fout);
		}
	}


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
	mpz_clear(m);
	
    return EXIT_SUCCESS;
}

//will write up to nearest multiple of key_size^2 greater or equal to size
void cipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m) {
	
	int i;
	int j;
	int flag = 0;
	int index = 0;
	int	index2 = 0;
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

	while (!flag) {
		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				if (src[index] == '\0') {
					//Zero pad matrix
					mpz_set_ui(matrixP[i][j], 0L);
					flag = 1;					

				} else {
					mpz_set_ui(matrixP[i][j], ((long)src[index] - 65));
					index++;
					//index2 = index;
				}
			}
		}
		

		mulMatrixMatrix(matrixP, key, matrixC, key_size, m);

		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				dst[index2 + i*key_size + j] = (mpz_get_si(matrixC[i][j]) + 65);
			}
		}
		
		index2 += key_size*key_size;
	}

	free_mpz_matrix(matrixP, key_size, key_size);
	free_mpz_matrix(matrixC, key_size, key_size);
}

//will assume there's something to read up to a multiple of key_size^2
void decipher(char* src, char* dst, int size, mpz_t** key, int key_size, mpz_t m) {
	int i;
	int j;
	int index = 0;
	int	index2 = 0;
	mpz_t** matrixP;
	mpz_t** matrixC;
	mpz_t** matrixK_Inv;

	init_mpz_matrix(&matrixP, key_size, key_size);
    if (matrixP == NULL) {
		return;
	}

    init_mpz_matrix(&matrixC, key_size, key_size);
    if (matrixC == NULL) {
		free_mpz_matrix(matrixP, key_size, key_size);
		return;
	}
	
	init_mpz_matrix(&matrixK_Inv, key_size, key_size);
    if (matrixK_Inv == NULL) {
		free_mpz_matrix(matrixP, key_size, key_size);
		free_mpz_matrix(matrixC, key_size, key_size);
		return;
	}

	printf("Matriz dada: \n");
	for (i = 0; i < key_size; i++) {
		gmp_printf("|%Zd %Zd %Zd|\n", key[i][0], key[i][1], key[i][2]);
	}

	if (matrixInverse(key, key_size, m, matrixK_Inv) < 0) {
		printf("La matriz de claves no tiene inversa.\n");
		return;
	}

	printf("Matriz inversa: \n");
	for (i = 0; i < key_size; i++) {
		gmp_printf("|%Zd %Zd %Zd|\n", matrixK_Inv[i][0], matrixK_Inv[i][1], matrixK_Inv[i][2]);
	}

	while (index < size) {
		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				mpz_set_ui(matrixC[i][j], ((long)src[index] - 65));
				index++;
			}
		}
		

		mulMatrixMatrix(matrixC, matrixK_Inv, matrixP, key_size, m);

		for (i = 0; i < key_size; i++) {
			for (j = 0; j < key_size; j++) {
				dst[index2 + i*key_size + j] = (mpz_get_si(matrixP[i][j]) + 65);
			}
		}
		
		index2 += key_size*key_size;
	}

	free_mpz_matrix(matrixP, key_size, key_size);
	free_mpz_matrix(matrixC, key_size, key_size);
	free_mpz_matrix(matrixK_Inv, key_size, key_size);
}
