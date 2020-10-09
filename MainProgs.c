#include <stdio.h>
#include "MainPrg.h"
#include "ParamReader.h"
#include "TellerHead.h"
#include "CustomerHead.h"
#include "queueManHead.h"

const int false = 0;
const int true = 1;

const int cMAXTELLER = 3;

//tShVariables ShVar;

void errorHandler(int errType)
{
  switch (errType)
    {
      case 1 : 
        { 
          fprintf(stderr, "Can not invoke fork\n");
          break;
        }
      case 2 : 
        {
          fprintf(stderr, "Can not read customer_file\n");
          break;
        }
      default : break; //no error, do nothing
    }
}

int RunProcess(int ReadInterval, int* LiveTime, 
                int TimeWithdraw, int TimeDeposit)
{
  int ProcessNo, ownProcessNo, i;
  pthread_t threads [cMAXTELLER]; //storing for the child's PID
  tThreadParameter ThreadParam [cMAXTELLER];

  pthread_mutex_init(&fileAccessMutex, NULL);
  pthread_mutex_init(&messageMutex, NULL);
  pthread_mutex_init(&noOfThreadMutex, NULL);
  pthread_mutex_init(&sharedInfoMutex, NULL);
  pthread_mutex_init(&condWaitMutex, NULL);

  pthread_cond_init(&conditionVar, NULL);

  int errorLog = 0;//for error checking

  FILE *logFile;
  logFile=fopen(logFileName, "w");
//  logFile=stdout;
  if (logFile==NULL) logFile=stdout;

  fprintf(logFile, "Starting simulation\n");
  fprintf(logFile, "-------------------\n\n");

  fclose(logFile);

  for (i=0; i<cMAXTELLER; i++)
    {
      ThreadParam[i].TimeWithdraw=TimeWithdraw;
      ThreadParam[i].TimeDeposit=TimeDeposit;
    }

  headQueue=(tPtr *) createNewQueue(sizeof(tPtr));
  tailQueue=headQueue;

  ShVar.noOfChildProcess=0;
  ShVar.moreCustomer=true;
//end of initiaizing shared variables

//initiaising message Queue ID

  ownProcessNo=0;//this is for indicating the own process no
//0 means parents, 1..3 means childs

//initiaisation for the loop
  ProcessNo=1; 
  while ((ProcessNo<=cMAXTELLER) && (errorLog==0))
    {
//create thread here    
      ThreadParam[ProcessNo-1].LiveTime=LiveTime[ProcessNo-1];
      ThreadParam[ProcessNo-1].ThreadNumber=ProcessNo;

      errorLog=pthread_create(&threads[ProcessNo-1], NULL, Teller, 
        (void *) &ThreadParam[ProcessNo-1]);
//Teller(ownProcessNo, timeOut, TimeWithdraw, 
//               TimeDeposit, ShVar);

      ProcessNo++;
    }

  Customer(ReadInterval, threads); 

  if (ownProcessNo==0)
    {  
//      printf("cleaning up\n");

      removeQueue((void *) headQueue);
    }

  if ((errorLog==0) && (ownProcessNo==0)) //parent process will go here
    {
      pthread_mutex_destroy(&fileAccessMutex);
      pthread_mutex_destroy(&messageMutex);
      pthread_mutex_destroy(&noOfThreadMutex);
      pthread_mutex_destroy(&sharedInfoMutex);
      pthread_mutex_destroy(&condWaitMutex);

      pthread_cond_destroy(&conditionVar);

      pthread_exit(NULL);
//      if (Customer(ReadInterval, ShVar, ChildPIDs)==-1) 
//        errorLog=2;
    }

  return errorLog;
}

int main(int argc, char** argv)
{
  int ReadInterval, TimeWithdraw, TimeDeposit;
  int LiveTime [cMAXTELLER];
  int ParamCheck;

  ParamCheck=getParameter(&ReadInterval, LiveTime, 
                          &TimeWithdraw, &TimeDeposit, 
                          argc, argv);

  if (ParamCheck==cPARAMETERVALID)
    {
      errorHandler(RunProcess(ReadInterval, LiveTime, 
                      TimeWithdraw, TimeDeposit));      
    }
  else
    writeParameterError(ParamCheck);

  return 0;
}
