/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                                 [MacOS]
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
#if(TARGET_MAC)
#include <SIOUX.h>              //for SIOUXSettings stuff
#include <SIOUXGlobals.h>       //for SIOUXQuitting
#include <Events.h>             //for Event stuff
#include <OSUtils.h>            //for Microseconds
#include <Processes.h>          //for ExitToShell()
#include "CommonTypesCW.h"
#include "OSUtils.h"

#define kMacToPOSIXTDiff    (((365 * 70UL) + 17) * 24 * 60 * 60)

#define POSIXTimeToMac(x)   (x - kMacToPOSIXTDiff)
#define MacTimeToPOSIX(x)   (x + kMacToPOSIXTDiff)
#define TimeVToTicks(x)     (60*x.mSeconds + (LONG)(double(x.mMicroSeconds)*0.0006f))



TIMET CurrentTime()
{
    return (TIMET) time(NULL);
}




TimeV ExactTime()
{
    TimeV           sTime;
    UInt64          nTwoPow32 = 4294967296;
    UnsignedWide    nMicroseconds;
    UInt64          n64BitMicros;
    

    Microseconds(&nMicroseconds);
    n64BitMicros  = nMicroseconds.hi * nTwoPow32;
    n64BitMicros += nMicroseconds.lo;


    sTime.mSeconds       = CurrentTime();
    sTime.mMicroSeconds  = (LONG) (n64BitMicros%1000000);
    return sTime;
}






void Sleep(TimeV& hTimeTV)
{
    INT         nEventMask = updateMask|activMask|highLevelEventMask|osMask|mDownMask|mUpMask|keyDownMask|keyUpMask;
    TimeV       nStartCycleTV;
    TimeV       nWaitedTV;
    EventRecord nEvent;
    
    
    
    do
    {
        nStartCycleTV = ExactTime();
        
        if(GetNextEvent(nEventMask, &nEvent)) // nSleepTicksInCycle, NULL))
            SIOUXHandleOneEvent(&nEvent); //Let SIOUX handle the event
    
        nWaitedTV += (ExactTime() - nStartCycleTV);
    }   
    while(nWaitedTV < hTimeTV);
}



void ExitApp(SHORT hOutCode)
{
    ExitToShell();
}





#endif //(TARGET_MAC)
