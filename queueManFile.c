#include "queueManHead.h"

void *createNewQueue(int sizeofNewPtr)
{
  void *newPtr;
  newPtr=malloc(sizeofNewPtr);
  ((tPtr *) newPtr)->next = NULL;
  ((tPtr *) newPtr)->data.Transaction = 'n';
//set the data to 'n' which means null
  return newPtr;
}

void *createNextQueue(void *LastPtr) 
//this is used to create the next element after last in 
//the queue.  this return the last element in the queue
{
  void *nextPtr;

  if (LastPtr!=NULL)
    {
      tPtr *localLastPtr = (tPtr *) LastPtr;

      nextPtr=createNewQueue(sizeof(tPtr));

      localLastPtr->next=nextPtr;
//point last pointer's next to the new pointer

      LastPtr=nextPtr;
//change the last to the new pointer
    }

  return LastPtr;
}

void *removeFirstQueue(void *FirstPtr)
{
  void *FirstBufPtr;

  if (FirstPtr!=NULL)
    {
      FirstBufPtr=FirstPtr;

      if (((tPtr *) FirstPtr)->next!=NULL)
//not the last element of the queue or not the only element of the queue
        {
          FirstPtr=((tPtr *) FirstPtr)->next;
          free(FirstBufPtr);
        }
      else
//the last or only element of the queue then set data to null
        ((tPtr *) FirstPtr)->data.Transaction='n';

    }

  return FirstPtr;
}

int isEmptyQueue(void *FirstPtr)
{
  return (((tPtr *) FirstPtr)->data.Transaction=='n');
}

void removeQueue(void *FirstPtr)
{
  free(FirstPtr);
}
