conectividade: conectividade.o graphopmatrixdi.o
	gcc -o conectividade conectividade.o graphopmatrixdi.o

conectividade.o: conectividade.c graphmatrix.h
	gcc -c conectividade.c

graphopmatrixdi.o: graphopmatrixdi.c graphmatrix.h
	gcc -c graphopmatrixdi.c