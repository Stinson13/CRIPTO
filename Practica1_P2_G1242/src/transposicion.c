#include "../includes/algoritmos.h"

char* cipher(char* src, char* permutation);

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Uso: %s {-C|-D} {-p permutacion | -n Nperm} [-i filein] [-o fileout]\n", argv[0]);
        return 0;
    }

    int modo = -1;
    int c = 0;
    int n = -1;					// Tamaño permutacion
    int i;
    char* permutation = NULL;
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};

    FILE* fin = NULL;
    FILE* fout = NULL;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"C", no_argument, 0, 'C'},
            {"D", no_argument, 0, 'D'},
            {"p", required_argument, 0, 'p'},
            {"n", required_argument, 0, 'n'},
            {"i", required_argument, 0, 'i'},
            {"o", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "+CDp:n:i:o:",
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
            case 'p':
            	// TODO: leer cadena entrada (permutacion)
            	break;
            case 'n':
            	n = atoi(optarg);

            	if (!n) {
            		printf("El valor de 'n' debe ser mayor o igual a 1.\n");
            		return EXIT_FAILURE;
            	}

            	permutation = makePermutation(n);

            	if (!permutation) {
            		printf("Error al crear una nueva permutacion.\n");
            		return EXIT_FAILURE;
            	}

            	//TODO: imprimir matriz en fichero tambien
            	printf("La permutacion aleatoria es: '");
            	for (i = 0; i < n; i++) {
            		printf("%d", permutation[i]);
            	}

            	printf("'\n");
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
                printf("Uso: %s {-C|-D} {-p permutacion | -n Nperm} [-i filein] [-o fileout]\n", argv[0]);

                if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
                return EXIT_FAILURE;
        }
    }

    if (modo != CIFRAR && n != -1) {
    	printf("El parametro 'n' solo esta disponible con la opcion -C.\n");
    	return EXIT_FAILURE;
    }

    if (modo == -1) {
        printf("{-C|-D} {-p permutacion | -n Nperm} son obligatorios\n");
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
	int len;				// Tamaño texto a cifrar
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

		printf("Read %d bytes", len);
		
		if (modo == CIFRAR) {
			
			char* encryption = cipher(strbuf, permutation);
			strcpy(strbuf, encryption);
			fwrite(strbuf, len, sizeof(char), fout);
			free(encryption);
			
		} else {
			
			//TODO: descifrado
			//fwrite(strbuf, cipher_len, sizeof(char), fout);
		}

		printf("\n");
	}

	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	free(permutation);

	return EXIT_SUCCESS;
}

char* cipher(char* src, char* permutation) {

	if (!src || !permutation) {
		return NULL;
	}

	int i;
	int rest;
	int index1 = 1;
	int index2 = 0;
	int len = strlen(src);
	int n = strlen(permutation);
	char* auxSrc;
	char* dst;

	auxSrc = (char *) malloc (n * sizeof(char));
	if (!auxSrc) {
		return NULL;
	}

	dst = (char *) malloc (len * sizeof(char));
	if (!dst) {
		free(auxSrc);
		return NULL;
	}

	rest = (len - n);

	if (rest < 0 || !rest) {
		while (index1 < len) {
			for (i = 0; i < n; i++, index1++) {
				auxSrc[(int)permutation[i] - 1] = src[i];
			}

			for (i = 0; i < n; i++, index2++) {
				dst[index2] = auxSrc[i];
			}
		}

	} else if (rest > 0) {
		while ((index1 + rest) < len) {
			for (i = 0; i < n; i++, index1++) {
				auxSrc[(int)permutation[i] - 1] = src[i];
			}

			for (i = 0; i < n; i++, index2++) {
				dst[index2] = auxSrc[i];
			}
		}

		for (i = 0; index1 < len; i++, index1++) {
			auxSrc[(int)permutation[i] - 1] = src[i];
		}

		for (i = 0; i < rest; i++, index2++) {
			dst[index2] = auxSrc[i];
		}
	}

	free(auxSrc);
	return dst;
}

