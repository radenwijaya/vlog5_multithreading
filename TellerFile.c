#include "TellerHead.h"
#include "MainPrg.h"
#include <stdio.h>
#include "TimeManageHead.h"
#include "queueManHead.h"

const int cTELLERTIMEDOUT = 301;
const int cTELLERNONEXTCUSTOMER = 302;
const int cTELLERNOMORECUSTOMER = 303;
const int cTELLERTAKEMODE = 1;
const int cTELLERFINISHMODE = 2;
const int cTELLERDIE = 3;
const char *logFileName = "simulation_log";

time_t initTeller()
{
//critical section
  pthread_mutex_lock(&noOfThreadMutex);
  ShVar.noOfChildProcess++;
  pthread_mutex_unlock(&noOfThreadMutex);
//end of critical section

  return time(NULL);
}

void doneTeller(time_t startTime, int ownProcessNo)
{
  int teller;
  FILE *logFile;
  char *buf;

  pthread_mutex_lock(&fileAccessMutex);
//critical section
  logFile=fopen(logFileName, "a");
//  logFile=stdout; //for debug only
  if (logFile==NULL) logFile=stdout;
//give output to stdout log file cannot be used
  
  pthread_mutex_lock(&noOfThreadMutex);
  ShVar.noOfChildProcess--;//decrease the number of child
  pthread_mutex_unlock(&noOfThreadMutex);

  fprintf(logFile, "Teller-%d Termination:\n", ownProcessNo);
  fprintf(logFile, "----------------------\n");

  fprintf(logFile, "Start time: ");
  buf=formatTime(startTime);
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\nTermination time: ");
  buf=formatTime(time(NULL));
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\n\n");

  if (ShVar.noOfChildProcess==0)//after all of the child has gone
    {
      fprintf(logFile, "Teller Statistic\n");
      fprintf(logFile, "----------------\n");

      for (teller=0; teller<cMAXTELLER; teller++)
        fprintf(logFile, "Teller-%d running time: %d CPU second(s)\n",
                teller+1, ShVar.TellerRunTime[teller]);

      fprintf(logFile, "Number of customer(s) %d of customer(s)\n", 
              ShVar.NoOfCustomer);
      fprintf(logFile, "Average waiting time: %.2f second(s)\n",
              (float) ShVar.WaitingTime/(float) ShVar.NoOfCustomer);
      fprintf(logFile, "Average turn around time: %.2f second(s)\n\n",
              (float) ShVar.TurnaroundTime/(float) ShVar.NoOfCustomer);
    }

//end of critical section
  fclose(logFile);

  pthread_mutex_unlock(&fileAccessMutex);

  pthread_exit(NULL);
}

void writeTellerLog(tCustomer Customer, int ownProcessNo, int mode)
{
  pthread_mutex_lock(&fileAccessMutex);

  FILE *logFile;
  char *buf;

  logFile=fopen(logFileName, "a");
//  logFile=stdout;

  if (logFile==NULL) logFile=stdout;
 
  fprintf(logFile, "Customer statistic for Teller-%d\n", ownProcessNo);
  fprintf(logFile, "-------------------------------\n");
  fprintf(logFile, "Customer %d\n", Customer.customerNo);
  fprintf(logFile, "Arrival time: ");
  buf=formatTime(Customer.arrivalTime);
  fprintf(logFile, buf);
  free(buf);

  if (mode==cTELLERTAKEMODE)
    {
      fprintf(logFile, "\nResponse time: ");
    }
  else
    {
      fprintf(logFile, "\nCompletion time: ");
    }

  buf=formatTime(time(NULL));
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\n\n");

  fclose(logFile);
  pthread_mutex_unlock(&fileAccessMutex);
}

int getMessages(int remActiveTime, tCustomer *nextCustomer, int ownProcessNo)
{
  int result = 0;
//put mutex

  if (!isEmptyQueue((void *) headQueue))
    {
      *nextCustomer=headQueue->data;
      headQueue=removeFirstQueue((void *) headQueue);
    }
  else
    {
      result = cTELLERNONEXTCUSTOMER;
    }

  return result;
}

//this is a new thread for telling current thread when the time is up
//this thread is executed by teller just before the teller will sleep
//in order waiting the customer (when there were no customer in the queue)
//the sleep is simulated using mutex.  Time out check tread will release
//or sending specific signal to it's parent's thread in order to make it's 
//parent responsive.
void *TimeOutCheckThread(void *ownParentNo)
{
  int *ParentNo;
  ParentNo=(int *) ownParentNo;
  sleep(ShVar.TellerRemTime[*ParentNo-1]);

  pthread_mutex_lock(&condWaitMutex);

  pthread_mutex_lock(&sharedInfoMutex);

  if (!ShVar.TellerActive[*ParentNo-1])
    {
      pthread_mutex_unlock(&sharedInfoMutex);//unlock immidiatly

      ShVar.CondSignalSource=*ParentNo;
      pthread_cond_broadcast(&conditionVar);
    }
  else
    pthread_mutex_unlock(&sharedInfoMutex); //to avoid double unlock

  pthread_mutex_unlock(&condWaitMutex);

  pthread_exit(NULL);
  
  return 0; 
}

void *Teller(void *ThreadData)
{
  tThreadParameter *ThreadParam;
  ThreadParam=(tThreadParameter *) ThreadData;

  tCustomer nextCustomer;
  int res;
  time_t startTime;
  int remActiveTime, custProcessed, die, prevRemTime;
  int timeOut, ownProcessNo;

  startTime=initTeller();

  timeOut=ThreadParam->LiveTime;
  ownProcessNo=ThreadParam->ThreadNumber;

  remActiveTime=timeOut;

  pthread_t localthread;

  do
    {
      custProcessed=false;

//checking if rem active time<0 however there were customer that has not been
//served
      if (remActiveTime<0)
        remActiveTime=prevRemTime;
      else
        prevRemTime=remActiveTime;

      ShVar.TellerRemTime[ownProcessNo-1]=remActiveTime;

      res=getMessages(remActiveTime, &nextCustomer, ownProcessNo);

      switch (res)
        {
          case 0: //success
            {
              writeTellerLog(nextCustomer, ownProcessNo, cTELLERTAKEMODE);

              pthread_mutex_lock(&sharedInfoMutex);

              ShVar.NoOfCustomer++;
              ShVar.WaitingTime=ShVar.WaitingTime+
                timeElapsed(nextCustomer.arrivalTime);

              pthread_mutex_unlock(&sharedInfoMutex);

              if (nextCustomer.Transaction=='d')
                sleep(ThreadParam->TimeDeposit);
              else
                sleep(ThreadParam->TimeWithdraw);

              writeTellerLog(nextCustomer, ownProcessNo, cTELLERFINISHMODE);

              pthread_mutex_lock(&sharedInfoMutex);
              ShVar.TurnaroundTime=ShVar.TurnaroundTime+
                timeElapsed(nextCustomer.arrivalTime);
              pthread_mutex_unlock(&sharedInfoMutex);

              custProcessed=true;

              break;
            }
          case 302:
            {
              if (ShVar.moreCustomer==0) //there were no more customer
                {
                  die=true;
                }
              else
                {
                  pthread_mutex_lock(&sharedInfoMutex);
                  ShVar.TellerActive[ownProcessNo-1]=false;
                  pthread_mutex_unlock(&sharedInfoMutex);

                  pthread_create(&localthread, NULL, 
                    TimeOutCheckThread, (void *) &ownProcessNo);

                  pthread_mutex_lock(&condWaitMutex);
                  pthread_cond_wait(&conditionVar, &condWaitMutex);
                  pthread_mutex_unlock(&condWaitMutex); 

                  if (ShVar.CondSignalSource>=1)
                    {
//got signal from time out thread, means die
                      if ((ShVar.moreCustomer!=0) &&
                          (ShVar.noOfChildProcess>1))
                        die=true;
                    }

                  pthread_mutex_lock(&sharedInfoMutex);
                  ShVar.TellerActive[ownProcessNo-1]=true;
                  pthread_mutex_unlock(&sharedInfoMutex);
                }

            } 
        }

//calculate the remaining active time for teller
      remActiveTime=timeOut-timeElapsed(startTime);

    }
  while (die!=1);

  ShVar.TellerRunTime[ownProcessNo-1]=timeElapsed(startTime);

//wait until the local thread die
  pthread_join(localthread, NULL);
 
  doneTeller(startTime, ownProcessNo);

  return 0;
}
