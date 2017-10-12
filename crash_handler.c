#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "backtrace_wrapper.h"


#define SEP_SECTION_STR    "===================="
#define SEP_STR            "--------------------"

#define MAX_STACK_LEN      65536

void handle_crash(char* path, char* progname)
{
  char    pStackStr[65536] = { 0 };
  char*   p = NULL;
  char    szTime[32] = { 0 };
  char    szFileName[260] = { 0 };
  time_t  t;
  struct tm  *tp;
  FILE*   fp;
  int     nLen = 0;
  int     i;

  time(&t);
  tp = localtime(&t);

  sprintf(szTime, "%04d%02d%02d",  tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

  if (path == NULL || progname == NULL)
  {
    goto Err;
  }

  if (access(path, F_OK) != 0)
  {
    if (mkdir(path, 0755) < 0)
    {
      goto Err;
    }
  }

  sprintf(szFileName, "%s/%s_%s.txt", path, progname, szTime);

  nLen = MAX_STACK_LEN;

  get_stack_str(pStackStr, &nLen);

  p = pStackStr;
  for (i = 0; i < 4; i++)
  {
    p = strchr(p, '\n');
    if (p == NULL)
    {
      goto Err;
    }
    p++;
  }

  fp = fopen(szFileName, "a");
  if (fp == NULL)
  {
    goto Err;
  }

  fprintf(fp, "\n\n%s\n", SEP_SECTION_STR);
  sprintf(szTime, "%04d%02d%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
    tp->tm_hour, tp->tm_min, tp->tm_sec);
  fprintf(fp, "%s\n", szTime);
  fprintf(fp, "%s\n", SEP_STR);
  fprintf(fp, "%s", p);

  fclose(fp);

  //exit program ***important
Err:
  exit(0);
}
