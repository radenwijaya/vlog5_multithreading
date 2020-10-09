CC=gcc
CFLAGS=-I
DEPS=MainPrg.h CustomerHead.h ParamReader.h queueManHead.h TellerHead.h TimeManageHead.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CLFAGS)

part_2: MainProgs.c ParameterReader.c queueManFile.c CustomerFile.c TellerFile.c TimeManageFile.c
	$(CC) -pthread -o part_2 MainProgs.c ParameterReader.c queueManFile.c CustomerFile.c TellerFile.c TimeManageFile.c
