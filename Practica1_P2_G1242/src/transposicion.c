#include "../includes/algoritmos.h"

#define NOM_FILEPERM "permutacion.dat"

void cipher(char* src, char* dst, char* permutation);
void descipher(char* src, char* dst, char* permutation);

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Uso: %s {-C|-D} {-p permutacion | -n Nperm} [-i filein] [-o fileout]\n", argv[0]);
        return 0;
    }

    int modo = -1;
    int c = 0;
    int n = -1;					// Tamaño permutacion
    int len;					// Tamaño texto a cifrar
    int i;
    char* auxPerm;
    char* strbuf;
    char* permutation = NULL;
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};

    FILE* fin = NULL;
    FILE* fout = NULL;
    FILE* fperm = NULL;

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
            	permutation = (char *) calloc (MAX_STR, sizeof(char));
				if (!permutation) {
					return EXIT_FAILURE;
				}

				auxPerm = strtok(optarg, " ");
				//memcpy(&permutation[0], atoi(auxPerm), sizeof(char));
				permutation[0] = atoi(auxPerm);

				for (i = 1; (auxPerm = strtok(NULL, " ")) != NULL; i++) {
					//memcpy(&permutation[i], auxPerm, sizeof(char));
					permutation[i] = atoi(auxPerm);
				}
            	break;
            case 'n':
            	n = atoi(optarg);

            	if (!n) {
            		printf("El valor de 'n' debe ser mayor o igual a 1.\n");
            		return EXIT_FAILURE;
            	}
            	permutation = (char *) calloc ((n + 1), sizeof(char));
				if (!permutation) {
					return EXIT_FAILURE;
				}

            	makePermutation(permutation, n);
            	if (!permutation) {
            		printf("Error al crear una nueva permutacion.\n");
            		return EXIT_FAILURE;
            	}

            	fperm = fopen(NOM_FILEPERM, "w");
            	if (fperm == NULL) {
                    printf("Error al abrir %s para leer\n", NOM_FILEPERM);
                    if (fout != NULL) {
						fclose(fout);
					}
					if (fin != NULL) {
						fclose(fout);
					}
                    return EXIT_FAILURE;
                }

                for (i = 0; i < n; i++) {
                	fprintf(fperm, "%d ", (int)permutation[i]);
                }

                fclose(fperm);
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

    if (!permutation || modo == -1) {
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

	while (!feof(fin) && !ferror(fin)) {
		strbuf = (char *) calloc (MAX_STR, sizeof(char));
		if (!strbuf) {
			printf("Error al reservar memoria.\n");
			return EXIT_FAILURE;
		}

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

			cipher(strbuf, strbuf, permutation);
			if (!strbuf) {
				printf("Error al cifrar el texto plano.\n");
				return EXIT_FAILURE;
			}

			fprintf(fout, "%s\n", strbuf);
			
		} else {
			
			descipher(strbuf, strbuf, permutation);
			if (!strbuf) {
				printf("Error al cifrar el texto plano.\n");
				return EXIT_FAILURE;
			}

			fprintf(fout, "%s", strbuf);
		}

		free(strbuf);
		//printf("\n");
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

void cipher(char* src, char* dst, char* permutation) {

	if (!src || !dst || !permutation) {
		return;
	}

	int i;
	int rest;
	int index1 = 0;
	int index2 = 0;
	int len = strlen(src);
	int n = strlen(permutation);
	int auxN = strlen(permutation);
	char* auxSrc;

	auxSrc = (char *) calloc ((n + 1), sizeof(char));
	if (!auxSrc) {
		dst = NULL;
		return;
	}

	// Añadimos padding si es necesario
	if ((len%n)) {
		rest = (len - n);

		if (rest < 0) {
			for (i = len; i < n; i++) {
				memcpy(&src[i], "A", sizeof(char));
			}

		} else if (rest > 0) {
			rest = len / n;
			rest = (rest * n) + n;

			for (i = len; i < rest; i++) {
				memcpy(&src[i], "A", sizeof(char));
			}
		}
	}

	// Tamaño de la cadena origen tras el padding
	len = strlen(src);

	while (index1 < len) {
		for (i = 0; index1 < auxN; i++, index1++){
			memcpy(&auxSrc[(int)permutation[i] - 1], &src[index1], sizeof(char));
		}

		for (i = 0; i < n; i++, index2++) {
			memcpy(&dst[index2], &auxSrc[i], sizeof(char));
		}

		if (auxN < len) {
			auxN += n;
		}
	}

	memcpy(&dst[index2], "\0", sizeof(char));

	free(auxSrc);
	return;
}

void descipher(char* src, char* dst, char* permutation) {

	if (!src || !dst || !permutation) {
		return;
	}

	int i;
	int rest;
	int index1 = 0;
	int index2 = 0;
	int len = strlen(src);
	int n = strlen(permutation);
	char* auxSrc;

	auxSrc = (char *) calloc ((n + 1), sizeof(char));
	if (!auxSrc) {
		dst = NULL;
		return;
	}

	while (index1 < len) {
		for (i = 0; i < n; i++, index1++) {
			memcpy(&auxSrc[i], &src[index1], sizeof(char));
		}

		for (i = 0; i < n; i++, index2++){
			memcpy(&dst[index2], &auxSrc[(int)permutation[i] - 1], sizeof(char));
		}
	}

	memcpy(&dst[index2], "\0", sizeof(char));

	free(auxSrc);
	return;
}

