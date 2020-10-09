#ifndef QUEUEMANHEAD_H
#define QUEUEMANHEAD_H

#include <stdlib.h>
#include "MainPrg.h"

typedef struct  
  {
    void *next;
    tCustomer data;
  } tPtr;

void *createNewQueue(int sizeofNewPtr);
void *createNextQueue(void *LastPtr);
void *removeFirstQueue(void *FirstPtr);
int isEmptyQueue(void *FirstPtr);
void removeQueue(void *FirstPtr);

tPtr *headQueue, *tailQueue;
#endif

