#include "MainPrg.h"
#include <stdio.h>
#include "TimeManageHead.h"
#include "queueManHead.h"

const char *customerFileName = "customer_file";

int readCustomerFile(FILE *customerFile, tCustomer *NextCustomer)
{
  int cNo, success=0;
  char cType[9];

  if (EOF!=fscanf(customerFile, "%d %s", &cNo, cType))
    {
      NextCustomer->customerNo=cNo;
      NextCustomer->Transaction=cType[0];
      NextCustomer->arrivalTime=time(NULL);
//only character w or d is enough to differentiate between
//deposit or withdrawal
    }
  else
    success=-1;

  return success;
}

void writeCustomerLog(tCustomer Customer)
{
  pthread_mutex_lock(&fileAccessMutex);
//  int errorLog;
  FILE *logFile;
  char *buf;

  logFile=fopen(logFileName, "a");

//  logFile=stdout; // for debug

//if cannot open for output then set output to standard output
  if (logFile==NULL) logFile=stdout;
 
  fprintf(logFile, "-------------------------------------\n");
  fprintf(logFile, "Customer %d: ", Customer.customerNo);

  switch (Customer.Transaction)
    {
      case 'd' : fprintf(logFile, "Deposit\n"); break;
      case 'w' : fprintf(logFile, "Withdrawal\n"); break;
    }

  fprintf(logFile, "Arrival time: ");
  buf=formatTime(Customer.arrivalTime);
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\n-------------------------------------\n\n");

  fclose(logFile);
  pthread_mutex_unlock(&fileAccessMutex);
}

int Customer(int ReadInterval, pthread_t *threads)
{
  FILE *customerFile;
  tCustomer nextCustomer;
//  CustomerQueueMsg custBuf;
  int success = 0;
//  int errorLog = 0;
  int waitLoop;

//  int runtime=0; //this is used to approximate the run time of the customer

  customerFile=fopen("customer_file", "r");
 
  if (customerFile!=NULL)
    {
      while (readCustomerFile(customerFile, &nextCustomer)==0)  
        {
          writeCustomerLog(nextCustomer); 

          tailQueue->data=nextCustomer;
          tailQueue=(tPtr *) createNextQueue((void *) tailQueue);

          pthread_mutex_lock(&condWaitMutex);
          pthread_cond_broadcast(&conditionVar);
          pthread_mutex_unlock(&condWaitMutex);

          sleep(ReadInterval);
        }
    }
  else
    {
      success=-1;      
    } 

//this is used to notify whether there were more customer or not
//it is no need to mutually exclusive, since the other file just
//reading
  ShVar.moreCustomer=false;

//wait until each of the child thread terminated
  for (waitLoop=0; waitLoop<cMAXTELLER; waitLoop++)
    {
      pthread_join(threads[waitLoop], NULL);
    }

  while (!isEmptyQueue((void *) headQueue)) 
    {
      printf("%d\n", (headQueue->data).customerNo);
      headQueue=removeFirstQueue((void *) headQueue);
    }

  fclose(customerFile);

  return success;
}
