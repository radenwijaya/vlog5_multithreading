//  Purpose : header file for the set of functions that is used to read 
//  parameters from a command line.

#ifndef PARAMREADER_H
#define PARAMREADER_H

extern void writeParameterError(int errType);
extern int getParameter(int *ReadInterval, int* LiveTime, 
                        int *TimeWithdraw, int *TimeDeposit,
                        int argc, char** argv);

#endif
