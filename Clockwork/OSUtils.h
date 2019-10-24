/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: January, 2000                                         [Crossplatform]
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
#ifndef OSUTILS_H
#define OSUTILS_H 1
#include <ctime>



TIMET       CurrentTime();
TimeV       ExactTime();
void        Sleep(TimeV& hTime);
void        ExitApp(SHORT hOutCode = 0);






//###########################################################################



class TimeV
{
    //** De/constructors **//
    public: TimeV(const TimeV& x) { *this = x; }
    public: TimeV(ULONG hS = 0, ULONG hMS = 0) : mSeconds(hS), mMicroSeconds(hMS) {}

    
    //** Public Member Functions **//
    public:
    float InFloatSecs() {return ((float)mSeconds)+(((float)mMicroSeconds)/1000000);}
    double InDoubleSecs() {return ((double)mSeconds)+(((double)mMicroSeconds)/1000000);}

    
    //** Public Operator Functions **//
    public:
    TimeV   operator+ (const TimeV& hT) const
    {
        TimeV   nResTime(*this);
        nResTime.CalcMath(hT.mSeconds, hT.mMicroSeconds);
        return nResTime;
    }
    
    TimeV&  operator+=(const TimeV& hT)
    {
        CalcMath(hT.mSeconds, hT.mMicroSeconds);
        return *this;
    }
    
    TimeV   operator- (const TimeV& hT) const
    {
        TimeV   nResTime(*this);
        nResTime.CalcMath(-hT.mSeconds, -hT.mMicroSeconds);
        return nResTime;
    }
    
    TimeV&  operator-=(const TimeV& hT)
    {
        CalcMath(-hT.mSeconds, -hT.mMicroSeconds);
        return *this;
    }
    
    bool    operator==(const TimeV& x)
    {
        static TimeV sThis;
        static TimeV sThem;
        sThem = x;
        sThis = *this;
        sThem.CalcMath(0, 0);   // All of this just makes sure mMicroSec < 1 seconds
        sThis.CalcMath(0, 0);   //
        return (sThis.mSeconds == sThem.mSeconds && sThis.mMicroSeconds == sThem.mMicroSeconds);
    }
    
    bool    operator< (const TimeV& x)
    {
        static TimeV sThis;
        static TimeV sThem;
        sThem = x;
        sThis = *this;
        sThem.CalcMath(0, 0);   // All of this just makes sure mMicroSec < 1 seconds
        sThis.CalcMath(0, 0);   //
        if(sThis.mSeconds < sThem.mSeconds) return true;
        if(sThis.mSeconds > sThem.mSeconds) return false;
        if(sThis.mMicroSeconds < sThem.mMicroSeconds) return true; else return false;
    }
    
    bool    operator> (const TimeV& x)
    {
        static TimeV sThis;
        static TimeV sThem;
        sThem = x;
        sThis = *this;
        sThem.CalcMath(0, 0);   // All of this just makes sure mMicroSec < 1 seconds
        sThis.CalcMath(0, 0);   //
        if(sThis.mSeconds > sThem.mSeconds) return true;
        if(sThis.mSeconds < sThem.mSeconds) return false;
        if(sThis.mMicroSeconds > sThem.mMicroSeconds) return true; else return false;
    }
    
    void CalcMath(LONG hToSecs, LONG hToMicroSecs)
    {
        mSeconds        += hToSecs;
        mMicroSeconds   += hToMicroSecs;
        mSeconds        += mMicroSeconds/1000000;
        mMicroSeconds    = mMicroSeconds%1000000;
    
        while(mMicroSeconds < 0)
        {
            mSeconds--;
            mMicroSeconds += 1000000;
        }
    }
    
    
    //** Public Data Members **//
    public:
    LONG            mSeconds;
    LONG            mMicroSeconds;
};



#endif //ifndef OSUTILS_H


