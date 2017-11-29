CC=arm-none-linux-gnueabi-gcc

boardExec: requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o jsmn.o
	$(CC) -o boardExec requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o jsmn.o

client-cli: client-cli.c requestModule.h keyMatrix.h tlcdWrapper.h
	$(CC) -c client-cli.c

requestModule: requestModule.c
	$(CC) -c requestModule.c

keyMatrixModule: keyMatrix.c
	$(CC) -c keyMatrix.c

tlcdModule: tlcdWrapper.c tlcdDefine.h
	$(CC) -c tlcdWrapper.c

jsmn: jsmn.c jsmn.h
	$(CC) -c jsmn.c

clean:
	rm *.o boardExec
