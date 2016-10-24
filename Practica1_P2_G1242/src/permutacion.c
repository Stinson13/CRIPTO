#include "../includes/algoritmos.h"

int main(int argc, char *argv[]) {

	if (argc < 3) {
        printf("Uso: %s {-C|-D} {-k1 K1 -k2 K2} [-i filein] [-o fileout]\n", argv[0]);
        return 0;
    }

    int modo = -1;
    int c = 0;
    char* strbuf;
    char filein[MAX_STR] = {0};
    char fileout[MAX_STR] = {0};

    FILE* fin = NULL;
    FILE* fout = NULL;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"C", no_argument, 0, 'C'},
            {"D", no_argument, 0, 'D'},
            {"k1", required_argument, 0, 'k1'},
            {"k2", required_argument, 0, 'k2'},
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
            case 'k1':
            	
            	break;
            case 'k2':
            	
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

                if (fin != NULL) {
					fclose(fin);
				}
				if (fout != NULL) {
					fclose(fout);
				}
                return EXIT_FAILURE;
        }
    }

    if (modo == -1) {
        printf("{-C|-D} {-k1 K1 -k2 K2} son obligatorios\n");
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

			// cipher

			//fprintf(fout, "%s\n", strbuf);
			
		} else {
			
			// descipher

			//fprintf(fout, "%s", strbuf);
		}

		free(strbuf);
	}

	if (fin != NULL) {
		fclose(fin);
	}
	if (fout != NULL) {
		fclose(fout);
	}

	return EXIT_SUCCESS;
}