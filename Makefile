
all: MATH MATRIX 
 
MATRIX:
	make -C matrix

MATH:
	make -C math

clean:
	rm ./matrix/obj/*.o
	rm ./math/*.o
