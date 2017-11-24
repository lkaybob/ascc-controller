CC=arm-none-linux-gnueabi-gcc

boardExec: requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o
	$(CC) -o boardExec requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o

client-cli: client-cli.c requestModule.h keyMatrix.h tlcdWrapper.h
	$(CC) -c client-cli.c

requestModule: requestModule.c
	$(CC) -c requestModule.c

keyMatrixModule: keyMatrix.c
	$(CC) -c keyMatrix.c

tlcdModule: tlcdWrapper.c tlcdDefine.h
	$(CC) -c tlcdWrapper.c

clean:
	rm *.o boardExec
