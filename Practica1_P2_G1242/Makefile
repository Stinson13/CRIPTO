CC=gcc
EXE1=afin
EXE2=afinflujo
EXE3=hill
EXE4=vignere
EXE5=kasiski
EXE6=transposicion
EXE7=ioc
EXE8=permutacion
SRC=src/
INC= includes/
LIB=lib/

todo: $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(EXE6) $(EXE7) $(EXE8)
	
$(EXE1): $(SRC)cifrado_afin.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_afin.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
$(EXE2): $(SRC)cifrado_afin_+.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_afin_+.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
$(EXE3): $(SRC)cifrado_hill.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_hill.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a

$(EXE4): $(SRC)cifrado_vignere.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_vignere.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a

$(EXE5): $(SRC)kasiski.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)kasiski.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
$(EXE7): $(SRC)indexofcoincidence.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)indexofcoincidence.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a	

$(EXE8):$(SRC)permutacion.c $(SRC)algoritmos.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)permutacion.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a

native: $(SRC)algoritmos.c $(SRC)cifrado_afin.c $(SRC)cifrado_afin_+.c $(SRC)cifrado_hill.c $(SRC)cifrado_vignere.c $(SRC)kasiski.c $(SRC)transposicion.c
	$(CC) -g -o $(EXE1) $(SRC)cifrado_afin.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE2) $(SRC)cifrado_afin_+.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE3) $(SRC)cifrado_hill.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE4) $(SRC)cifrado_vignere.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE5) $(SRC)kasiski.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE6) $(SRC)transposicion.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE7) $(SRC)indexofcoincidence.c $(SRC)algoritmos.c -lgmp -lm
	$(CC) -g -o $(EXE8) $(SRC)permutacion.c $(SRC)algoritmos.c -lgmp -lm
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(EXE6) $(EXE7) $(EXE8)
