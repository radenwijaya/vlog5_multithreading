#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
//#include "SharedMemHead.h"
//#include "SemManageHead.h"

#ifndef MAINHEADER_H
#define MAINHEADER_H

#include <time.h>

extern const int false;
extern const int true;

extern const int cPARAMETERVALID;
extern const int cINCORRECTPARAMETER;
extern const int cPARAMETERNOTINTEGER;
extern const char *logFileName;

const int cMAXTELLER;

//this is used to record the customer information
typedef struct
  {
    int customerNo;
    char Transaction;
    time_t arrivalTime;
//later add time for time management
  } tCustomer;

typedef struct
  {
    int LiveTime;
    int TimeWithdraw;
    int TimeDeposit;

    int ThreadNumber;
  } tThreadParameter;

typedef struct
  {
//independent variables
    int noOfChildProcess;//number of active child process
//    int noOfMsgQueue;//to count the number of customer in the queue
    int moreCustomer;//to check whether more customer still in the file

    int TellerRemTime[3]; //this is the remaining time of running teller
    int TellerActive[3]; //this is for the active teller time

    int CondSignalSource; //this is indicating the source of conditional
                          //signal which were sent to the teller
                          //1 means another thread, 0 means customer

//statistic data
    int TellerRunTime[3];
    int NoOfCustomer;
    int WaitingTime;
    int TurnaroundTime;       
  } tShVariables;

pthread_mutex_t fileAccessMutex;
pthread_mutex_t messageMutex;
pthread_mutex_t noOfThreadMutex;
pthread_mutex_t sharedInfoMutex;
pthread_mutex_t condWaitMutex;

pthread_cond_t conditionVar;

tShVariables ShVar;

#endif
