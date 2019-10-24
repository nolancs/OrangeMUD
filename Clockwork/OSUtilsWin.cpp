/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                               [Windows]
 License: MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Copyright 2000-2019 Matthew Nolan, All Rights Reserved
******************************************************************************/
#if(TARGET_WIN)
//#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "CommonTypesCW.h"
#include "OSUtils.h"




BOOL AFXAPI AfxAssertFailedLine(LPCSTR lpszFileName, int nLine)
{
    printf("Asssert failed (%s:%d)\n", lpszFileName, nLine);
    fflush(stdout);
    return 1;
}




TIMET CurrentTime()
{
    return (TIMET) time(NULL);
}




TimeV ExactTime()
{
    struct  timeval nUnixTime;
    //gettimeofday(&nUnixTime, NULL);   
    return TimeV(nUnixTime.tv_sec, nUnixTime.tv_usec);
}






void Sleep(TimeV& hTimeTV)
{
    struct timeval nSleepTime;
    nSleepTime.tv_sec   = hTimeTV.mSeconds;
    nSleepTime.tv_usec  = hTimeTV.mMicroSeconds;
    if(select(0, NULL, NULL, NULL, &nSleepTime) == -1)
    {
        //Bad
    }
}




void ExitApp(SHORT hOutCode)
{
    exit(hOutCode);
}




#endif //(TARGET_WIN)
