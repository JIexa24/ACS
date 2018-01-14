
all: MATRIX 
 
MATRIX:
	make -C matrix

ASM:
	make -C asm

clean:
	rm ./matrix/obj/*.o
	rm ./math/*.o
