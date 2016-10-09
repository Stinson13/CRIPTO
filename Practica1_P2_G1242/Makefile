CC=gcc
EXE1=afin
EXE2=afinflujo
EXE3=hill
SRC=src/
INC= includes/
LIB=lib/

todo: $(EXE1) $(EXE2) $(EXE3)
	
$(EXE1): $(SRC)cifrado_afin.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_afin.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
$(EXE2): $(SRC)cifrado_afin_+.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_afin_+.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
$(EXE3): $(SRC)cifrado_hill.c $(LIB)libgmp.a
	$(CC) -o $@ $(SRC)cifrado_hill.c $(SRC)algoritmos.c -L. $(LIB)libgmp.a
	
native: $(SRC)algoritmos.c $(SRC)cifrado_afin.c $(SRC)cifrado_afin_+.c $(SRC)cifrado_hill.c
	$(CC) -o $(EXE1) $(SRC)cifrado_afin.c $(SRC)algoritmos.c -lgmp
	$(CC) -o $(EXE2) $(SRC)cifrado_afin_+.c $(SRC)algoritmos.c -lgmp
	$(CC) -g -o $(EXE3) $(SRC)cifrado_hill.c $(SRC)algoritmos.c -lgmp
	
clean:
	rm -f $(EXE1) $(EXE2) $(EXE3)
