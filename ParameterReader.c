#include "ParamReader.h"
#include "MainPrg.h"
#include <stdio.h>

const int cPARAMETERVALID = 0;
const int cINCORRECTPARAMETER = 1;
const int cPARAMETERNOTINTEGER = 2;

void writeParameterError(int errType)
{
  switch (errType)
    {
      case 1:
        { 
          fprintf(stderr, "Incorrect parameter\n");
          fprintf(stderr, "Usage   : part_1 tc t1 t2 t3 tw td\n");
          fprintf(stderr, " tc     : is an integer stating \n");
          fprintf(stderr, "          intervals in reading input\n");
          fprintf(stderr, " t1..t3 : is an integer stating \n");
          fprintf(stderr, "          live time of the teller\n");
          fprintf(stderr, " tw     : is an integer stating how long\n");
          fprintf(stderr, "          is the withdraw process take\n");
          fprintf(stderr, " td     : is an integer stating how long\n");
          fprintf(stderr, "          is the deposit process take\n\n");
        } break;
      case 2:
        {
          fprintf(stderr, "Parameter must be a positive integer\n\n");
        } break;
    }
}

int getParameter(int *ReadInterval, int* LiveTime, 
                 int *TimeWithdraw, int *TimeDeposit,
                 int argc, char** argv)
{
  int result = cPARAMETERVALID;
  int ParamCheck;

  if (argc==7)
//5 is the number of the total parameter.
//parameter 0 = file name
    {
      *ReadInterval=(int) strtoul(argv[1], NULL, 0);
      if (*ReadInterval<=0) 
        result=cPARAMETERNOTINTEGER;  
//this is for checking tc, if <0 then it is not success

      ParamCheck=1;//starting from the 1st parameter
      while ((ParamCheck<=cMAXTELLER) && (result==cPARAMETERVALID)) 
//if parameter to be checked is less than or equal than max teller
        {
          LiveTime[ParamCheck-1]=(int) strtoul(argv[ParamCheck+1], NULL, 0);
//ParamCheck+1 required because LiveTime array starts from 0, however,
//in argv, the parameter that will be read to Live Time starts from 2, 
//since the 0th is used by the file name, and the 1st is the 1st parameter

          if (LiveTime[ParamCheck-1]<=0) 
            result=cPARAMETERNOTINTEGER;

          ParamCheck++;
        }

//this is for managin time withdraw and time deposit
      *TimeWithdraw=(int) strtoul(argv[5], NULL, 0);
      *TimeDeposit=(int) strtoul(argv[6], NULL, 0);
      if ((TimeWithdraw<=0) || (TimeDeposit<=0))
        result=cPARAMETERNOTINTEGER;
    }
  else
    result = cINCORRECTPARAMETER;
  
  return result;
}
