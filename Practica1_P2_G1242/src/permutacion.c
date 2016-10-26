#include "../includes/algoritmos.h"

void cipher(char* src, char* dst, int* p1, int* p2);
void decipher(char* src, char* dst, int* p1, int* p2);

int main(int argc, char *argv[]) {

	if (argc < 3) {
        printf("Uso: %s {-C|-D} {-1 K1 -2 K2} [-i filein] [-o fileout]\n", argv[0]);
        return 0;
    }

    int modo = -1;
    int c = 0;
    char* strbuf;
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};
    int* k1 = NULL;
    int k1size = 0;
    int* k2 = NULL;
    int k2size = 0;
    int i, j;

    FILE* fin = NULL;
    FILE* fout = NULL;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"C", no_argument, 0, 'C'},
            {"D", no_argument, 0, 'D'},
            {"1", required_argument, 0, '1'},
            {"2", required_argument, 0, '2'},
            {"i", required_argument, 0, 'i'},
            {"o", required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "+CD1:2:i:o:",
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
            case '1':
            	k1 = (int*)calloc(strlen(optarg), sizeof(int));
            	if (k1 == NULL) {
					if (k2 != NULL) {
						free(k2);
					}
					if (fin != NULL) {
						fclose(fin);
					}
					if (fout != NULL) {
						fclose(fout);
					}
					return EXIT_FAILURE;
				}
				for (i = 0, j = 0; optarg[i] != '\0'; i++) {
					//ignore spaces
					while(!isdigit(optarg[i]) && optarg[i+1] != '\0') {
						i++;
					}
					k1[j] = atoi(optarg + i);
					//update only if a number was succesfully read
					if (k1[j] != 0) {
						j++;
						k1size++;
					}
					//ignore read number
					while(isdigit(optarg[i]) && optarg[i+1] != '\0') {
						i++;
					}
				}
            	break;
            case '2':
            	k2 = (int*)calloc(strlen(optarg), sizeof(int));
            	if (k2 == NULL) {
					if (k1 != NULL) {
						free(k1);
					}
					if (fin != NULL) {
						fclose(fin);
					}
					if (fout != NULL) {
						fclose(fout);
					}
					return EXIT_FAILURE;
				}
				for (i = 0, j = 0; optarg[i] != '\0'; i++) {
					//ignore spaces
					while(!isdigit(optarg[i]) && optarg[i+1] != '\0') {
						i++;
					}
					k2[j] = atoi(optarg + i);
					//update only if a number was succesfully read
					if (k2[j] != 0) {
						j++;
						k2size++;
					}
					//ignore read number
					while(isdigit(optarg[i]) && optarg[i+1] != '\0') {
						i++;
					}
				}
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
                printf("Uso: %s {-C|-D} {-k1 K1 -k2 K2} [-i filein] [-o fileout]\n", argv[0]);
				
				if (k1 != NULL) {
					free(k1);
				}
				if (k2 != NULL) {
					free(k2);
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

    if (modo == -1 || k1 == NULL || k2 == NULL) {
        printf("Uso: %s {-C|-D} {-1 K1 -2 K2} [-i filein] [-o fileout]\n", argv[0]);
		if (k1 != NULL) {
			free(k1);
		}
		if (k2 != NULL) {
			free(k2);
		}
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

	strbuf = (char *) calloc (k1size*k2size + 1, sizeof(char));
	if (!strbuf) {
		printf("Error al reservar memoria.\n");
		return EXIT_FAILURE;
	}
	
	int len;

	while (!feof(fin) && !ferror(fin)) {
		if (fin == stdin) {
			fgets(strbuf, k1size*k2size, fin);
			if(feof(fin)) {
				break;
			}

			len = strlen(strbuf);

			if (strbuf[len-1] == '\n') {
				len--;
				strbuf[len] = '\0';			
			}

		} else {
			len = fread(strbuf, sizeof(char), k1size*k2size, fin);
			strbuf[len]='\0';
		}

		printf("Read %d bytes\n", len);
		
		if (modo == CIFRAR) {
			toUpperOnly(strbuf);

			// cipher
			cipher(strbuf, strbuf, k1, k2);
			fprintf(fout, "%s\n", strbuf);
			
		} else {
			
			// decipher
			decipher(strbuf, strbuf, k1, k2);
			fprintf(fout, "%s", strbuf);
		}
	}
	
	free(strbuf);
	/*printf("La clave 1 es: ");
	for (i = 0; k1[i] != 0; i++) {
		printf("%d ", k1[i]);
	}
	putchar('\n');
	printf("La clave 2 es: ");
	for (i = 0; k2[i] != 0; i++) {
		printf("%d ", k2[i]);
	}
	putchar('\n');*/
	
	free(k1);
	free(k2);
	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	return EXIT_SUCCESS;
}

void cipher(char* src, char* dst, int* p1, int* p2) {

	if (!src || !dst || !p1 || !p2) {
		return;
	}

	int i, j;
	int len = strlen(src);
	int rest;
	
	for (i = 0; p1[i] != 0; i++);
	int cols = i;
	for (i = 0; p2[i] != 0; i++);
	int rows = i;
	
	char* p;

	for (rest = len, p = src; rest > 0; rest -= cols*rows, p += cols*rows) {
		
		// Añadimos padding si es necesario
		if (rest < cols*rows) {
			for (i = rest; i < cols*rows; i++) {
				src[i] = 'A';
			}
		}

		for(i = 0; i < cols; i++) {
			for (j = 0; j < rows; j++) {
				if (p[j*rows+i] == p[j*rows + p1[i] - 1]) {
					continue;
				}
				//memory-efficient swap
				printf("Swapcols %hhx and %hhx\n", p[j*rows+i], p[j*rows + p1[i] - 1]);
				p[j*rows+i] += p[j*rows + p1[i] - 1];
				p[j*rows + p1[i] - 1] = p[j*rows+i] - p[j*rows + p1[i] - 1];
				p[j*rows+i] = p[j*rows+i] - p[j*rows + p1[i] - 1];
				printf("Now %hhx and %hhx\n", p[j*rows+i], p[j*rows + p1[i] - 1]);
			}
		}
		
		for(i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				if (p[i*rows + j] == p[(p2[i] - 1) * rows + j]) {
					continue;
				}
				//memory-efficient swap
				printf("Swaprow %hhx and %hhx\n", p[i*rows + j],  p[(p2[i] - 1) * rows + j]);
				p[i*rows + j] += p[(p2[i] - 1) * rows + j];
				p[(p2[i] - 1) * rows + j] = p[i*rows + j] - p[(p2[i] - 1) * rows + j];
				p[i*rows + j] = p[i*rows + j] - p[(p2[i] - 1) * rows + j];
				printf("Now %hhx and %hhx\n", p[i*rows + j],  p[(p2[i] - 1) * rows + j]);
			}
		}
	}
}

void decipher(char* src, char* dst, int* p1, int* p2) {

	if (!src || !dst || !p1 || !p2) {
		return;
	}

	int i, j;
	int len = strlen(src);
	int rest;
	
	for (i = 0; p1[i] != 0; i++);
	int cols = i;
	for (i = 0; p2[i] != 0; i++);
	int rows = i;
	
	char* p;

	for (rest = len, p = src; rest > 0; rest -= cols*rows, p += cols*rows) {
		
		// Añadimos padding si es necesario
		if (rest < cols*rows) {
			for (i = rest; i < cols*rows; i++) {
				src[i] = 'A';
			}
		}
		
		for(i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				if (p[i*rows + j] == p[(p2[i] - 1) * rows + j]) {
					continue;
				}
				//memory-efficient swap
				p[i*rows + j] += p[(p2[i] - 1) * rows + j];
				p[(p2[i] - 1) * rows + j] = p[i*rows + j] - p[(p2[i] - 1) * rows + j];
				p[i*rows + j] = p[i*rows + j] - p[(p2[i] - 1) * rows + j];
			}
		}
		
		for(i = 0; i < cols; i++) {
			for (j = 0; j < rows; j++) {
				if (p[j*rows+i] == p[j*rows + p1[i] - 1]) {
					continue;
				}
				//memory-efficient swap
				p[j*rows+i] += p[j*rows + p1[i] - 1];
				p[j*rows + p1[i] - 1] = p[j*rows+i] - p[j*rows + p1[i] - 1];
				p[j*rows+i] = p[j*rows+i] - p[j*rows + p1[i] - 1];
			}
		}
	}
}