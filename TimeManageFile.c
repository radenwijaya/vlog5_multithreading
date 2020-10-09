#include "TimeManageHead.h"
#include <time.h>

int timeElapsed(time_t past)
//this will return the time elapsed since past
//until present
{
  time_t present;
  int result;

  present=time(NULL);

  result = (int) difftime(present, past);

  return result;
}

char *formatTime(time_t time)
{
  struct tm tm;
  char *ch;

  ch=(char *) malloc(9);

  tm=*localtime(&time);
  strftime(ch, 9, "%H:%M:%S", &tm);
  
  return ch;
}
