CC=arm-none-linux-gnueabi-gcc

boardExec: requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o jsmn.o dotMatrixWorker.o 7segWorker.o oledWrapper.o
	$(CC) -o boardExec requestModule.o client-cli.o keyMatrix.o tlcdWrapper.o jsmn.o dotMatrixWorker.o 7segWorker.o oledWrapper.o -lpthread

client-cli: client-cli.c requestModule.h keyMatrix.h tlcdWrapper.h dotMatrixWorker.h oledWrapper.h
	$(CC) -c client-cli.c

requestModule: requestModule.c
	$(CC) -c requestModule.c

keyMatrixModule: keyMatrix.c
	$(CC) -c keyMatrix.c

tlcdModule: tlcdWrapper.c tlcdDefine.h
	$(CC) -c tlcdWrapper.c

jsmn: jsmn.c jsmn.h
	$(CC) -c jsmn.c

dotMatirxWorker: dotMatrixWorker.c
	$(CC) -c dotMatrixWorker.c

7segWorker: 7segWorker.c
	$(CC) -c 7segWorker.c

oledModule: oledWrapper.c
	$(CC) -c oledWrapper.c
clean:
	rm *.o boardExec
